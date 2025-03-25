#include "PluginTA2SRICLIEncoding.h"

#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "log.h"

#define TRACE_SRI_METHOD(...) TRACE_METHOD_BASE(PluginTA2SRICLIEncodingCpp, ##__VA_ARGS__)
#define TRACE_SRI_FUNCTION(...) TRACE_FUNCTION_BASE(PluginTA2SRICLIEncodingCpp, ##__VA_ARGS__)

const char *PluginTA2SRICLIEncoding::name = "SRICLIEncoding";


PluginTA2SRICLIEncoding::PluginTA2SRICLIEncoding(IEncodingSdk *_sdk, const PluginConfig &_pluginConfig) : sdk(_sdk), pluginConfig(_pluginConfig) {
    if (this->sdk == nullptr) {
        throw std::runtime_error ("PluginTA2SRICLIEncoding: sdk parameter is NULL");
    }

    this->cliCodec = nullptr;

#if 0
    if (this->pluginConfig.pluginDirectory.length () /* > 0 */)
      CLICodec::SetDirname ("/usr/local/lib/race/ta2/PluginTA2SRIDecomposed");
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

    // auto codecJSONPath = CLICodec::DirFilename ("codec.json");
    // Horrible hack:
    auto codecJSONPath = CLICodec::DirFilename("/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/codec.json");
    auto codecJSON     = codecJSONPath.c_str ();

    // The codec.json file contains info about how to do the encoding
    // decoding, but is wired for JPEG.  What's the best way to extend
    // this to video?  We could add entries to codec.json

    if (fileExists (codecJSON)) {
         std::ifstream fJSON (codecJSON);
         cliCodec = CLICodec::GetCodecFromStream (fJSON);
         if (cliCodec == nullptr || !cliCodec->isGood ())
             throw std::runtime_error ("PluginTA2SRICLIEncoding: bad or incomplete JSON (" + codecJSONPath + ")");
    }
    else
        throw std::runtime_error ("PluginTA2SRICLIEncoding: JSON (" + codecJSONPath + ") not found");

    sdk->updateState(COMPONENT_STATE_STARTED);
}

ComponentStatus PluginTA2SRICLIEncoding::onUserInputReceived(RaceHandle handle, bool answered,
                                                             const std::string &response) {
    TRACE_SRI_METHOD(handle, answered, response);

    return COMPONENT_OK;
}

EncodingProperties PluginTA2SRICLIEncoding::getEncodingProperties() {
    TRACE_SRI_METHOD();

    return {cliCodec->encodingTime (), cliCodec->mimeType ()};
}

SpecificEncodingProperties PluginTA2SRICLIEncoding::getEncodingPropertiesForParameters(
    const EncodingParameters & /* params */) {
    TRACE_SRI_METHOD();

    // {static_cast <int32_t> (cliCodec->minCapacity ())};
    return {static_cast <int32_t> (3000)};
}

ComponentStatus PluginTA2SRICLIEncoding::encodeBytes(RaceHandle handle,
                                                     const EncodingParameters &params,
                                                     const std::vector<uint8_t> &bytes) {
    TRACE_SRI_METHOD(handle, params.linkId, params.type, params.encodePackage, params.json);

    // Per https://github.com/redboltz/mqtt_cpp/issues/854
    auto   *pMsgIn = bytes.data ();
    size_t  nMsgIn = bytes.size ();

    logInfo ("PluginTA2SRICLIEncoding::encodeBytes, nMsgIn: " + std::to_string (nMsgIn));

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

ComponentStatus PluginTA2SRICLIEncoding::decodeBytes(RaceHandle handle,
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
