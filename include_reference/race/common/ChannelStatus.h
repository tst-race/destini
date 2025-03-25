#ifndef __RACE_CHANNEL_STATUS_H_
#define __RACE_CHANNEL_STATUS_H_

#include <string>

enum ChannelStatus {
    CHANNEL_UNDEF = 0,        // default / undefined status
    CHANNEL_AVAILABLE = 1,    // available
    CHANNEL_UNAVAILABLE = 2,  // unavailable
    CHANNEL_ENABLED = 3,
    CHANNEL_DISABLED = 4,
    CHANNEL_STARTING = 5,
    CHANNEL_FAILED = 6,
    CHANNEL_UNSUPPORTED = 7,
};

/**
 * @brief Convert a ChannelStatus value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param channelStatus The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string channelStatusToString(ChannelStatus channelStatus);

std::ostream &operator<<(std::ostream &out, ChannelStatus channelStatus);

#endif
