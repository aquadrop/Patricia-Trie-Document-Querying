//
//  patricia_trie.cpp
//  Sigmod
//
//  Created by Jianwei Xu on 05/05/2013.
//  Copyright (c) 2013 Jianwei Xu. All rights reserved.
//

#include "../include/patricia_trie.h"
#include "../include/patricia_core.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>

using namespace std;



Patricia::Patricia()
{
    this->patricia_trie = this->create_empty_tree_node();
    debug = false;
    debug2 = false;
}







//situations
// 0. root, directly add, NO RECURSIVE
// 1. ALL of the edgekey has NO STARTING PART with word, create a new node, NO RECURSIVE
// 2. edgekey is prefix of word, create node of remains, RECURSIVE, until leaf
// 3. word is prefix of edgekey, create node of remains, and an empty node, NO RECURSIVE
// 4. word and edgekey which is linked to leaf share starting part, create nodes of both remains
//NO RECURSIVE

//Be adviced that the tree is in the format of firstchild-nextsibling structure
//

// insert one word
// remember edgeword is ended with 0
// the input word must ended with 0
// strcpy doesn't copy 0, and will automatically end the destination char* a NULL terminator
// this is an unordered radix tree


//caution with duplicate insert

void Patricia::insert(char* word, CSTree tree)//or CSTree root
{
    
    if (!word[0])
        return;
    
    
    //0.
    if (tree->firstchild==NULL)//directly create node
    {
        if (debug)
        {
            cout<<"Situation 0 "<<word<<endl;
        }
        CSNode *node = this->create_tree_node();
        strcpy(node->edgekey, word);
        tree->firstchild = node;
        
        //if the node is leaf not root, we need to create an empty node for it
        if (tree->edgekey)
        {
            CSNode *empty_node = this->create_empty_tree_node();
            empty_node->nextsibling = tree->firstchild;
            tree->firstchild = empty_node;
        }
        return;
    }
    
    CSNode *p = tree->firstchild;
    
    while (p) {
        
        //skip empty node
        if (!p->edgekey)
        {
            p = p->nextsibling;
            continue;
        }
        
        int share_prefix_checkpoint = share_prefix(word, p->edgekey);
        //1.
        if (!share_prefix_checkpoint)
        {
            
            
            
            if (p->nextsibling)
            {
                if (debug)
                {
                    cout<<"situation 1 skip "<<word<<endl;
                }
                p = p->nextsibling;
                continue;
            }
            else{
                if (debug)
                {
                    cout<<"situation 1 insert "<<word<<endl;
                }
                //equals to after the loop ends
                CSNode *node = this->create_tree_node();
                strcpy(node->edgekey, word);
                node->nextsibling = tree->firstchild;
                tree->firstchild = node;
                return;
            }
        }
        else
        {
            //duplicate input
            if (!strcmp(word, p->edgekey))
            {
                if (debug)
                {
                    cout<<"situation 5 "<<word<<endl;
                }
                
                //only for NON-leaf or internal node that is WITHOUT empty node
                if (p->firstchild)
                {
                    bool do_create_empty = true;
                    CSNode *q = p->firstchild;
                    while (q)
                    {
                        if (!q->edgekey)
                        {
                            do_create_empty = false;
                            break;
                        }
                        q = q->nextsibling;
                    }
                    
                    
                    
                    if (do_create_empty){
                        CSNode *empty_node = this->create_empty_tree_node();
                        empty_node->nextsibling = p->firstchild;
                        p->firstchild = empty_node;
                    }
                }
                return;
            }
            
            //3. word is prefix of edgekey
            else if (!word[share_prefix_checkpoint])
            {
                
                if (debug)
                {
                    cout<<"situation 3 "<<word<<endl;
                }
                
                //alter the edgekey
                char *edgekey = new char[MAX_WORD_LENGTH];
                strcpy(edgekey, p->edgekey);//reserve p->edgekey because it will be rewritten
                p->edgekey[share_prefix_checkpoint] = 0;
                //create a new node and make it firstchild
                CSNode *node = this->create_tree_node();
                //copy the remains
                strcpy(node->edgekey, edgekey+share_prefix_checkpoint);
                node->firstchild = p->firstchild;
                p->firstchild = node;
                
                //create an empty node only if the p node is NOT leaf
                CSNode *empty_node = this->create_empty_tree_node();
                empty_node->nextsibling = p->firstchild;
                p->firstchild = empty_node;
                return;
            }
            
            //2. edgekey is the word prefix
            else if (!p->edgekey[share_prefix_checkpoint])
            {
                
                if (debug)
                {
                    cout<<"situation 2 "<<word+share_prefix_checkpoint<<" "<<share_prefix_checkpoint<<" "<<p->edgekey<<endl;
                }
                                
                insert(word+share_prefix_checkpoint, p);
                return;
            }
            
            //4.
            else
            {
                if (debug)
                {
                    cout<<"situation 4 "<<word<<endl;
                }
                
                //rewrite the edge key of node p
                char *edgekey = new char[MAX_WORD_LENGTH];
                strcpy(edgekey, p->edgekey);//reserve p->edgekey because it will be rewritten
                p->edgekey[share_prefix_checkpoint] = 0;
                
                
                //create a new node for prefix-remain of the edgekey
                CSNode *a_node = this->create_tree_node();
                strcpy(a_node->edgekey, edgekey+share_prefix_checkpoint);
                //-->now put children of origin node to this new created node
                a_node->firstchild = p->firstchild;
                p->firstchild = a_node;
                
                //create a new node for prefix-remain of word
                CSNode *b_node = this->create_tree_node();
                strcpy(b_node->edgekey, word+share_prefix_checkpoint);
                b_node->nextsibling = p->firstchild;
                p->firstchild = b_node;
                return;
            }
            
        }
        
        p = p->nextsibling;
    }
}

