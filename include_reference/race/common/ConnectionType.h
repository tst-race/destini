#ifndef __RACE_CONNECTION_TYPE_H_
#define __RACE_CONNECTION_TYPE_H_

#include <string>

enum ConnectionType {
    CT_UNDEF = 0,     // undefined
    CT_DIRECT = 1,    // direct connection
    CT_INDIRECT = 2,  // indirect connection
    CT_MIXED = 3,     // link both reveals its IP to other node and contacts a 3rd-party service
    CT_LOCAL = 4      // link is physically local (e.g. a WiFi hotspot)
};

/**
 * @brief Convert a ConnectionType value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param connectionType The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string connectionTypeToString(ConnectionType connectionType);

// TODO: documentation
ConnectionType connectionTypeFromString(const std::string &connectionTypeString);

std::ostream &operator<<(std::ostream &out, ConnectionType connectionType);

#endif
