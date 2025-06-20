#ifndef SOURCE_ENCODING_PLUGINSRIVIDEOENCODING_H
#define SOURCE_ENCODING_PLUGINSRIVIDEOENCODING_H

#include <IEncodingComponent.h>

#include <string>

#include "CLICodec.h"


class PluginTA2SRI_VideoEncoding : public IEncodingComponent {
public:
    explicit PluginTA2SRI_VideoEncoding(IEncodingSdk *sdk, const PluginConfig &pluginConfig);

    virtual ComponentStatus onUserInputReceived(RaceHandle handle, bool answered,
                                                const std::string &response) override;

    virtual EncodingProperties getEncodingProperties() override;

    virtual SpecificEncodingProperties getEncodingPropertiesForParameters(
        const EncodingParameters &params) override;

    virtual ComponentStatus encodeBytes(RaceHandle handle, const EncodingParameters &params,
                                        const std::vector<uint8_t> &bytes) override;

    virtual ComponentStatus decodeBytes(RaceHandle handle, const EncodingParameters &params,
                                        const std::vector<uint8_t> &bytes) override;

public:
    static const char *name;

private:
    IEncodingSdk       *sdk;
    const PluginConfig &pluginConfig;
    CLICodec           *cliCodec;
};

#endif
