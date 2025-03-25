#ifndef __RACE_SEND_TYPE_H_
#define __RACE_SEND_TYPE_H_

#include <string>

enum SendType {
    ST_UNDEF = 0,  // undefined
    ST_STORED_ASYNC =
        1,  // sent messages are stored but there is no way to know the other side is ready
    ST_EPHEM_SYNC = 2  // the other side must be ready, but the link will know when that occurs
};

/**
 * @brief Convert a SendType value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param sendType The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string sendTypeToString(SendType sendType);
std::ostream &operator<<(std::ostream &out, SendType sendType);

// TODO: documentation
SendType sendTypeFromString(const std::string &sendTypeString);

#endif
