#ifndef __SOURCE_ENCODING_PLUGINTA2TWOSIXNOOPENCODING_H__
#define __SOURCE_ENCODING_PLUGINTA2TWOSIXNOOPENCODING_H__

#include <IEncodingComponent.h>

#include <string>

class PluginTa2TwoSixNoopEncoding : public IEncodingComponent {
public:
    explicit PluginTa2TwoSixNoopEncoding(IEncodingSdk *sdk);

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
    IEncodingSdk *sdk;
};

#endif
