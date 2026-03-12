#include <unity.h>

#include "core/Sen66Status.h"

void setUp() {}

void tearDown() {}

void test_relevant_bits_keeps_only_supported_sen66_flags() {
    const uint32_t raw =
        Sen66Status::kFanSpeedWarning |
        Sen66Status::kPmSensorError |
        (1UL << 12) |
        (1UL << 0);

    const uint32_t relevant = Sen66Status::relevantBits(raw);

    TEST_ASSERT_EQUAL_HEX32(Sen66Status::kFanSpeedWarning | Sen66Status::kPmSensorError, relevant);
}

void test_collect_transitions_reports_new_warning_and_error() {
    Sen66Status::Transition transitions[4] = {};
    const size_t count =
        Sen66Status::collectTransitions(0,
                                        Sen66Status::kFanSpeedWarning | Sen66Status::kPmSensorError,
                                        transitions,
                                        4);

    TEST_ASSERT_EQUAL_UINT32(2, count);
    TEST_ASSERT_EQUAL(Logger::Warn, transitions[0].level);
    TEST_ASSERT_EQUAL_STRING("fan speed warning", transitions[0].message);
    TEST_ASSERT_EQUAL(Logger::Error, transitions[1].level);
    TEST_ASSERT_EQUAL_STRING("PM sensor error", transitions[1].message);
}

void test_collect_transitions_reports_warning_recovery_only_for_non_sticky_flag() {
    Sen66Status::Transition transitions[4] = {};
    const size_t count =
        Sen66Status::collectTransitions(Sen66Status::kFanSpeedWarning | Sen66Status::kPmSensorError,
                                        Sen66Status::kPmSensorError,
                                        transitions,
                                        4);

    TEST_ASSERT_EQUAL_UINT32(1, count);
    TEST_ASSERT_EQUAL(Logger::Info, transitions[0].level);
    TEST_ASSERT_EQUAL_STRING("fan speed warning cleared", transitions[0].message);
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_relevant_bits_keeps_only_supported_sen66_flags);
    RUN_TEST(test_collect_transitions_reports_new_warning_and_error);
    RUN_TEST(test_collect_transitions_reports_warning_recovery_only_for_non_sticky_flag);
    return UNITY_END();
}
