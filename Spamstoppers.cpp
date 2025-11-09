//
// created by spherohero on 11/3/2025.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "json-develop/json-develop/single_include/nlohmann/json.hpp"
#include "FindCombos.h"
#include "HashTable.h"
#include "Trie.h"

using json = nlohmann::json;
#define SUBMISSION 1
#define COMMENT 2

//define post object
struct Post {
    std::string author;
    std::string title;
    std::string selftext;
};

//convert post inputs to lowercase
std::string tolowercase(std::string strmix) {
    for (char &c : strmix) {
        c = std::tolower(c);
    }
    return strmix;
}

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <submissions_jsonl_path> <comments_jsonl_path>\n";
        return 1;
    }
    std::cerr << "Welcome to SpamStoppers!\n";
    std::cerr << "Logan Scott, Nicholas Reyes, Shravya Mandava\n";
    for (int i = 1; i < argc; i++) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
        std::ifstream file1(argv[i]);
        if (!file1.is_open()) {
            std::cerr << "Couldn't open " << argv[1] << std::endl;
            return -1;
        }

        std::vector<Post> posts;
        std::string line;

        while (std::getline(file1, line)) {
            json j = json::parse(line);
            Post p;
            if (i == SUBMISSION) {
                p.author   = j.value("author", "");
                p.title    = j.value("title", "");
                p.title = tolowercase(p.title);
                p.selftext = j.value("selftext", "");
                p.selftext = tolowercase(p.selftext);
                posts.push_back(p);
            } else if (i == COMMENT) {
                p.author   = j.value("author", "");
                p.title    = j.value("name", ""); // Comments don't have titles, so name as a unique placeholder
                p.selftext = j.value("body", "");
                p.selftext = tolowercase(p.selftext);
                posts.push_back(p);
            }
        }

        HashTable tbl = HashTable();
        trie t = trie();

        std::ifstream file2("common_spam_words.txt");
        if (!file2.is_open()) {
            std::cerr << "Couldn't open file2." << std::endl;
            return -1;
        }
        while (std::getline(file2, line)) {
            // Convert each char to lowercase
            line = tolowercase(line);
            tbl.insert(line);
            t.insert(line);
        }


        int spamCounterHT = 0;
        int spamCounterTrie = 0;
        std::unordered_map<std::string, int> spammersHT;
        std::unordered_map<std::string, int> spammersTrie;

        for (auto post: posts) {
            // Find all up to 9 word combinations in the title and body
            std::vector<std::string> phrases = findUpToNCombos(post.title, 9);
            std::vector<std::string> temp = findUpToNCombos(post.selftext, 9);
            phrases.insert(phrases.end(), temp.begin(), temp.end());

            bool alreadyCounted = false;
            for (auto phrase: phrases) {
                if (tbl.search(phrase) && t.search(phrase)) { // Spam found
                    if (!alreadyCounted) {
                        spamCounterHT++;
                        alreadyCounted = true;
                        spammersHT[post.author]++;
                    }
                }
            }
        }


        for (auto& post : posts) {
            bool foundInTrie = false;

            std::string combined = post.title + " " + post.selftext;

            for (int i = 0; i < combined.length() && !foundInTrie; i++) {
                Tnode* curr = t.root;
                for (int j = i; j < combined.length(); j++) {
                    unsigned char ch = combined[j];
                    if (curr->children[ch] == nullptr)
                        break;
                    curr = curr->children[ch];
                    if (curr->is_blacklisted_word) {
                        foundInTrie = true;
                        break;
                    }
                }
            }

            if (foundInTrie) {
                spamCounterTrie++;
                spammersTrie[post.author]++;
            }
        }



        std::cout << "\n" << spamCounterHT << " spam posts detected in " << argv[i] << " using Hash Table" << std::endl;

        // Remove the default username for deleted accounts
        spammersHT.erase("[deleted]");

        std::vector<std::pair<std::string,int>> topSpammersHT = findTop10(spammersHT);
        std::cout << "Top 10 spammers using Hash Table:" << std::endl;
        for (int i = 1; i < topSpammersHT.size(); i++) {
            std::cout << i << ". " << topSpammersHT[i].first  << " : " << topSpammersHT[i].second << " spam posts\n";
        }

        std::cout << "\n" << spamCounterTrie << " spam posts detected in " << argv[i] << " using Trie" << std::endl;

        // Remove the default username for deleted accounts
        spammersTrie.erase("[deleted]");

        std::vector<std::pair<std::string,int>> topSpammersTrie = findTop10(spammersTrie);
        std::cout << "Top 10 spammers using Trie:" << std::endl;
        for (int i = 1; i < topSpammersHT.size(); i++) {
            std::cout << i << ". " << topSpammersHT[i].first  << " : " << topSpammersHT[i].second << " spam posts\n";
        }


    }
    return 0;
}