//it is assumed that the word is terminated by 0
bool Patricia::search(char* word, CSTree tree)
{
    //the initial entry would always be root
    
    
    //iterate over the children and search for the edgekey that is the PREFIX of word
    CSNode *p = tree->firstchild;
    
    
    
    while(p)
    {
        //skip empty node
        if (p->edgekey==NULL)
        {
            p = p->nextsibling;
            continue;
        }
        
        //take care of the equal case
        if (!strcmp(word, p->edgekey))
        {
            //leaf case
            if (!p->firstchild)
                return true;
            else
            {
                
                
                //check if there is empty node attached
                CSNode *q = p->firstchild;
                while (q) {
                    if (!q->edgekey)
                    {
                        return true;
                    }
                    q = q->nextsibling;
                }
                return false;
            }
        }
        
        int share_prefix_checkpoint = share_prefix(word, p->edgekey);
        if (debug2)
            cout<<share_prefix_checkpoint<<" "<<word<<" "<<p->edgekey<<endl;
        if (share_prefix_checkpoint>0)
        {
            
            
            
            if (search(word+share_prefix_checkpoint, p))
                return true;
            else
                return false;
        }
        p = p->nextsibling;
    }
    
    return false;
}


//rubi ruber return 3, 0 meaning no share prefix
int Patricia::share_prefix(char *word, char *edgekey)
{
    int share_check_point = 0;
    if (!edgekey)
        return share_check_point;
    while (word[share_check_point]&&edgekey[share_check_point]) {
        if (word[share_check_point]!=edgekey[share_check_point])
            break;
        share_check_point++;
    }
    
    return share_check_point++;
}

CSNode* Patricia::create_tree_node()
{
    CSNode* node = new CSNode;
    node->edgekey = new char[MAX_WORD_LENGTH];
    node->firstchild = NULL;
    node->nextsibling = NULL;
    
    return node;
}

CSNode* Patricia::create_empty_tree_node()
{
    CSNode* node = new CSNode;
    node->edgekey = NULL;
    node->firstchild = NULL;
    node->nextsibling = NULL;
    
    return node;
}





