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
};

} // namespace power_ranks