/*
 * SPDX-FileCopyrightText: 2026-2026 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "../im/pinyin/chaizifilter.h"
#include <fcitx-utils/testing.h>
#include <libime/pinyin/shuangpinprofile.h>
#include <string>
#include <vector>

int main() {
    libime::ShuangpinProfile profile(libime::ShuangpinBuiltinProfile::Ziranma);

    const std::vector<std::pair<std::string, std::string>> cases{
        {"qy", "qing"},
        {"uv", "shui"},
        {"aqy", "aqing"},
        {"nv", "nv"},
    };
    for (const auto &[input, expected] : cases) {
        const auto result =
            fcitx::shuangpinToChaiziFilterInputs(input, profile);
        FCITX_ASSERT(result.size() == 1);
        FCITX_ASSERT(result.front() == expected);
    }

    return 0;
}
