// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#pragma once

#include <Arduino.h>

namespace Bmp3xxProbe {

enum class Variant : uint8_t {
    Unknown = 0,
    BMP388,
    BMP390
};

bool detect(uint8_t addr, Variant &variant);

} // namespace Bmp3xxProbe
