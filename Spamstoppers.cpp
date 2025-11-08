//main lol

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "json-develop/json-develop/single_include/nlohmann/json.hpp"
#include "FindCombos.h"
#include "HashTable.h"

using json = nlohmann::json;

struct Post {
    std::string author;
    std::string title;
    std::string selftext;
};

int main(int argc, char** argv) {
    std::ifstream file1("data/AITH/AITH_submissions.jsonl");
    if (!file1.is_open()) {
        std::cerr << "Couldn't open file1." << std::endl;
        return -1;
    }

    std::vector<Post> posts;
    std::string line;

    while (std::getline(file1, line)) {
        json j = json::parse(line);
        Post p;
        p.author   = j.value("author", "");
        p.title    = j.value("title", "");
        p.selftext = j.value("selftext", "");
        posts.push_back(p);
    }

    HashTable tbl = HashTable();

    std::ifstream file2("common_spam_words.txt");
    if (!file2.is_open()) {
        std::cerr << "Couldn't open file2." << std::endl;
        return -1;
    }
    while (std::getline(file2, line)) {
        tbl.insert(line);
    }

    int spamCounter = 0;
    for (auto post: posts) {
        // Find all up to 3 word combinations in the title and body
        std::vector<std::string> phrases = findUpToNCombos(post.title, 9);
        std::vector<std::string> temp = findUpToNCombos(post.selftext, 9);
        phrases.insert(phrases.end(), temp.begin(), temp.end());

        bool alreadyCounted = false;
        for (auto phrase: phrases) {
            if (tbl.search(phrase)) { // Spam found
                if (!alreadyCounted) {
                    spamCounter++;
                    alreadyCounted = true;
                    std::cerr << "Spam detected:" << std::endl;
                    std::cerr << "Author: " << post.author <<  std::endl;
                    std::cerr << "Body: " << post.selftext <<  std::endl;
                    std::cerr << "Matching phrase: " << phrase << std::endl;
                }
            }
        }
    }

    return 0;
}