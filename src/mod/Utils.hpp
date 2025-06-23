#include <algorithm>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>


namespace power_ranks {

class Utils final {
public:
    static constexpr std::string_view MC_ESCAPE_CODE = "§";

    static std::string clean(const std::string& input, bool removeFormat = true) {
        std::string string = input;

        // Remove Unicode Private Use Area characters (U+E000 to U+F8FF)
        string = removePrivateUseUnicode(string);

        if (removeFormat) {
            std::regex formatCodeRegex(std::string(MC_ESCAPE_CODE) + "[0-9a-v]", std::regex::icase);
            string = std::regex_replace(string, formatCodeRegex, "");

            std::regex loneEscapeRegex((std::string(MC_ESCAPE_CODE)));
            string = std::regex_replace(string, loneEscapeRegex, "");
        }

        // Remove ANSI escape sequences like \x1b[31m
        std::regex ansiRegex(R"(\x1b[\(\)\[\]0-9;]*[Bm])", std::regex::icase);
        string = std::regex_replace(string, ansiRegex, "");

        // Remove stray \x1b bytes
        string.erase(std::remove(string.begin(), string.end(), '\x1b'), string.end());
        return string;
    }

    static std::string
    strReplace(const std::string& originalStr, std::string_view whatNeedToReplace, std::string_view whatForReplace) {
        std::string result = originalStr;

        size_t pos = 0;
        while ((pos = result.find(whatNeedToReplace, pos)) != std::string::npos) {
            result.replace(pos, whatNeedToReplace.size(), whatForReplace);
            pos += whatForReplace.size();
        }

        return result;
    }

    static std::string strReplace(
        const std::string&              originalStr,
        const std::vector<std::string>& whatNeedToReplace,
        const std::vector<std::string>& whatForReplace
    ) {
        std::string result = originalStr;
        if (whatNeedToReplace.size() != whatForReplace.size()) {
            throw std::invalid_argument("Vectors «whatNeedToReplace» and «whatForReplace» must have the same size!");
        }

        for (size_t i = 0; i < whatNeedToReplace.size(); ++i) {
            const std::string& searchFor   = whatNeedToReplace[i];
            const std::string& replaceWith = whatForReplace[i];

            result = strReplace(result, searchFor, replaceWith);
        }

        return result;
    }

    static std::vector<std::string> strSplit(std::string_view str, std::string_view separator) {
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

    static std::string separateUnorderedSet(const std::unordered_set<std::string>& set, std::string_view separator) {
        if (set.empty()) {
            return "";
        }

        std::string result;
        for (auto [index, str] : std::views::enumerate(set)) {
            result += str;
            if (static_cast<size_t>(index) + 1 < set.size()) {
                result += separator;
            }
        }

        return result;
    }

    static inline std::string strTrim(std::string_view str) {
        // clang-format off
        auto trimmed = str
            | std::views::drop_while([](unsigned char ch) -> int { return std::isspace(ch); })
            | std::views::reverse
            | std::views::drop_while([](unsigned char ch) -> int { return std::isspace(ch); })
            | std::views::reverse;
        // clang-format on

        return std::string(trimmed.begin(), trimmed.end());
    }

private:
    // Helper to remove Unicode codepoints in range U+E000 to U+F8FF
    static std::string removePrivateUseUnicode(std::string_view input) {
        std::string output;
        size_t      i = 0;

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
                len = 4;
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
};

} // namespace power_ranks