#ifndef SOURCE_ENCODING_PLUGINSRICLIENCODING_H
#define SOURCE_ENCODING_PLUGINSRICLIENCODING_H

#include <IEncodingComponent.h>

#include <string>

#include "CLICodec.h"


class PluginTA2SRICLIEncoding : public IEncodingComponent {
public:
    explicit PluginTA2SRICLIEncoding(IEncodingSdk *sdk, const PluginConfig &pluginConfig);

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

    // Add these private members to store override arguments and handles
    std::string overrideCommonArgs;
    bool hasCommonArgsOverride = false;

    // RaceHandles for user input requests
    RaceHandle commonArgsHandle = NULL_RACE_HANDLE;

    // Track if inputs have been received
    bool commonArgsReceived = false;

    // Add method to get minimum capacity
    size_t getMinimumCapacity() const;
};

#endif
