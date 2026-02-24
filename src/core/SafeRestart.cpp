// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "core/SafeRestart.h"

#include <Arduino.h>
#include <esp_cpu.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "core/Logger.h"

namespace {

constexpr uint32_t kCore0RestartTaskStackBytes = 4096;
constexpr UBaseType_t kCore0RestartTaskPriority = configMAX_PRIORITIES - 1;
TaskHandle_t core0_restart_task_handle = nullptr;
portMUX_TYPE core0_restart_task_mux = portMUX_INITIALIZER_UNLOCKED;

void core0_restart_task(void *) {
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        esp_restart();
    }
}

TaskHandle_t ensure_core0_restart_task() {
    taskENTER_CRITICAL(&core0_restart_task_mux);
    TaskHandle_t existing = core0_restart_task_handle;
    taskEXIT_CRITICAL(&core0_restart_task_mux);
    if (existing != nullptr) {
        return existing;
    }

    TaskHandle_t created = nullptr;
    const BaseType_t ok = xTaskCreatePinnedToCore(core0_restart_task,
                                                   "restart_core0",
                                                   kCore0RestartTaskStackBytes,
                                                   nullptr,
                                                   kCore0RestartTaskPriority,
                                                   &created,
                                                   0);
    if (ok != pdPASS || created == nullptr) {
        LOGE("Restart", "failed to create Core0 restart task");
        return nullptr;
    }

    taskENTER_CRITICAL(&core0_restart_task_mux);
    if (core0_restart_task_handle == nullptr) {
        core0_restart_task_handle = created;
        created = nullptr;
    }
    TaskHandle_t result = core0_restart_task_handle;
    taskEXIT_CRITICAL(&core0_restart_task_mux);

    if (created != nullptr) {
        vTaskDelete(created);
    }
    return result;
}

[[noreturn]] void hard_restart_fallback() {
    esp_restart();
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

} // namespace

[[noreturn]] void safe_restart_via_core0() {
    if (esp_cpu_get_core_id() == 0) {
        hard_restart_fallback();
    }

    TaskHandle_t task = ensure_core0_restart_task();
    if (task == nullptr) {
        hard_restart_fallback();
    }

    const BaseType_t notify_ok = xTaskNotifyGive(task);
    if (notify_ok != pdPASS) {
        LOGE("Restart", "failed to notify Core0 restart task");
        hard_restart_fallback();
    }

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
