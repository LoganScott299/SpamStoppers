//
// Created by LoganScott299 on 11/2/2025.
//

#pragma once
#include <vector>
#include <string>
#include <list>

/*
 * This is a manual implementation of a hash table for strings.
 *
 * The number of buckets should be prime to reduce collisions.
 * I'm expecting ~500 entries, so I chose 673 for a 0.75 load factor.
 * Source: https://medium.com/swlh/why-should-the-length-of-your-hash-table-be-a-prime-number-760ec65a75d1
 *
 * If there's a collision, separate chaining will fix it by making each element a linked list.
 * Source: https://www.geeksforgeeks.org/dsa/separate-chaining-collision-handling-technique-in-hashing/
*/
class HashTable {
public:

    const int size = 673;
    std::vector<std::list<std::string>> data;

    HashTable();

    long long hashString(const std::string& str);

    bool search(const std::string& str);

    void insert(const std::string& str);

    bool erase(const std::string& str);

    void debug();
};
