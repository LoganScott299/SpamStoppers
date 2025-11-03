//
// Created by LoganScott299 on 11/2/2025.
//

#include <iostream>
#include <vector>
#include <string>
#include <list>

class HashTable {
public:
    /*
     * The number of buckets should be prime to reduce collisions.
     * I'm expecting ~500 entries, so I chose 673 for a ~0.75 load factor.
     * Source: https://medium.com/swlh/why-should-the-length-of-your-hash-table-be-a-prime-number-760ec65a75d1
     *
     * If there's a collision, separate chaining will fix it by making each element a linked list.
     * Source: https://www.geeksforgeeks.org/dsa/separate-chaining-collision-handling-technique-in-hashing/
    */
    const int size = 673;
    std::vector<std::list<std::string>> data;

    HashTable() {
        data.resize(size);
    };

    /*
     * I am going to manually implement a polynomial rolling hash function.
     * The implementation closely follows the example from https://cp-algorithms.com/string/string-hashing.html
     * I chose p=257 to cover the 256 character extended ASCII table.
    */
    long long hashString(const std::string& str) {
        int p = 257;
        int m = 2147483629; // Some large prime. I chose the second largest one that can be stored as an int
        long long hashedValue = 0;
        long long pPow = 1;
        for (char c : str) {
            hashedValue = hashedValue + (c * pPow) % m;
            pPow = (pPow * p) % m;
        }
        return hashedValue % size;
    }

    // Returns true if a string is present in the hash table
    bool search(const std::string& str) {
        long long key = hashString(str);
        for (auto& entry : data[key]) {
            if (entry == str) {
                return true;
            }
        }
        return false;
    }

    // Inserts a string into the hash table if it's not already present
    void insert(const std::string& str) {
        long long key = hashString(str);
        bool exists = false;
        for (auto& entry : data[key]) {
            if (entry == str) {
                exists = true;
            }
        }
        if (!exists) {
            data[key].push_back(str);
        }
    }

    // Displays all buckets, the number of collisions, the load factor, and the greatest chain length
    void debug() {

        int greatestChain = 0;
        int collisionCounter = 0;
        int entryCounter = 0;

        for (int i = 0; i < data.size(); ++i) {
            int chainCounter = 0;
            std::cout << "Bucket: " << i << std::endl;
            for (auto& entry : data[i]) {
                std::cout << "\t" << entry << std::endl;
                chainCounter++;
                if (chainCounter > 1) {
                    collisionCounter++;
                }
                entryCounter++;
            }

            if (chainCounter > greatestChain) {
                greatestChain = chainCounter;
            }
        }

        float loadFactor = (float)entryCounter / size;

        std::cout << "\nNumber of Collisions: " << std::to_string(collisionCounter) << std::endl;
        std::cout << "Load Factor: " << loadFactor << std::endl;
        std::cout << "Greatest Chain Length: " << greatestChain << std::endl;
    }
};
