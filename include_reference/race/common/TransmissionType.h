#ifndef __RACE_TRANSMISSION_TYPE_H_
#define __RACE_TRANSMISSION_TYPE_H_

#include <string>

enum TransmissionType {
    TT_UNDEF = 0,     // undefined
    TT_UNICAST = 1,   // unicast
    TT_MULTICAST = 2  // multicast enabled as of v1.1.0
};

/**
 * @brief Convert a TransmissionType value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param transmissionType The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string transmissionTypeToString(TransmissionType transmissionType);

// TODO: documentation
TransmissionType transmissionTypeFromString(const std::string &transmissionTypeString);

std::ostream &operator<<(std::ostream &out, TransmissionType transmissionType);

#endif
