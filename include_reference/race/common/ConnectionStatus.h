#ifndef __RACE_CONNECTION_STATUS_H_
#define __RACE_CONNECTION_STATUS_H_

#include <iostream>
#include <string>

enum ConnectionStatus {
    CONNECTION_INVALID = 0,           // default / undefined status
    CONNECTION_OPEN = 1,              // opened
    CONNECTION_CLOSED = 2,            // closed
    CONNECTION_AWAITING_CONTACT = 3,  // awaiting contact
    CONNECTION_INIT_FAILED = 4,       // init failed
    CONNECTION_AVAILABLE = 5,         // available
    CONNECTION_UNAVAILABLE = 6        // unavailable
};

/**
 * @brief Convert a ConnectionStatus value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param connectionStatus The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string connectionStatusToString(ConnectionStatus connectionStatus);

std::ostream &operator<<(std::ostream &out, ConnectionStatus connectionStatus);

#endif
