//
// Created by shrav on 11/3/2025.
//

#include <iostream>

//trie node class
class node{
  public:
    bool wordEnd;
    node* children[26];

    //constructor for trie node
    node(){
        wordEnd = false;
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
         int index = c - 'a';
         if(!curr->children[index]){
           curr->children[index] = new node();
         }
         curr = curr->children[index];
       }
       curr->wordEnd = true;
     }

     //search for word in trie
     bool search(std::string word){
       node* curr = root;

       for(char c : word){
         int index = c - 'a';
         if(!curr->children[index]){
           return false;
         }
         curr = curr->children[index];
       }
       return curr->wordEnd;
     }

     //delete word from trie
     void remove(std::string word){
       node* curr = root;

       for(char c : word){
         int index = c - 'a';
         if(!curr->children[index]){
           return;
         }
         curr = curr->children[index];
       }
       if(curr->wordEnd == true){
         curr->wordEnd = false;
       }
     }
}

