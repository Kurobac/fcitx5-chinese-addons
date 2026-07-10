/*
 * SPDX-FileCopyrightText: 2026-2026 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _PINYIN_CHAIZIFILTER_H_
#define _PINYIN_CHAIZIFILTER_H_

#include <string>
#include <string_view>
#include <vector>

namespace libime {
class ShuangpinProfile;
}

namespace fcitx {

std::vector<std::string>
shuangpinToChaiziFilterInputs(std::string_view input,
                              const libime::ShuangpinProfile &profile);

} // namespace fcitx

#endif // _PINYIN_CHAIZIFILTER_H_
