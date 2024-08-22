#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace power_ranks {

class Utils final {
public:
    static inline std::string fixPath(std::string path) {
        std::transform(path.begin(), path.end(), path.begin(), [](unsigned char c) -> int {
            return c == '\\' ? '/' : c;
        });
        return path;
    }

    static inline std::string strToLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
        return str;
    }

    static std::string
    strReplace(std::string originalStr, const std::string& whatNeedToReplace, const std::string& whatForReplace) {
        originalStr.replace(originalStr.find(whatNeedToReplace), whatNeedToReplace.length(), whatForReplace);
        return originalStr;
    }

    static std::string strReplace(
        std::string                     originalStr,
        const std::vector<std::string>& whatNeedToReplace,
        const std::vector<std::string>& whatForReplace
    ) {
        if (whatNeedToReplace.size() != whatForReplace.size()) {
            throw std::invalid_argument("Vectors «whatNeedToReplace» and «whatForReplace» must have the same size!");
        }

        for (size_t i = 0; i < whatNeedToReplace.size(); ++i) {
            const std::string& target      = whatNeedToReplace[i];
            const std::string& replacement = whatForReplace[i];

            originalStr = strReplace(originalStr, target, replacement);
        }

        return originalStr;
    }

    template <typename T>
    static bool isValueInVector(const std::vector<T>& vector, const T& value) {
        auto iterator = std::find(vector.begin(), vector.end(), value);
        return iterator != vector.end();
    }

    static std::vector<std::string> strSplit(const std::string& str, const std::string& separator) {
        if (separator == "") {
            return {};
        }

        std::vector<std::string> strings;
        size_t                   start = 0;
        size_t                   end   = 0;

        while ((end = str.find(separator, start)) != std::string::npos) {
            strings.push_back(trim_copy(str.substr(start, end - start)));
            start = end + separator.length();
        }

        strings.push_back(trim_copy(str.substr(start)));
        return strings;
    }

private:
    // Оригинал кода, написанного ниже: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring

    static inline void ltrim(std::string& str) {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
                      return !std::isspace(ch);
                  }));
    }

    static inline void rtrim(std::string& str) {
        str.erase(
            std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
            str.end()
        );
    }

    static inline void trim(std::string& str) {
        rtrim(str);
        ltrim(str);
    }

    static inline std::string trim_copy(std::string str) {
        trim(str);
        return str;
    }
};

} // namespace power_ranks