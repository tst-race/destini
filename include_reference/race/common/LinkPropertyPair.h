#ifndef __LINK_PROPERTY_PAIR_H_
#define __LINK_PROPERTY_PAIR_H_

#include <cstdint>
#include <string>
#include <vector>

#include "LinkPropertySet.h"

class LinkPropertyPair {
public:
    /**
     * @brief Properties for the send side of the link
     *
     */
    LinkPropertySet send;
    /**
     * @brief Properties for the receive side of the link
     *
     */
    LinkPropertySet receive;
};

inline bool operator==(const LinkPropertyPair &a, const LinkPropertyPair &b) {
    return a.send == b.send && a.receive == b.receive;
}
inline bool operator!=(const LinkPropertyPair &a, const LinkPropertyPair &b) {
    return !(a == b);
}

/**
 * @brief Convert a LinkPropertyPair object to a human-readable string. This function is strictly
 * for logging and debugging. The output formatting may change without notice. Do NOT use this for
 * any logical comparisons, etc. The functionality of your plugin should in no way rely on the
 * output of this function.
 *
 * @param props The LinkPropertyPair object to convert to a string.
 * @return std::string The string representation of the provided object.
 */
std::string linkPropertyPairToString(const LinkPropertyPair &props);

#endif
