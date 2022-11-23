#ifndef UTILS_H
#define UTILS_H

#include <istream>
#include <vector>
#include <map>

namespace utils {

std::vector<std::string> split_by_comma(const std::string& text);

std::vector<std::string> split_by_space(const std::string& text);

std::vector<std::string> split_by_delims(const std::string& text, const std::string& delim);

std::map<std::string, std::string> attributes_for(const std::string& attributes);

template<typename Delim>
std::string get_word(std::istream& stream, Delim d) {
    std::string word;

    for(char letter; stream.get(letter); ) {    // skip delimiters
        if(!d(letter)) {
            word.push_back(letter);
            break;
        }
    }

    for(char letter; stream.get(letter); ) {    // collect word
        if(!d(letter)) {
            word.push_back(letter);
        } else {
            break;
        }
    }

    return word;
}

}

#endif