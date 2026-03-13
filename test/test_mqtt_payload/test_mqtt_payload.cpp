#include <unity.h>
#include <string.h>

#include "config/AppData.h"
#include "modules/MqttPayloadBuilder.h"

namespace {

void assert_contains(const String &text, const char *needle) {
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(text.c_str(), needle), needle);
}

} // namespace

void setUp() {}
void tearDown() {}

void test_state_payload_includes_pm05_pm1_pm4_and_co_null_without_sensor() {
    SensorData data{};
    data.temp_valid = true;
    data.temperature = 22.4f;
    data.hum_valid = true;
    data.humidity = 46.2f;
    data.co2_valid = true;
    data.co2 = 812;
    data.co_sensor_present = false;
    data.co_valid = true;
    data.co_ppm = 7.3f;
    data.pm05_valid = true;
    data.pm05 = 321.4f;
    data.pm1_valid = true;
    data.pm1 = 8.7f;
    data.pm4_valid = true;
    data.pm4 = 12.3f;

    String payload = MqttPayloadBuilder::buildStatePayload(data, true, false, true);

    assert_contains(payload, "\"pm05\":321.4");
    assert_contains(payload, "\"pm1\":8.7");
    assert_contains(payload, "\"pm4\":12.3");
    assert_contains(payload, "\"co\":null");
    assert_contains(payload, "\"night_mode\":\"ON\"");
    assert_contains(payload, "\"alert_blink\":\"OFF\"");
    assert_contains(payload, "\"backlight\":\"ON\"");
}

void test_state_payload_includes_co_when_sensor_present_and_valid() {
    SensorData data{};
    data.co_sensor_present = true;
    data.co_valid = true;
    data.co_ppm = 1.5f;
    data.pm05_valid = false;
    data.pm05 = 777.0f;
    data.pm1_valid = false;
    data.pm1 = 0.0f;

    String payload = MqttPayloadBuilder::buildStatePayload(data, false, true, false);

    assert_contains(payload, "\"co\":1.5");
    assert_contains(payload, "\"pm05\":null");
    assert_contains(payload, "\"pm1\":null");
    assert_contains(payload, "\"alert_blink\":\"ON\"");
    assert_contains(payload, "\"backlight\":\"OFF\"");
}

void test_state_payload_buffer_builder_matches_string_payload() {
    SensorData data{};
    data.temp_valid = true;
    data.temperature = 21.6f;
    data.hum_valid = true;
    data.humidity = 43.5f;
    data.co2_valid = true;
    data.co2 = 745;
    data.pm25_valid = true;
    data.pm25 = 11.2f;
    data.pressure_valid = true;
    data.pressure = 1009.4f;

    char payload[512] = {};
    size_t written = MqttPayloadBuilder::buildStatePayload(payload, sizeof(payload), data, true, true, false);
    TEST_ASSERT_GREATER_THAN_UINT32(0, static_cast<uint32_t>(written));

    String string_payload = MqttPayloadBuilder::buildStatePayload(data, true, true, false);
    TEST_ASSERT_EQUAL_STRING(string_payload.c_str(), payload);
}

void test_discovery_sensor_payload_contains_pm05_template_and_topics() {
    const String device_id = "aura_test";
    const String device_name = "Aura \"Kitchen\"";
    const String base_topic = "project_aura/room1";

    String payload = MqttPayloadBuilder::buildDiscoverySensorPayload(
        device_id,
        device_name,
        base_topic,
        "pm05",
        "PM0.5",
        "#/cm\\u00b3",
        "",
        "measurement",
        "{{ value_json.pm05 }}",
        "mdi:dots-hexagon");

    assert_contains(payload, "\"name\":\"PM0.5\"");
    assert_contains(payload, "\"unique_id\":\"aura_test_pm05\"");
    assert_contains(payload, "\"state_topic\":\"project_aura/room1/state\"");
    assert_contains(payload, "\"availability_topic\":\"project_aura/room1/status\"");
    assert_contains(payload, "\"value_template\":\"{{ value_json.pm05 }}\"");
    assert_contains(payload, "\"unit_of_measurement\":\"#/cm\\u00b3\"");
    assert_contains(payload, "\"state_class\":\"measurement\"");
    assert_contains(payload, "\"icon\":\"mdi:dots-hexagon\"");
    assert_contains(payload, "\"device\":{\"identifiers\":[\"aura_test\"],\"name\":\"Aura \\\"Kitchen\\\"\"");
}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_state_payload_includes_pm05_pm1_pm4_and_co_null_without_sensor);
    RUN_TEST(test_state_payload_includes_co_when_sensor_present_and_valid);
    RUN_TEST(test_state_payload_buffer_builder_matches_string_payload);
    RUN_TEST(test_discovery_sensor_payload_contains_pm05_template_and_topics);
    return UNITY_END();
}
