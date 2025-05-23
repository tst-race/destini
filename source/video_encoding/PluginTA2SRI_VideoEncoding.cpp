#include  <IEncodingComponent.h>
#include "PluginTA2SRI_VideoEncoding.h"

#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <algorithm>

#include "log.h"

#define TRACE_SRI_METHOD(...) TRACE_METHOD_BASE(PluginTA2SRI_VideoEncodingCpp, ##__VA_ARGS__)
#define TRACE_SRI_FUNCTION(...) TRACE_FUNCTION_BASE(PluginTA2SRI_VideoEncodingCpp, ##__VA_ARGS__)

const char *PluginTA2SRI_VideoEncoding::name = "SRIVideoEncoding";


PluginTA2SRI_VideoEncoding::PluginTA2SRI_VideoEncoding(IEncodingSdk *_sdk, const PluginConfig &_pluginConfig) : sdk(_sdk), pluginConfig(_pluginConfig) {
    if (this->sdk == nullptr) {
        throw std::runtime_error ("PluginTA2SRI_VideoEncoding: sdk parameter is NULL");
    }

    this->cliCodec = nullptr;

    // This does:
    CLICodec::SetDirname (this->pluginConfig.pluginDirectory);
    
    auto codecJSONPath = CLICodec::DirFilename(pluginConfig.pluginDirectory + "video_codec.json");
    auto codecJSON     = codecJSONPath.c_str ();

    // Get codec definition

    // // We should be able to use this, but it seems to be broken:
    // auto codecJSONPath = CLICodec::DirFilename ("video_codec.json");   // works out to be /etc/race/etc/<channel>

    // // Hack:  For now, the strategy is to keep all codec descriptors in the same SRI Decomposed Plugin directory.
    // // auto codecJSONPath = CLICodec::DirFilename("/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/video_codec.json");
    // auto codecJSON     = codecJSONPath.c_str ();

    if (fileExists (codecJSON)) {
         std::ifstream fJSON (codecJSON);
         cliCodec = CLICodec::GetCodecFromStream (fJSON);
         if (cliCodec == nullptr || !cliCodec->isGood ())
             throw std::runtime_error ("PluginTA2SRI_VideoEncoding: bad or incomplete JSON (" + codecJSONPath + ")");
    }
    else
        throw std::runtime_error ("PluginTA2SRI_VideoEncoding: JSON (" + codecJSONPath + ") not found");

    sdk->updateState(COMPONENT_STATE_STARTED);
}

ComponentStatus PluginTA2SRI_VideoEncoding::onUserInputReceived(RaceHandle handle, bool answered,
                                                             const std::string &response) {
    TRACE_SRI_METHOD(handle, answered, response);

    return COMPONENT_OK;
}

EncodingProperties PluginTA2SRI_VideoEncoding::getEncodingProperties() {
    TRACE_SRI_METHOD();

    return {cliCodec->encodingTime (), cliCodec->mimeType ()};
}

SpecificEncodingProperties PluginTA2SRI_VideoEncoding::getEncodingPropertiesForParameters(
    const EncodingParameters & /* params */) {
    TRACE_SRI_METHOD();

    // {static_cast <int32_t> (cliCodec->minCapacity ())};
    return {static_cast <int32_t> (3000)};
}

ComponentStatus PluginTA2SRI_VideoEncoding::encodeBytes(RaceHandle handle,
                                                     const EncodingParameters &params,
                                                     const std::vector<uint8_t> &bytes) {
    TRACE_SRI_METHOD(handle, params.linkId, params.type, params.encodePackage, params.json);

    // Per https://github.com/redboltz/mqtt_cpp/issues/854
    auto   *pMsgIn = bytes.data ();
    size_t  nMsgIn = bytes.size ();

    logInfo ("PluginTA2SRI_VideoEncoding::encodeBytes, nMsgIn: " + std::to_string (nMsgIn));

    if (nMsgIn == 0) {
    	sdk->onBytesEncoded (handle, bytes, ENCODE_OK);
		return COMPONENT_OK;
    }

    char   *pMsgOut;
    size_t  nMsgOut;

    if (cliCodec->encode (pMsgIn, nMsgIn, reinterpret_cast <void **> (&pMsgOut), &nMsgOut) /* != 0 */) {
        sdk->onBytesEncoded (handle, {}, ENCODE_FAILED);

        return COMPONENT_ERROR;
    }
    else {
        // Per https://www.appsloveworld.com/cplus/100/1168/convert-unsigned-char-into-stdvectoruint8-t
        std::vector<uint8_t> encodedBytes (pMsgOut, pMsgOut + nMsgOut);
        free (pMsgOut);

        logDebug("HASHING ENCODED BYTES");
        logDebug("encoded bytes hash " + RaceLog::stringifyValues("hash", std::hash<std::string>()(std::string(encodedBytes.begin(), encodedBytes.end()))));
        sdk->onBytesEncoded (handle, encodedBytes, ENCODE_OK);

        return COMPONENT_OK;
    }
}

ComponentStatus PluginTA2SRI_VideoEncoding::decodeBytes(RaceHandle handle,
                                                     const EncodingParameters &params,
                                                     const std::vector<uint8_t> &bytes) {
  auto thehash = std::hash<std::string>()({bytes.begin(), bytes.end()});
  TRACE_SRI_METHOD(handle, params.linkId, params.type, params.encodePackage, params.json, thehash);

    // Per https://github.com/redboltz/mqtt_cpp/issues/854
    auto   *pMsgIn = bytes.data ();
    size_t  nMsgIn = bytes.size ();
    char   *pMsgOut;
    size_t  nMsgOut;

    if (cliCodec->decode (pMsgIn, nMsgIn, reinterpret_cast <void **> (&pMsgOut), &nMsgOut) /* != 0 */) {
        sdk->onBytesDecoded (handle, {}, ENCODE_FAILED);

        return COMPONENT_ERROR;
    }
    else {
        // Per https://www.appsloveworld.com/cplus/100/1168/convert-unsigned-char-into-stdvectoruint8-t
        std::vector<uint8_t> decodedBytes (pMsgOut, pMsgOut + nMsgOut);
        free (pMsgOut);

        sdk->onBytesDecoded (handle, decodedBytes, ENCODE_OK);

        return COMPONENT_OK;
    }
}

#ifndef TESTBUILD
IEncodingComponent *createEncoding(const std::string &encoding, IEncodingSdk *sdk,
                                   const std::string &roleName, const PluginConfig &pluginConfig) {
    TRACE_FUNCTION(encoding, roleName, pluginConfig.pluginDirectory);
    return new PluginTA2SRI_VideoEncoding(sdk, pluginConfig);
}
void destroyEncoding(IEncodingComponent *component) {
    TRACE_FUNCTION();
    delete component;
}

const RaceVersionInfo raceVersion = RACE_VERSION;
#endif
