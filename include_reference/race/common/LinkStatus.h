#ifndef __RACE_LINK_STATUS_H_
#define __RACE_LINK_STATUS_H_

#include <iostream>
#include <string>

enum LinkStatus {
    LINK_UNDEF = 0,     // default / undefined status
    LINK_CREATED = 1,   // opened
    LINK_LOADED = 2,    // opened
    LINK_DESTROYED = 3  // closed
};

/**
 * @brief Convert a LinkStatus value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param linkStatus The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string linkStatusToString(LinkStatus linkStatus);

std::ostream &operator<<(std::ostream &out, LinkStatus linkStatus);

#endif
