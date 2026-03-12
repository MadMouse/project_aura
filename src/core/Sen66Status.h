// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/Logger.h"

namespace Sen66Status {

constexpr uint32_t kFanSpeedWarning = (1UL << 21);
constexpr uint32_t kPmSensorError = (1UL << 11);
constexpr uint32_t kCo2SensorError = (1UL << 9);
constexpr uint32_t kGasSensorError = (1UL << 7);
constexpr uint32_t kRhTemperatureError = (1UL << 6);
constexpr uint32_t kFanError = (1UL << 4);

constexpr uint32_t kRelevantMask =
    kFanSpeedWarning |
    kPmSensorError |
    kCo2SensorError |
    kGasSensorError |
    kRhTemperatureError |
    kFanError;

struct FlagDescriptor {
    uint32_t mask;
    Logger::Level active_level;
    const char *active_message;
    const char *cleared_message;
    bool sticky;
};

struct Transition {
    Logger::Level level;
    const char *message;
};

constexpr FlagDescriptor kFlagDescriptors[] = {
    {kFanSpeedWarning, Logger::Warn, "fan speed warning", "fan speed warning cleared", false},
    {kPmSensorError, Logger::Error, "PM sensor error", nullptr, true},
    {kCo2SensorError, Logger::Error, "CO2 sensor error", nullptr, true},
    {kGasSensorError, Logger::Error, "gas sensor error", nullptr, true},
    {kRhTemperatureError, Logger::Error, "RH&T sensor error", nullptr, true},
    {kFanError, Logger::Error, "fan error", nullptr, true},
};

inline uint32_t relevantBits(uint32_t raw_status) {
    return raw_status & kRelevantMask;
}

inline size_t collectTransitions(uint32_t previous_status,
                                 uint32_t current_status,
                                 Transition *out,
                                 size_t max_entries) {
    if (!out || max_entries == 0) {
        return 0;
    }

    const uint32_t prev_bits = relevantBits(previous_status);
    const uint32_t curr_bits = relevantBits(current_status);

    size_t count = 0;
    for (const FlagDescriptor &descriptor : kFlagDescriptors) {
        const bool was_set = (prev_bits & descriptor.mask) != 0;
        const bool is_set = (curr_bits & descriptor.mask) != 0;
        if (is_set && !was_set) {
            if (count >= max_entries) {
                break;
            }
            out[count++] = {descriptor.active_level, descriptor.active_message};
            continue;
        }
        if (!is_set && was_set && !descriptor.sticky && descriptor.cleared_message) {
            if (count >= max_entries) {
                break;
            }
            out[count++] = {Logger::Info, descriptor.cleared_message};
        }
    }

    return count;
}

} // namespace Sen66Status
