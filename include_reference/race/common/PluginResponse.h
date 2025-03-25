#ifndef __RACE_PLUGIN_RESPONSE_H_
#define __RACE_PLUGIN_RESPONSE_H_

#include <string>

enum PluginResponse {
    PLUGIN_INVALID = 0,     // default / undefined status
    PLUGIN_OK = 1,          // OK
    PLUGIN_TEMP_ERROR = 2,  // temporary error
    PLUGIN_ERROR = 3,       // error
    PLUGIN_FATAL = 4        // fatal error
};

/**
 * @brief Convert a PluginResponse value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param pluginResponse The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string pluginResponseToString(PluginResponse pluginResponse);

std::ostream &operator<<(std::ostream &out, PluginResponse pluginResponse);

#endif
