#include "PluginTA2SRI_VideoEncoding.h"

#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "log.h"

#define TRACE_SRI_METHOD(...) TRACE_METHOD_BASE(PluginTA2SRI_VideoEncodingCpp, ##__VA_ARGS__)
#define TRACE_SRI_FUNCTION(...) TRACE_FUNCTION_BASE(PluginTA2SRI_VideoEncodingCpp, ##__VA_ARGS__)

const char *PluginTA2SRI_VideoEncoding::name = "SRIVideoEncoding";


PluginTA2SRI_VideoEncoding::PluginTA2SRI_VideoEncoding(IEncodingSdk *_sdk, const PluginConfig &_pluginConfig) : sdk(_sdk), pluginConfig(_pluginConfig) {
    if (this->sdk == nullptr) {
        throw std::runtime_error ("PluginTA2SRI_VideoEncoding: sdk parameter is NULL");
    }

    this->cliCodec = nullptr;

#if 0
    // Not sure why this isn't working, but will have to test & fix:
    if (this->pluginConfig.pluginDirectory.length () /* > 0 */)
      // Old:
      // JELCodec::SetDirname ("/usr/local/lib/race/ta2/PluginTA2SRIDecomposed");
      // But let's put encoder-related stuff in /etc/race:
      JELCodec::SetDirname ("/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed");
    else
#endif
      ///////////////////////////////////////////////////////////////
      /* Discrepancy in PluginConfig.h - There are two versions of
	 PluginConfig.h.  One is missing the slot "pluginDirectory,
	 while the other throws a huge pile of compile errors in
	 nlohmann/json.cpp */
      
      // This does not exist:
      // CLICodec::SetDirname (this->pluginConfig.pluginDirectory);

      // This does:
      CLICodec::SetDirname (this->pluginConfig.etcDirectory);

    // Get codec definition

    // We should be able to use this, but it seems to be broken:
    auto codecJSONPath = CLICodec::DirFilename ("video_codec.json");   // works out to be /etc/race/etc/<channel>

    // Hack:  For now, the strategy is to keep all codec descriptors in the same SRI Decomposed Plugin directory.
    // auto codecJSONPath = CLICodec::DirFilename("/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/video_codec.json");
    auto codecJSON     = codecJSONPath.c_str ();

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

        sdk->onBytesEncoded (handle, encodedBytes, ENCODE_OK);

        return COMPONENT_OK;
    }
}

ComponentStatus PluginTA2SRI_VideoEncoding::decodeBytes(RaceHandle handle,
                                                     const EncodingParameters &params,
                                                     const std::vector<uint8_t> &bytes) {
    TRACE_SRI_METHOD(handle, params.linkId, params.type, params.encodePackage, params.json);

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
