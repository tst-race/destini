#ifndef __RACE_PACKAGE_STATUS_H_
#define __RACE_PACKAGE_STATUS_H_

#include <string>

enum PackageStatus {
    PACKAGE_INVALID = 0,               // default / undefined status
    PACKAGE_SENT = 1,                  // sent
    PACKAGE_RECEIVED = 2,              // received
    PACKAGE_FAILED_GENERIC = 3,        // failed
    PACKAGE_FAILED_NETWORK_ERROR = 4,  // failed due to network error
    PACKAGE_FAILED_TIMEOUT = 5         // package timed out before sending
};

/**
 * @brief Convert a PackageStatus value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param packageStatus The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string packageStatusToString(PackageStatus packageStatus);

std::ostream &operator<<(std::ostream &out, PackageStatus packageStatus);

#endif
