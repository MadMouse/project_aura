// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#pragma once

#ifndef APP_VERSION
#define APP_VERSION "dev"
#endif

#ifndef APP_BUILD_ID
#define APP_BUILD_ID "nogit"
#endif

namespace AppVersion {

inline bool isNumeric(char c) {
    return c >= '0' && c <= '9';
}

inline bool isStableRelease() {
    const char *version = APP_VERSION;
    bool saw_digit = false;
    bool saw_dot = false;

    for (; *version; ++version) {
        if (isNumeric(*version)) {
            saw_digit = true;
            continue;
        }
        if (*version == '.') {
            saw_dot = true;
            continue;
        }
        return false;
    }

    return saw_digit && saw_dot;
}

inline const char *shortVersion() {
    return APP_VERSION;
}

inline const char *buildId() {
    return APP_BUILD_ID;
}

inline const char *fullVersion() {
    if (isStableRelease()) {
        return APP_VERSION;
    }
    return APP_VERSION "-" APP_BUILD_ID;
}

} // namespace AppVersion
