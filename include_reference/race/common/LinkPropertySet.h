#ifndef __LINK_PROPERTY_SET_H_
#define __LINK_PROPERTY_SET_H_

#include <cstdint>
#include <string>
#include <vector>

class LinkPropertySet {
public:
    LinkPropertySet() : bandwidth_bps(-1), latency_ms(-1), loss(-1.) {}

public:
    /**
     * @brief Bandwidth measured in bits per second. If the value is negative then the value
     * is either unset or unknown for this link.
     *
     */
    std::int32_t bandwidth_bps;
    /**
     * @brief latency measured in milliseconds, includes estimated Comms processing time on both
     * sides. If the value is negative then the value is either unset or unknown for this link.
     *
     */
    std::int32_t latency_ms;
    /**
     * @brief loss measured in chance-of-loss per-unit. If the value is negative then the value
     * is either unset or unknown for this link.
     *
     */
    float loss;
};

inline bool operator==(const LinkPropertySet &a, const LinkPropertySet &b) {
    return a.bandwidth_bps == b.bandwidth_bps && a.latency_ms == b.latency_ms && a.loss == b.loss;
}
inline bool operator!=(const LinkPropertySet &a, const LinkPropertySet &b) {
    return a.bandwidth_bps != b.bandwidth_bps || a.latency_ms != b.latency_ms || a.loss != b.loss;
}

/**
 * @brief Convert a LinkPropertySet object to a human-readable string. This function is strictly
 * for logging and debugging. The output formatting may change without notice. Do NOT use this for
 * any logical comparisons, etc. The functionality of your plugin should in no way rely on the
 * output of this function.
 *
 * @param props The LinkPropertySet object to convert to a string.
 * @return std::string The string representation of the provided object.
 */
std::string linkPropertySetToString(const LinkPropertySet &props);

#endif
