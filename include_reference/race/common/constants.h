#pragma once

#include <cstdint>
#include <limits>

/**
 * @brief This constant may be used to specify that a function taking a timeout should never time
 * out.
 */
static const int32_t RACE_BLOCKING = std::numeric_limits<int32_t>::min();
