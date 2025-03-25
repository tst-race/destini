#pragma once

#include <cstdint>
#include <string>

#include "../common/RaceHandle.h"

extern "C" {

enum SdkStatus {
    SDK_INVALID = 0,           // default / undefined status
    SDK_OK = 1,                // OK
    SDK_SHUTTING_DOWN = 2,     //  shutting down
    SDK_PLUGIN_MISSING = 3,    // plugin missing
    SDK_INVALID_ARGUMENT = 4,  // invalid argument
    SDK_QUEUE_FULL = 5         // plugin / connection queue is full
};

struct SdkResponseC {
    SdkStatus status;
    double queueUtilization;  // proportion of queue current in use
    RaceHandle handle;
};
}

class SdkResponse : public SdkResponseC {
public:
    SdkResponse();
    // cppcheck-suppress noExplicitConstructor
    SdkResponse(SdkStatus _status);
    SdkResponse(SdkStatus _status, double _queueUtilization, RaceHandle _handle);
};

/**
 * @brief Convert a SdkStatus value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param sdkStatus The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string sdkStatusToString(SdkStatus sdkStatus);
std::ostream &operator<<(std::ostream &out, SdkStatus sdkStatus);
