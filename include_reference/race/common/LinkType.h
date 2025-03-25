#ifndef __RACE_LINK_TYPE_H_
#define __RACE_LINK_TYPE_H_

#include <string>

enum LinkType {
    LT_UNDEF = 0,  // undefined
    LT_SEND = 1,   // send
    LT_RECV = 2,   // receive
    LT_BIDI = 3    // bi-directional
};

/**
 * @brief Convert a LinkType value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param linkType The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string linkTypeToString(LinkType linkType);

std::ostream &operator<<(std::ostream &out, LinkType linkType);

#endif
