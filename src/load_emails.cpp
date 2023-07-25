#include <fstream>
#include <stdexcept>

#include "load_emails.hpp"

namespace sqlite {
std::vector<std::string> loadEmails()
{
    const char* const fileName{"emails.txt"};
    std::ifstream     ifs{fileName};

    if (!ifs) {
        throw std::runtime_error{
            "Could not open file stream to \"" + std::string{fileName} + "\""};
    }

    std::vector<std::string> emails{};
    std::string              email{};

    while (std::getline(ifs, email)) {
        emails.push_back(email);
    }

    if (ifs.eof()) {
        return emails;
    }

    throw std::runtime_error{
        "File stream did not reach EOF for file \"" + std::string{fileName}
        + "\""};
}
} // namespace sqlite
