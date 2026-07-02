/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include "../im/pinyin/chaizi.h"
#include "testdir.h"
#include <fcitx-utils/log.h>
#include <fcitx-utils/testing.h>
#include <fstream>
#include <string>
#include <vector>

namespace {

bool fileHasLine(const std::string &path, const std::string &expected) {
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        if (line == expected) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    const std::string sourceChaizi =
        std::string(TESTING_SOURCE_DIR) + "/im/pinyin/chaizi.txt";
    const std::string filteredChaizi =
        std::string(TESTING_BINARY_DIR) + "/im/pinyin/chaizi-filtered.txt";

    std::ifstream in(sourceChaizi);
    FCITX_ASSERT(in.is_open());

    fcitx::ChaiziIndex index;
    FCITX_ASSERT(index.empty());
    index.load(in);
    FCITX_ASSERT(!index.empty());

    struct TestCase {
        std::string hanzi;
        std::string rawCode;
        std::string code;
        std::string prefix;
    };

    const std::vector<TestCase> cases{
        {"清", "shui'qing", "shuiqing", "shui"},
        {"你", "ren'er", "rener", "ren"},
        {"现", "yu'jian", "yujian", "yu"},
        {"码", "shi'ma", "shima", "shi"},
        {"需", "yu'er", "yuer", "yu"},
    };

    for (const auto &testCase : cases) {
        auto *codes = index.lookup(testCase.hanzi);
        FCITX_ASSERT(codes);
        FCITX_ASSERT(index.match(testCase.hanzi, testCase.prefix));
        FCITX_ASSERT(index.match(testCase.hanzi, testCase.code));
        FCITX_ASSERT(index.match(testCase.hanzi, testCase.code.substr(0, 2)));
        FCITX_ASSERT(index.matchAnyChar(std::string("a") + testCase.hanzi,
                                        testCase.prefix));

        FCITX_ASSERT(fileHasLine(sourceChaizi, testCase.hanzi + " " +
                                                   testCase.rawCode + " -2"));
        FCITX_ASSERT(!fileHasLine(
            filteredChaizi, testCase.hanzi + " " + testCase.rawCode + " -2"));
    }

    FCITX_ASSERT(fcitx::ChaiziIndex::normalizeCode("shui'qing") == "shuiqing");
    FCITX_ASSERT(!index.match("清", "rener"));
    index.clear();
    FCITX_ASSERT(index.empty());

    return 0;
}
