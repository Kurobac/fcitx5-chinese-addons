/*
 * SPDX-FileCopyrightText: 2026-2026 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _PINYIN_CHAIZI_H_
#define _PINYIN_CHAIZI_H_

#include <istream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fcitx {

class ChaiziIndex {
public:
    ChaiziIndex();

    void load(std::istream &in);
    void clear();
    bool empty() const { return index_.empty(); }

    const std::vector<std::string> *lookup(std::string_view hanzi) const;
    bool match(std::string_view hanzi, std::string_view code) const;
    bool matchAnyChar(std::string_view text, std::string_view code) const;

    static std::string normalizeCode(std::string_view code);

private:
    std::unordered_map<std::string, std::vector<std::string>> index_;
};

} // namespace fcitx

#endif // _PINYIN_CHAIZI_H_
