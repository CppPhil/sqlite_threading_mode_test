#include <algorithm>
#include <string>

#include "clean_function.hpp"

namespace sqlite {
static void replaceOccurrences(
    std::string&       string,
    const std::string& toReplace,
    const std::string& replaceWith)
{
    for (std::string::iterator it = string.end();
         (it = std::search(
              string.begin(), string.end(), toReplace.begin(), toReplace.end()))
         != string.end();) {
        string.replace(
            it, it + toReplace.size(), replaceWith.begin(), replaceWith.end());
    }
}

static bool removeAllocator(std::string& string)
{
    const std::string            searchString = ",std::allocator<";
    std::string::size_type       pos          = string.find(searchString);
    const std::string::size_type begin        = pos;

    if (pos == std::string::npos) {
        return false;
    }

    pos += searchString.length();
    int angleBracketCount = 1;

    while (angleBracketCount > 0) {
        pos++;

        if (string[pos] == '<') {
            angleBracketCount++;
        }
        else if (string[pos] == '>') {
            angleBracketCount--;
        }
    }

    string.erase(begin, 1 + (pos - begin));
    return true;
}

std::string cleanFunction(const char* currentFunction)
{
    std::string result = currentFunction;

#ifdef _MSC_VER
    const std::string callingConvention = "__cdecl ";
    size_t            index             = result.find(callingConvention);

    if (index != std::string::npos) {
        result.erase(index, callingConvention.length());
    }

    const size_t      lengthOfVoid  = 4;
    const std::string redundantVoid = "(void)";
    index                           = result.find(redundantVoid);

    if (index != std::string::npos) {
        result.erase(index + 1, lengthOfVoid);
    }

    const std::string redundantStruct = "struct ";

    while ((index = result.find(redundantStruct)) != std::string::npos) {
        result.erase(index, redundantStruct.length());
    }

    const std::string redundantClass = "class ";

    while ((index = result.find(redundantClass)) != std::string::npos) {
        result.erase(index, redundantClass.length());
    }

    replaceOccurrences(
        result,
        "std::basic_string<char,std::char_traits<char>,std::allocator<char> >",
        "std::string");
    while (removeAllocator(result)) {
    }
    replaceOccurrences(result, "> >", ">>");

    return result;

#else
    return result;
#endif
}
} // namespace sqlite
