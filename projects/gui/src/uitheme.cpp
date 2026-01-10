#include "uitheme.h"

static void registerThemes() {
    UIThemeManager::instance().registerTheme("Default", UITheme{});
    UIThemeManager::instance().registerTheme("Green", UITheme{QColor(236, 237, 209), QColor(116, 150, 81), QColor(247, 193, 76)});
    UIThemeManager::instance().registerTheme("Blue", UITheme{QColor(194, 207, 231), QColor(94, 132, 173), QColor(247, 193, 76)});
}

static const bool registered = (registerThemes(), true);