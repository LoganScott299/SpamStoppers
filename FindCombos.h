//
// Created by LoganScott299 on 11/3/2025.
//

#pragma once
#include <sstream>
#include <vector>
#include <string>

// Split string of words into all up to n word sequential combinations
std::vector<std::string> up_to_n_combos(const std::string& str, int n) {
    std::vector<std::string> result;
    std::vector<std::string> words;
    std::string word;

    // Push words from string onto words vector
    std::stringstream ss(str);
    while (ss >> word) {
        words.push_back(word);
    }

    // Generate combos up to n words
    for (int i = 1; i < n; i++) { // For 'i' = size 1 thru n
        for (int j = 0; j < words.size(); j++) { // For each word as a starting position
            std::string newStr = words[j];
            for (int k = 1; k < i; k++) { // Add the next 'i' words
                newStr += " ";
                newStr += words[j + k];
            }
            result.push_back(newStr);
        }
    }
    return result;
}
