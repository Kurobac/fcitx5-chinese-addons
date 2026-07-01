/*
 * SPDX-FileCopyrightText: 2026-2026 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "chaizi.h"
#include <algorithm>
#include <fcitx-utils/macros.h>
#include <fcitx-utils/stringutils.h>
#include <fcitx-utils/utf8.h>
#include <string>

namespace fcitx {

ChaiziIndex::ChaiziIndex() = default;

std::string ChaiziIndex::normalizeCode(std::string_view code) {
    std::string result;
    result.reserve(code.size());
    for (char c : code) {
        if (c != '\'') {
            result.push_back(c);
        }
    }
    return result;
}

void ChaiziIndex::load(std::istream &in) {
    clear();

    std::string line;
    while (std::getline(in, line)) {
        if (!utf8::validate(line)) {
            continue;
        }

        auto lineView = stringutils::trimView(line);
        if (lineView.empty() || lineView[0] == '#') {
            continue;
        }

        auto hanziEnd = lineView.find_first_of(FCITX_WHITESPACE);
        if (hanziEnd == std::string_view::npos) {
            continue;
        }
        auto hanzi = lineView.substr(0, hanziEnd);
        if (utf8::length(hanzi) != 1) {
            continue;
        }

        auto codeStart = lineView.find_first_not_of(FCITX_WHITESPACE, hanziEnd);
        if (codeStart == std::string_view::npos) {
            continue;
        }
        auto codeEnd = lineView.find_first_of(FCITX_WHITESPACE, codeStart);
        auto code = lineView.substr(codeStart, codeEnd - codeStart);
        auto normalizedCode = normalizeCode(code);
        if (normalizedCode.empty()) {
            continue;
        }

        auto &codes = index_[std::string(hanzi)];
        if (std::find(codes.begin(), codes.end(), normalizedCode) ==
            codes.end()) {
            codes.push_back(std::move(normalizedCode));
        }
    }
}

void ChaiziIndex::clear() { index_.clear(); }

const std::vector<std::string> *
ChaiziIndex::lookup(std::string_view hanzi) const {
    auto iter = index_.find(std::string(hanzi));
    if (iter == index_.end()) {
        return nullptr;
    }
    return &iter->second;
}

bool ChaiziIndex::match(std::string_view hanzi, std::string_view code) const {
    auto normalizedCode = normalizeCode(code);
    if (normalizedCode.empty()) {
        return true;
    }
    auto *codes = lookup(hanzi);
    if (!codes) {
        return false;
    }
    return std::any_of(codes->begin(), codes->end(),
                       [&normalizedCode](const std::string &candidateCode) {
                           return candidateCode.starts_with(normalizedCode);
                       });
}

bool ChaiziIndex::matchAnyChar(std::string_view text,
                               std::string_view code) const {
    std::string textString(text);
    if (!utf8::validate(textString)) {
        return false;
    }

    for (auto chr : utf8::MakeUTF8CharRange(textString)) {
        auto hanzi = utf8::UCS4ToUTF8(chr);
        if (match(hanzi, code)) {
            return true;
        }
    }
    return false;
}

} // namespace fcitx
