//
// Created by shrav on 11/3/2025.
//

#include <iostream>

//trie node class
class node{
  public:
    //indicates that spam/phrase or wrd is complete/reached its end
    bool is_blacklisted_word;
    //contains 128 nodes to represent ASCII character set
    node* children[128];

    //constructor for trie node
    node(){
        is_blacklisted_word = false;
        for(node* c : children){
            c = nullptr;
        }
    }
};

//trie class
class trie {

  trie* root;

  public:
     trie(){
       root = new node();
     }

     //insert word into trie
     void insert(std::string word){
       node* curr = root;
       for(char c : word){
         int index = static_cast<int>(c);
         if(!curr->children[index]){
           curr->children[index] = new node();
         }
         curr = curr->children[index];
       }
       curr->is_blacklisted_word = true;
     }

     //search for word in trie
     bool search(std::string word){
       node* curr = root;

       for(char c : word){
         int index = static_cast<int>(c);
         if(!curr->children[index]){
           return false;
         }
         curr = curr->children[index];
       }
       return curr->is_blacklisted_word;
     }

     //delete word from trie
     void remove(std::string word){
       node* curr = root;

       for(char c : word){
         int index = static_cast<int>(c);
         if(!curr->children[index]){
           return;
         }
         curr = curr->children[index];
       }
       if(curr->is_blacklisted_word == true){
         curr->is_blacklisted_word = false;
       }
     }

