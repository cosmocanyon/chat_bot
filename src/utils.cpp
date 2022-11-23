#include "utils.h"

#include <sstream>
#include <iostream>

namespace utils {

std::vector<std::string> split_by_comma(const std::string& text) {
    std::vector<std::string> v;
    std::stringstream sstream(text);
    std::string word;

    while(std::getline(sstream, word, ',')) {
        v.push_back(word);
    }

    return v;
}

std::vector<std::string> split_by_space(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream ss(text);

    for(std::string w; ss >> w; ) {
        words.push_back(w);
    }

    return words;
}

std::vector<std::string> split_by_delims(const std::string& text, const std::string& delim) {
    std::stringstream ss(text);

    auto del = [&](char c) {
        for(auto x : delim) {
            if(x == c)
                return true;
        }

         return false;
    };

    std::vector<std::string> words;
    for(std::string w; (w = get_word(ss, del)) != ""; ) {
        words.push_back(w);
    }

    return words;
}

std::map<std::string, std::string> attributes_for(const std::string& attributes) {
    std::map<std::string, std::string> results;

    std::vector<std::string> pair{split_by_delims(attributes, ";")};
    for(std::string word : pair)
        std::cout << word << "\n";

    results["foo"] = "bar";
    return results;
}

}
