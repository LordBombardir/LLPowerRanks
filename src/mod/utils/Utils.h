#include <ranges>
#include <string>
#include <vector>

namespace power_ranks {

class Utils final {
public:
    static std::string clean(const std::string& input, bool removeFormat = true);

    static std::string
    strReplace(const std::string& originalStr, std::string_view whatNeedToReplace, std::string_view whatForReplace);

    static void
    strReplaceInPlace(std::string& str, std::string_view whatNeedToReplace, std::string_view whatForReplace);

    static std::string strReplace(
        const std::string&              originalStr,
        const std::vector<std::string>& whatNeedToReplace,
        const std::vector<std::string>& whatForReplace
    );

    static std::vector<std::string> strSplit(std::string_view str, std::string_view separator);

    template <typename Container>
        requires std::ranges::range<Container> && requires(const Container& c) { c.size(); }
              && std::convertible_to<std::ranges::range_value_t<Container>, std::string_view>
    static std::string separateContainer(const Container& container, std::string_view separator) {
        if (container.empty()) {
            return "";
        }

        size_t total_size = 0;
        for (const auto& str : container) {
            total_size += std::string_view(str).size();
        }
        total_size += separator.size() * (container.size() - 1);

        std::string result;
        result.reserve(total_size);

        size_t index = 0;
        for (const auto& str : container) {
            result += str;
            if (index + 1 < container.size()) {
                result += separator;
            }
            index++;
        }

        return result;
    }

    static std::string strTrim(std::string_view str);

private:
    // Helper to remove Unicode codepoints in range U+E000 to U+F8FF
    static std::string removePrivateUseUnicode(std::string_view input);
};

} // namespace power_ranks