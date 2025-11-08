//
// Created by LoganScott299 on 11/3/2025.
//

#pragma once
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>

// Split string of words into all up to n word sequential combinations
std::vector<std::string> findUpToNCombos(const std::string& str, int n) {
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

// Find the top 10 string, int pairs in an unordered map
std::vector<std::pair<std::string,int>> findTop10(const std::unordered_map<std::string,int>& map) {
    std::vector<std::pair<std::string,int>> top10;
    // Make a minHeap of int, string pairs
    // int comes first so that it compares the values, not the keys
    std::priority_queue<std::pair<int, std::string>,
        std::vector<std::pair<int, std::string>>,
        std::greater<std::pair<int, std::string>>> minHeap;

    // Push values from our unordered map into the minHeap
    // and pop the smallest value when size > 10
    for (auto& pair : map) {
        minHeap.push({pair.second, pair.first});
        if (minHeap.size() > 10) minHeap.pop();
    }

    //Push the top 10 largest values into a vector
    while (!minHeap.empty()) {
        std::pair<int, std::string> top = minHeap.top();
        top10.push_back({top.second, top.first});
        minHeap.pop();
    }

    // Reverse the vector so it starts with the largest value (#1, #2, #3...)
    std::reverse(top10.begin(), top10.end());
    return top10;
}

