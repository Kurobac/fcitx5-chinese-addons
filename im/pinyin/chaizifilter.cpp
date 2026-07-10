/*
 * SPDX-FileCopyrightText: 2026-2026 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "chaizifilter.h"
#include <cstddef>
#include <libime/core/segmentgraph.h>
#include <libime/pinyin/pinyinencoder.h>
#include <libime/pinyin/shuangpinprofile.h>
#include <string>
#include <unordered_set>
#include <vector>

namespace fcitx {

namespace {

std::string chaiziCodeForSyllable(libime::PinyinInitial initial,
                                  libime::PinyinFinal final) {
    auto pinyin =
        libime::PinyinEncoder::initialFinalToPinyinString(initial, final);
    constexpr char umlautU[] = "\xc3\xbc";
    for (size_t pos = 0;
         (pos = pinyin.find(umlautU, pos)) != std::string::npos;) {
        pinyin.replace(pos, sizeof(umlautU) - 1, "v");
    }
    return pinyin;
}

} // namespace

std::vector<std::string>
shuangpinToChaiziFilterInputs(std::string_view input,
                              const libime::ShuangpinProfile &profile) {
    auto graph = libime::PinyinEncoder::parseUserShuangpin(
        std::string(input), profile, libime::PinyinFuzzyFlags{});
    std::vector<std::string> result;
    std::unordered_set<std::string> seen;
    graph.dfs([&result, &seen, &profile](const libime::SegmentGraphBase &graph,
                                         const std::vector<size_t> &path) {
        std::vector<std::string> pathInputs{""};
        size_t start = 0;
        for (auto end : path) {
            std::vector<std::string> syllables;
            std::unordered_set<std::string> seenSyllables;
            auto matched =
                libime::PinyinEncoder::shuangpinToSyllablesWithFuzzyFlags(
                    graph.segment(start, end), profile,
                    libime::PinyinFuzzyFlags{});
            for (const auto &[initial, finals] : matched) {
                for (const auto &finalAndFlags : finals) {
                    auto syllable =
                        chaiziCodeForSyllable(initial, finalAndFlags.first);
                    if (!syllable.empty() &&
                        seenSyllables.insert(syllable).second) {
                        syllables.push_back(std::move(syllable));
                    }
                }
            }
            if (syllables.empty()) {
                return true;
            }

            std::vector<std::string> nextInputs;
            std::unordered_set<std::string> seenInputs;
            for (const auto &prefix : pathInputs) {
                for (const auto &syllable : syllables) {
                    auto next = prefix + syllable;
                    if (seenInputs.insert(next).second) {
                        nextInputs.push_back(std::move(next));
                    }
                }
            }
            pathInputs = std::move(nextInputs);
            start = end;
        }

        for (auto &pathInput : pathInputs) {
            if (seen.insert(pathInput).second) {
                result.push_back(std::move(pathInput));
            }
        }
        return true;
    });
    return result;
}

} // namespace fcitx
