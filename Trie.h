//
// Created by shrav on 11/3/2025.
//

#include <iostream>
#include <string>
#include <vector>

//trie Tnode class
class Tnode{
  public:
    //indicates that spam/phrase or wrd is complete/reached its end
    bool is_blacklisted_word;
    //contains 256 Tnodes to represent ASCII character set
    Tnode* children[256];

    //constructor for trie Tnode
    Tnode(){
        is_blacklisted_word = false;
        for(int i = 0; i < 256; i++){
            children[i] = nullptr;
        }
    }
};

//trie class
class trie {
  private:
    Tnode* root;

  public:
    trie(){
       root = new Tnode();
     }

     //insert word into trie
     void insert(std::string word){
       Tnode* curr = root;
       for(char c : word){
         uint8_t index = static_cast<uint8_t>(c); //because weird characters are included in jsonl
         if(!curr->children[index]){
           curr->children[index] = new Tnode();
         }
         curr = curr->children[index];
       }
       curr->is_blacklisted_word = true;
     }

     //search for word in trie
     bool search(std::string word){
       Tnode* curr = root;

       for(char c : word){
         uint8_t index = static_cast<uint8_t>(c);
         if(!curr->children[index]){
           return false;
         }
         curr = curr->children[index];
       }
       return curr->is_blacklisted_word;
     }

     //delete word from trie
     void remove(std::string word){
       Tnode* curr = root;

       for(char c : word){
         uint8_t index = static_cast<uint8_t>(c);
         if(!curr->children[index]){
           return;
         }
         curr = curr->children[index];
       }
       if(curr->is_blacklisted_word == true){
         curr->is_blacklisted_word = false;
       }
     }
};