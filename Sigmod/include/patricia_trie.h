//
//  patricia_trie.h
//  Sigmod
//
//  Created by Jianwei Xu on 05/05/2013.
//  Copyright (c) 2013 Jianwei Xu. All rights reserved.
//

#ifndef __Sigmod__patricia_trie__
#define __Sigmod__patricia_trie__

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <queue>
#include <map>
#include "patricia_core.h"
using namespace std;
//use child-sibling to represent the patricia_trie

typedef struct CSNode{

    char *edgekey;//parent edge
    //ref http://bootstrapping.wordpress.com/2008/01/24/prefix-searching-with-radix-tree/
    
    struct CSNode *firstchild, *nextsibling;
}CSNode, *CSTree;

class Patricia
{
public:
    
    //constructor
    Patricia();
    
    
    //data zone:
    CSTree patricia_trie;//root
    //----------
    
    //----------------------------------
    
    bool debug;
    bool debug2;
    
    void insert(char* word, CSTree tree);
    bool search(char* word, CSTree tree);
    int share_prefix(char* word, char* edgekey);
    CSNode* create_tree_node();
    CSNode* create_empty_tree_node();
private:
    
    
};
#endif /* defined(__Sigmod__patricia_trie__) */
