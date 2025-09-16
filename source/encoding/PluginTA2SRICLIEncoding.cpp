#include  <IEncodingComponent.h>
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


PluginTA2SRICLIEncoding::PluginTA2SRICLIEncoding(IEncodingSdk *_sdk, const PluginConfig &_pluginConfig) : 
    sdk(_sdk), 
    pluginConfig(_pluginConfig),
    overrideCommonArgs(""),
    hasCommonArgsOverride(false),
    commonArgsHandle(NULL_RACE_HANDLE),
    commonArgsReceived(false) {
    
    if (this->sdk == nullptr) {
        throw std::runtime_error ("PluginTA2SRICLIEncoding: sdk parameter is NULL");
    }

    this->cliCodec = nullptr;
    CLICodec::SetDirname (this->pluginConfig.pluginDirectory);

    // Get codec definition
    logDebug ("PluginTA2SRICLIEncoding:: PluginConfig: etcDirectory " + _pluginConfig.etcDirectory);
    logDebug ("PluginTA2SRICLIEncoding:: PluginConfig: loggingDirectory " + _pluginConfig.loggingDirectory);
    logDebug ("PluginTA2SRICLIEncoding:: PluginConfig: auxDataDirectory " + _pluginConfig.auxDataDirectory);
    logDebug ("PluginTA2SRICLIEncoding:: PluginConfig: tmpDirectory " + _pluginConfig.tmpDirectory);
    logDebug ("PluginTA2SRICLIEncoding:: PluginConfig: pluginDirectory " + _pluginConfig.pluginDirectory);
    
    CLICodec::SetDirname(pluginConfig.pluginDirectory);
    auto codecJSONPath = CLICodec::DirFilename("codec.json");
    auto codecJSON     = codecJSONPath.c_str ();

    if (fileExists (codecJSON)) {
         std::ifstream fJSON (codecJSON);
         cliCodec = CLICodec::GetCodecFromStream (fJSON);
         if (cliCodec == nullptr || !cliCodec->isGood ())
             throw std::runtime_error ("PluginTA2SRICLIEncoding: bad or incomplete JSON (" + codecJSONPath + ")");
    }
    else
        throw std::runtime_error ("PluginTA2SRICLIEncoding: JSON (" + codecJSONPath + ") not found");

    // Request user input for argument overrides
    commonArgsHandle = sdk->requestPluginUserInput(
        "commonArgs",
        "Enter common arguments override (or leave empty to use defaults from codec.json):",
        false  // Not required - can be empty
    ).handle;

    logDebug("PluginTA2SRICLIEncoding: Requested user input for argument overrides");
    logDebug("PluginTA2SRICLIEncoding: Common args handle: " + std::to_string(commonArgsHandle));
}

ComponentStatus PluginTA2SRICLIEncoding::onUserInputReceived(RaceHandle handle, bool answered,
                                                             const std::string &response) {
    TRACE_SRI_METHOD(handle, answered, response);

    logDebug("PluginTA2SRICLIEncoding: Received user input - Handle: " + std::to_string(handle) + 
             ", Answered: " + (answered ? "true" : "false") + 
             ", Response length: " + std::to_string(response.length()));

    if (handle == commonArgsHandle) {
        commonArgsReceived = true;
        
        if (answered && !response.empty()) {
            overrideCommonArgs = response;
            hasCommonArgsOverride = true;
            logInfo("PluginTA2SRICLIEncoding: Common args override set to: '" + response + "'");
            
            // Since common args changed, we need to recalculate all capacities
            if (cliCodec != nullptr) {
                logInfo("PluginTA2SRICLIEncoding: Common args overridden, triggering capacity recalculation");
                cliCodec->recalculateCapacities(overrideCommonArgs);
            }
        } else {
            hasCommonArgsOverride = false;
            logDebug("PluginTA2SRICLIEncoding: No common args override provided, will use codec.json defaults");
        }
        
        // Pass the override arguments to the codec
        if (cliCodec != nullptr) {
            cliCodec->setArgumentOverrides(
                hasCommonArgsOverride ? overrideCommonArgs : "",
                hasCommonArgsOverride
            );
            logDebug("PluginTA2SRICLIEncoding: All user inputs received, updated codec with overrides");
        }
        
        // Update component state to indicate we're ready
        sdk->updateState(COMPONENT_STATE_STARTED);
        
    } else {
        logWarning("PluginTA2SRICLIEncoding: Received unexpected handle: " + std::to_string(handle));
        return COMPONENT_ERROR;
    }

    return COMPONENT_OK;
}

EncodingProperties PluginTA2SRICLIEncoding::getEncodingProperties() {
    TRACE_SRI_METHOD();

    return {cliCodec->encodingTime (), cliCodec->mimeType ()};
}

SpecificEncodingProperties PluginTA2SRICLIEncoding::getEncodingPropertiesForParameters(const EncodingParameters &params) {
    TRACE_SRI_METHOD(params.type, params.linkId);
    
    // Get the actual minimum capacity from MediaPaths instead of hardcoded 3000
    size_t capacity = getMinimumCapacity();
    
    logDebug("PluginTA2SRICLIEncoding::getEncodingPropertiesForParameters: Using capacity " + std::to_string(capacity));
    
    return {static_cast <int32_t>(capacity)};
}

size_t PluginTA2SRICLIEncoding::getMinimumCapacity() const {
    if (cliCodec == nullptr) {
        logWarning("PluginTA2SRICLIEncoding::getMinimumCapacity: cliCodec is null, returning default 3000");
        return 3000;
    }
    
    size_t minCapacity = cliCodec->getMinimumCapacity();
    if (minCapacity == 0) {
        logWarning("PluginTA2SRICLIEncoding::getMinimumCapacity: No valid capacities found, returning default 3000");
        return 3000;
    }
    
    logDebug("PluginTA2SRICLIEncoding::getMinimumCapacity: returning " + std::to_string(minCapacity));
    return minCapacity;
}

ComponentStatus PluginTA2SRICLIEncoding::encodeBytes(RaceHandle handle,
                                                     const EncodingParameters &params,
                                                     const std::vector<uint8_t> &bytes) {
    TRACE_SRI_METHOD(handle, params.linkId, params.type, params.encodePackage, params.json);

    // Per https://github.com/redboltz/mqtt_cpp/issues/854
    auto   *pMsgIn = bytes.data ();
    size_t  nMsgIn = bytes.size ();

    logDebug ("PluginTA2SRICLIEncoding::encodeBytes, nMsgIn: " + std::to_string (nMsgIn));

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

#ifndef TESTBUILD
IEncodingComponent *createEncoding(const std::string &encoding, IEncodingSdk *sdk,
                                   const std::string &roleName, const PluginConfig &pluginConfig) {
    TRACE_FUNCTION(encoding, roleName, pluginConfig.pluginDirectory);
    return new PluginTA2SRICLIEncoding(sdk, pluginConfig);
}
void destroyEncoding(IEncodingComponent *component) {
    TRACE_FUNCTION();
    delete component;
}

const RaceVersionInfo raceVersion = RACE_VERSION;
#endif
