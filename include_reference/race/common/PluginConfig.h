#ifndef __PLUGIN_CONFIG_H_
#define __PLUGIN_CONFIG_H_

#include <string>

/**
 * Various config variables passed by the sdk to the plugins
 *
 * These variables are dynamic and depend on runtime conditions, e.g. platform
 */
class PluginConfig {
public:
    std::string etcDirectory;
    std::string loggingDirectory;
    std::string auxDataDirectory;
    std::string tmpDirectory;
    std::string pluginDirectory;
};

inline bool operator==(const PluginConfig &lhs, const PluginConfig &rhs) {
    return lhs.etcDirectory == rhs.etcDirectory && lhs.loggingDirectory == rhs.loggingDirectory &&
           lhs.auxDataDirectory == rhs.auxDataDirectory && lhs.tmpDirectory == rhs.tmpDirectory &&
           lhs.pluginDirectory == rhs.pluginDirectory;
}

inline bool operator!=(const PluginConfig &lhs, const PluginConfig &rhs) {
    return !operator==(lhs, rhs);
}

#endif
