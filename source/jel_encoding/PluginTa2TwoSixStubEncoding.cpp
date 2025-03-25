#include <IEncodingComponent.h>
#include <string.h>  // strcmp

#include "PluginTa2TwoSixBase64Encoding.h"
#include "PluginTa2TwoSixNoopEncoding.h"
#include "PluginTA2SRI_JELEncoding.h"
#include "log.h"

#ifndef TESTBUILD
IEncodingComponent *createEncoding(const std::string &encoding, IEncodingSdk *sdk,
                                   const std::string &roleName,
                                   const PluginConfig &pluginConfig) {
    TRACE_FUNCTION(encoding, roleName);

    if (sdk == nullptr) {
        RaceLog::logError(logPrefix, "`sdk` parameter is set to NULL.", "");
        return nullptr;
    }

    if (encoding == PluginTa2TwoSixNoopEncoding::name) {
        return new PluginTa2TwoSixNoopEncoding(sdk);
    } else if (encoding == PluginTa2TwoSixBase64Encoding::name) {
        return new PluginTa2TwoSixBase64Encoding(sdk);
    } else if (encoding == PluginTA2SRI_JELEncoding::name) {
        return new PluginTA2SRI_JELEncoding(sdk, pluginConfig);
    } else {
        RaceLog::logError(logPrefix, "invalid encoding type: " + std::string(encoding), "");
        return nullptr;
    }
}
void destroyEncoding(IEncodingComponent *component) {
    TRACE_FUNCTION();
    delete component;
}

const RaceVersionInfo raceVersion = RACE_VERSION;
#endif
