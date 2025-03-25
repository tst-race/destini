#ifndef __RACE_PACKAGE_TYPE_H_
#define __RACE_PACKAGE_TYPE_H_

#include <string>

enum PackageType {
    PKG_TYPE_UNDEF = 0,            // undefined
    PKG_TYPE_NETWORK_MANAGER = 1,  // NetworkManager package
    PKG_TYPE_TEST_HARNESS = 2,     // test harness package
    PKG_TYPE_SDK = 3,              // Core (e.g., bootstrapping) package
};

/**
 * @brief Convert a PackageType value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param packageType The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string packageTypeToString(PackageType packageType);

std::ostream &operator<<(std::ostream &out, PackageType packageType);

#endif
