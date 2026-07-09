#include "Utils.h"

#include <algorithm>
#include <regex>
#include <stdexcept>

namespace power_ranks {

std::string Utils::clean(const std::string& input, bool removeFormat) {
    std::string string = input;

    // Remove Unicode Private Use Area characters (U+E000 to U+F8FF)
    string = removePrivateUseUnicode(string);

    if (removeFormat) {
        static const std::regex formatCodeRegex("§[0-9a-v]", std::regex::icase);
        string = std::regex_replace(string, formatCodeRegex, "");

        static const std::regex loneEscapeRegex("§");
        string = std::regex_replace(string, loneEscapeRegex, "");
    }

    // Remove ANSI escape sequences like \x1b[31m
    static const std::regex ansiRegex(R"(\x1b[\(\)\[\]0-9;]*[Bm])", std::regex::icase);
    string = std::regex_replace(string, ansiRegex, "");

    // Remove stray \x1b bytes
    string.erase(std::remove(string.begin(), string.end(), '\x1b'), string.end());
    return string;
}

std::string
Utils::strReplace(const std::string& originalStr, std::string_view whatNeedToReplace, std::string_view whatForReplace) {
    std::string result = originalStr;

    strReplaceInPlace(result, whatNeedToReplace, whatForReplace);
    return result;
}

void Utils::strReplaceInPlace(std::string& str, std::string_view whatNeedToReplace, std::string_view whatForReplace) {
    size_t pos = 0;
    while ((pos = str.find(whatNeedToReplace, pos)) != std::string::npos) {
        str.replace(pos, whatNeedToReplace.size(), whatForReplace);
        pos += whatForReplace.size();
    }
}

std::string Utils::strReplace(
    const std::string&              originalStr,
    const std::vector<std::string>& whatNeedToReplace,
    const std::vector<std::string>& whatForReplace
) {
    if (whatNeedToReplace.size() != whatForReplace.size()) {
        throw std::invalid_argument("Vectors «whatNeedToReplace» and «whatForReplace» must have the same size!");
    }

    std::string result = originalStr;
    for (size_t i = 0; i < whatNeedToReplace.size(); ++i) {
        strReplaceInPlace(result, whatNeedToReplace[i], whatForReplace[i]);
    }

    return result;
}

std::vector<std::string> Utils::strSplit(std::string_view str, std::string_view separator) {
    std::vector<std::string> strings;
    if (separator == "") {
        return strings;
    }

    size_t start = 0, end;
    while ((end = str.find(separator, start)) != std::string::npos) {
        strings.emplace_back(strTrim(str.substr(start, end - start)));
        start = end + separator.size();
    }

    strings.emplace_back(strTrim(str.substr(start)));
    return strings;
}

std::string Utils::strTrim(std::string_view str) {
    // clang-format off
        auto trimmed = str
            | std::views::drop_while([](unsigned char ch) -> int { return std::isspace(ch); })
            | std::views::reverse
            | std::views::drop_while([](unsigned char ch) -> int { return std::isspace(ch); })
            | std::views::reverse;
    // clang-format on

    return std::string(trimmed.begin(), trimmed.end());
}

std::string Utils::removePrivateUseUnicode(std::string_view input) {
    std::string output;
    output.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        unsigned char c         = input[i];
        uint32_t      codepoint = 0;
        size_t        len       = 0;

        if ((c & 0x80) == 0) {
            codepoint = c;
            len       = 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < input.size()) {
            codepoint = ((c & 0x1F) << 6) | (input[i + 1] & 0x3F);
            len       = 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < input.size()) {
            codepoint = ((c & 0x0F) << 12) | ((input[i + 1] & 0x3F) << 6) | (input[i + 2] & 0x3F);
            len       = 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < input.size()) {
            codepoint = ((c & 0x07) << 18) | ((input[i + 1] & 0x3F) << 12) | ((input[i + 2] & 0x3F) << 6)
                      | (input[i + 3] & 0x3F);
            len       = 4;
        } else {
            // Invalid UTF-8 sequence
            ++i;
            continue;
        }

        if (codepoint < 0xE000 || codepoint > 0xF8FF) {
            output.append(input, i, len);
        }

        i += len;
    }

    return output;
}

} // namespace power_ranks