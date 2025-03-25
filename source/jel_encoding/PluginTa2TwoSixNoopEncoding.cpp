#include "PluginTa2TwoSixNoopEncoding.h"

#include "log.h"

const char *PluginTa2TwoSixNoopEncoding::name = "noop";

PluginTa2TwoSixNoopEncoding::PluginTa2TwoSixNoopEncoding(IEncodingSdk *_sdk) : sdk(_sdk) {
    if (this->sdk == nullptr) {
        throw std::runtime_error("PluginTa2TwoSixNoopEncoding: sdk parameter is NULL");
    }
    sdk->updateState(COMPONENT_STATE_STARTED);
}

ComponentStatus PluginTa2TwoSixNoopEncoding::onUserInputReceived(RaceHandle handle, bool answered,
                                                                 const std::string &response) {
    TRACE_METHOD(handle, answered, response);

    return COMPONENT_OK;
}

EncodingProperties PluginTa2TwoSixNoopEncoding::getEncodingProperties() {
    TRACE_METHOD();

    return {0, "application/octet-stream"};
}

SpecificEncodingProperties PluginTa2TwoSixNoopEncoding::getEncodingPropertiesForParameters(
    const EncodingParameters & /* params */) {
    TRACE_METHOD();

    return {1000000};
}

ComponentStatus PluginTa2TwoSixNoopEncoding::encodeBytes(RaceHandle handle,
                                                         const EncodingParameters &params,
                                                         const std::vector<uint8_t> &bytes) {
    TRACE_METHOD(handle, params.linkId, params.type, params.encodePackage, params.json);

    // TODO: do I need to thread this? Or should I just for exemplary sake?
    sdk->onBytesEncoded(handle, bytes, ENCODE_OK);

    return COMPONENT_OK;
}

ComponentStatus PluginTa2TwoSixNoopEncoding::decodeBytes(RaceHandle handle,
                                                         const EncodingParameters &params,
                                                         const std::vector<uint8_t> &bytes) {
    TRACE_METHOD(handle, params.linkId, params.type, params.encodePackage, params.json);

    // TODO: do I need to thread this? Or should I just for exemplary sake?
    sdk->onBytesDecoded(handle, bytes, ENCODE_OK);

    return COMPONENT_OK;
}
