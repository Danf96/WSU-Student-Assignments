//Daniel Fuchs
//CS 450
//Written in C++
//Compiled during testing with g++
//supports avl, scapegoat, rbt
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <memory>
#include <cmath>
using namespace std;
void avl(ifstream &ban_list);
void scapegoat(ifstream &ban_list);
void rbt(ifstream &ban_list);
void btree(ifstream &ban_list, ifstream &query_file);

/*
    This container will be used in both tree types for searching for quickly finding whether or not a node exists in
    the tree as well as return the information we want from the node
*/
struct Container{
    bool exists;
    int count;
    int ban_date;
};

class AvlNode {
    public:
        string username;
        int ban_date, balance, count;
        AvlNode(string username, int server_id, int ban_date): username(username), ban_date(ban_date), count(1), left(nullptr), right(nullptr){}
        AvlNode *left, *right;
        void contains(string const& username, Container& container);
        AvlNode* insert(string const& username, int server_id, int ban_date);
        void print_preorder() const;
        AvlNode* rotate_left(bool adjust_balance);
        AvlNode* rotate_right(bool adjust_balance);
        AvlNode* rotate_left_right();
        AvlNode* rotate_right_left();
};

/*
    Recursively goes through tree and updated contained if found, otherwise returns with container boolean still set to false
*/
void AvlNode::contains(string const& username, Container& container){
    int comparison = username.compare(this->username);
    if(comparison == 0){ //if username == this->username
        container.exists = true;
        container.ban_date = this->ban_date;
        container.count = this->count;
        return;
    }
    if(this->left && comparison < 0){ this->left->contains(username, container);} //if username < this->username
    if(this->right && comparison > 0){ this->right->contains(username, container);}
    return;
}

/*
    Inserts into tree based on code in slides
*/
AvlNode* AvlNode::insert(string const& username, int server_id, int ban_date){
    int comparison = username.compare(this->username);
    if(comparison == 0){ //if username == this->username
        this->count++;
        if(this->ban_date < ban_date){
            this->ban_date = ban_date;
        }
        return this;
    }
    else if(comparison < 0){ //if(username < this->username)
        if(this->left){
            int before_balance = this->left->balance;
            this->left = this->left->insert(username, server_id, ban_date);
            int after_balance = this->left->balance;
            //left sub tree got bigger or smaller
            if(before_balance == 0 && after_balance != 0){balance--;}
        }
        else{
            this->left = new AvlNode(username, server_id, ban_date);
            balance--;
        }
        if(balance < -1 && left->balance <= -1){return rotate_right(true);}
        if(balance < -1 && left->balance >= 1){return rotate_left_right();}
    }
    //else, go right
    else{
        if(this->right){
            int before_balance = this->right->balance;
            this->right = this->right->insert(username, server_id, ban_date);
            int after_balance = this->right->balance;
            //right sub tree got bigger or smaller
            if(before_balance == 0 && after_balance != 0){balance++;}
        }
        else{
            this->right = new AvlNode(username, server_id, ban_date);
            balance++;
        }
        if(balance > 1 && right->balance >= 1){return rotate_left(true);}
        if(balance > 1 && right->balance <= -1){return rotate_right_left();}
    }
    return this;
}
/*
    Left rotation based on slides, boolean is for when this function is used in right-left/left-right rotations
*/
AvlNode* AvlNode::rotate_left(bool adjust_balance){
    if (!right){return this;}

    auto new_root = right;

    right = right->left;
    new_root->left = this;

    if(adjust_balance){
        new_root->balance = 0;
        balance = 0;
    }
    return new_root;
}
/*
    Right rotation based on slides, boolean is for when this function is used in right-left/left-right rotations
*/
AvlNode* AvlNode::rotate_right(bool adjust_balance){
    if (!left){return this;}
    auto new_root = left;

    left = left->right;
    new_root->right = this;

    if(adjust_balance){
        new_root->balance = 0;
        balance = 0;
    }
    return new_root;
}

/*
    Rotates left-right then adjusts balances
*/
AvlNode* AvlNode::rotate_left_right(){
    left = left->rotate_left(false);
    auto y = rotate_right(false);
    auto x = y->left;
    auto z = y->right;

    if(y->balance == 0){
        x->balance = 0;
        z->balance = 0;
    }
    else{//same as right_left()
        if(y->balance > 0){
            x->balance = -1;
            z->balance = 0;
        }
        else{
            x->balance = 0;
            z->balance = 1;
        }
        y->balance = 0;
    }
    return y;
}

/*
    Rotates right-left then adjusts balances
*/
AvlNode* AvlNode::rotate_right_left(){
    right = right->rotate_right(false);
    auto y = rotate_left(false);
    auto x = y->left;
    auto z = y->right;

    if(y->balance == 0){
        x->balance = 0;
        z->balance = 0;
    }
    else{
        if(y->balance > 0){
            x->balance = -1;
            z->balance = 0;
        }
        else{
            x->balance = 0;
            z->balance = 1;
        }
        y->balance = 0;
    }
    return y;
}

/*
    Preorder print used for testing
*/
void AvlNode::print_preorder() const{
    cout << " " << username << " ";
    if (!this->left && !this->right){return;}
    cout << "(";
    if(this->left){this->left->print_preorder();} else{cout << "_";}
    if(this->right){this->right->print_preorder();}else{cout << "_";}
    cout << ")";
}

/*
    Tree that handles root case and calls node methods.
*/
class AvlTree{
    public:
        AvlNode* root;
        AvlTree(): root(nullptr) {}
        ~AvlTree(){delete root; root = nullptr;};
        void contains(string const& username, Container& container);
        void insert(string const& username, int server_id, int ban_date);
};
void AvlTree::contains(string const& username, Container& container){
    if(!root){return;}
    root->contains(username, container);
}
void AvlTree::insert(string const& username, int server_id, int ban_date){
    if (this->root){
        this->root = this->root->insert(username, server_id, ban_date);
    }
    else{
        this->root = new AvlNode(username, server_id, ban_date);
    }
}



int main(int argc, char* argv[]){ //argv[1] = tree type, argv[2] = banlist file, cin = query file
clock_t start = clock();
ifstream ban_list(argv[2]);
string tree_type = string(argv[1]);
if(tree_type == "scapegoat"){
    scapegoat(ban_list);
}else if(tree_type == "btree"){
    //avl(ban_list);
}else if (tree_type == "rbt"){
    rbt(ban_list);
}else{
    avl(ban_list);
}
double time_taken_in_seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
double time_taken_in_microseconds = time_taken_in_seconds * 1000000.0;
cout << "total time in microseconds: " << time_taken_in_microseconds << endl;
}

void avl(ifstream &ban_list){
    string line, username;
    Container container;
    int server_id, ban_date;
    //create tree then insert at root node
    auto avl_tree = new AvlTree();
    //int i = 0;
    while (ban_list >> username){
        ban_list >> server_id;
        ban_list >> ban_date;
        avl_tree->insert(username, server_id, ban_date);
    }
    while(cin >> username){
        container.exists = false;
        avl_tree->contains(username, container);
        if(container.exists){
            cout << username << " was banned from " << container.count << " servers. most recently on: " << container.ban_date << "\n";
        }
        else{
            cout << username << " is not currently banned from any servers.\n";
        }
    }
}

//all scapegoat classes and methods below
struct ScapeNode{
    string username;
    int ban_date;
    unsigned short int count;
    int children[2]; //doing index into array method
    int size;
};
class ScapeTree{
    public:
        vector<ScapeNode> node_vec;
        int root;
        double alpha;
        ScapeTree(double alpha){
            this->alpha = alpha;
            this->root = -1;
            node_vec.reserve(100000);
        }
        void insert(string const& username, int ban_date);
        void searchTree(int root, string const& username, Container &contaner);
        int findScapegoat(int node, vector<int> &path);
        int getSibling(int node,vector<int> &path);
        int insertKey(int index, int depth, int side, vector<int> &path, string const& username, int ban_date);
        int getAlphaHeight(int depth);
        void rebuildTree(vector<int> &path, int scape_node);
        void inOrderWalk(vector<int> &list, int index);
        int buildRecursion(vector<int>& list, int start_point, int end_point, unsigned short size);

};
int ScapeTree::getAlphaHeight(int depth){
    double size = this->node_vec.size();    //nodeSize(this->root);
    return (int)floor(log2(size)/log2(1/this->alpha));
}
void ScapeTree::insert(string const& username, int ban_date){
    if(this->root == -1){//inserting into empty tree
        this->node_vec.push_back({username, ban_date, 1, {-1,-1}, (unsigned short int)1});
        this->root = 0;
    }
    else{
        vector<int> path;
        int height = insertKey(this->root, 0, 2, path, username, ban_date); //side needs to be fixed
        if (height == 0){
            return; //node count was incremented, no need to rebalance
        }else if(height > getAlphaHeight(height)){
            int scape_index = findScapegoat(path.back(), path);
            rebuildTree(path, scape_index);
        }
    }
}
/*
    Recursive insertion function, returns -depth if node exists so that the ScapeTree insert method is able to see that alpha height does not need to be checked,
    returns 0 then recurses up with function call + 1 to get correct height on return
*/
int ScapeTree::insertKey(int index, int depth, int side, vector<int> &path, string const& username, int ban_date){
    int new_index;
    if(index == -1){//at end of tree, time to add
        new_index = this->node_vec.size();
        this->node_vec[path.back()].children[side] = new_index;
        ScapeNode new_node = {username, ban_date, 1,{-1,-1}, 1};
        this->node_vec.push_back(new_node);
        path.push_back(new_index);
        return 0;
    }
    int comparison = username.compare(this->node_vec[index].username);
    if (comparison == 0){//node already exists
        this->node_vec[index].count++;
        if(this->node_vec[index].ban_date < ban_date){
            this->node_vec[index].ban_date = ban_date;
        }
        return -depth;
    }
    else if (comparison < 0){ //go left
        new_index = this->node_vec[index].children[0];
        path.push_back(index);
        depth++;
        this->node_vec[index].size++;
        auto height = insertKey(new_index, depth, 0, path, username, ban_date) + 1;
        if (height < 1){
            this->node_vec[index].size--;
        }
        return height;
    }
    else{
        new_index = this->node_vec[index].children[1];
        path.push_back(index);
        depth++;
        this->node_vec[index].size++;
        auto height = insertKey(new_index, depth, 1, path, username, ban_date) + 1;
        if(height < 1){
            this->node_vec[index].size--;
        }
        return height;
    }
}
/*
    Recursive search similar to AVL search, just using vector of nodes
*/
void ScapeTree::searchTree(int root, string const& username, Container& container){
    if (root == -1){
        return;
    }
    int comparison = username.compare(this->node_vec[root].username);
    if(comparison == 0){ //if username == this->username
        container.exists = true;
        container.ban_date = this->node_vec[root].ban_date;
        container.count = this->node_vec[root].count;
        return;
    }
    if(comparison < 0){ searchTree(this->node_vec[root].children[0], username, container);} //if username < this->username
    else{searchTree(this->node_vec[root].children[1], username, container);}
    return;
}
/*
    Works through path vector from the end to the start, looking for nodes that violate our height check, then tries to find if any next nodes also violate to rebuild from
*/
int ScapeTree::findScapegoat(int node, vector<int> &path){
    int size = 1;
    int height = 0;
    int totalSize = 0;
    bool scape_found = false;
    while (path.size() > 1){
        height++;
        auto sibling = getSibling(node, path);
        if (sibling == -1){sibling = 0;}
        else{sibling = this->node_vec[sibling].size;}
        totalSize = 1 + this->node_vec[node].size + sibling;
        if (height > log2(totalSize)/log2(1/this->alpha)){ //will continue to check until fails and scapegoat is found
            path.pop_back();
            node = path.back();
            scape_found = true;
        }
        else if (scape_found == true){
            return node;
        }
        else{
            path.pop_back();
            node = path.back();
        }
    }
    return this->root; //only root is left, has to be root
}
int ScapeTree::getSibling(int node, vector<int> &path){//depends on whether path gets last node appended before or after calling
    int parent = path.end()[-2]; //gets last element of vector
    if(this->node_vec[parent].children[0] == node){
        return this->node_vec[parent].children[1];
    }
    else {return this->node_vec[parent].children[0];}
}

/*
    Calls method that will build the ordered list and then assigns recursive result to its correct position in the tree
*/
void ScapeTree::rebuildTree(vector<int> &path, int scape_index){
    vector<int> ordered_list;
    inOrderWalk(ordered_list, scape_index);
    if(scape_index == this->root){
        this->root = buildRecursion(ordered_list, 0, ordered_list.size()-1, this->node_vec[this->root].size);
    }
    else{
        path.pop_back(); //we want the parent of the scapegoat
        auto scape_parent = path.back();
        if(this->node_vec[scape_parent].children[0] == scape_index){
            this->node_vec[scape_parent].children[0] = buildRecursion(ordered_list, 0, ordered_list.size()-1, this->node_vec[scape_index].size);
        }
        else{
            this->node_vec[scape_parent].children[1] = buildRecursion(ordered_list, 0, ordered_list.size()-1, this->node_vec[scape_index].size);
        }
    }
}
/*
    Builds vector of indices in order to rebuild the tree with
*/
void ScapeTree::inOrderWalk(vector<int> &list, int index){
    if(index == -1){
        return;
    }
    inOrderWalk(list, this->node_vec[index].children[0]);
    list.push_back(index);
    inOrderWalk(list, this->node_vec[index].children[1]);
}
/*
    Recursively builds the tree with the midpoint of the list being the root, and the midpoints of the left and right children being its partition
*/
int ScapeTree::buildRecursion(vector<int>& list, int start_point, int end_point, unsigned short size){
    if(start_point == end_point){
        //nullify children then return index
        auto node_index = list[start_point];
        ScapeNode * node = &this->node_vec[node_index];
        node->children[0] = -1;
        node->children[1] = -1;
        node->size = 1;
        return node_index;
    }
    else{
        auto mid_point = (start_point + end_point)/2;
        auto node_index = list[mid_point];
        ScapeNode * node = &this->node_vec[node_index]; //need cases where if midpoint adjustment breaks bounds
        node->size = size;
        if (start_point <= mid_point-1){
        node->children[0] = buildRecursion(list, start_point, mid_point-1, size - 1);
        }
        else{node->children[0] = -1;}
        node->children[1] = buildRecursion(list, mid_point+1, end_point, size - 1);
        return node_index;
    }
}
/*
    Main scapegoat function where the list and standard input are parsed
*/
void scapegoat(ifstream &ban_list){
    ScapeTree scape_tree(0.70);
    string username;
    int ban_date, server_id;
    Container container;

    while (ban_list >> username){
        ban_list >> server_id;
        ban_list >> ban_date;
        scape_tree.insert(username, ban_date);
    }

    while(cin >> username){
        container.exists = false;
        scape_tree.searchTree(scape_tree.root, username, container);
        if(container.exists){
            cout << username << " was banned from " << container.count << " servers. most recently on: " << container.ban_date << "\n";
        }
        else{
            cout << username << " is not currently banned from any servers.\n";
        }
    }
}

//classes and functions below are for Red-Black
enum Dir{
    LEFT = 0,
    RIGHT = 1
};
struct RbNode{
    RbNode* parent;
    RbNode* child[2];
    string username;
    int ban_date;
    unsigned short int count;
    bool is_red;
    RbNode(string username, int ban_date, RbNode* parent){
            this->parent = parent;
            this->username = username; 
            this->ban_date = ban_date;
            this->count = 1;
            is_red = true;
            this->child[LEFT] = nullptr;
            this->child[RIGHT] = nullptr;
    }
    RbNode* getSibling(RbNode* node);
    RbNode* getUncle(RbNode* node);
    void insert(string const& username, int ban_date, RbNode *& root);
    void insertFixup(RbNode * node, RbNode *& root);
    void swapColors(RbNode * node, RbNode *& root);
    RbNode* caseCheck(RbNode *& root);
    RbNode* rotate(Dir dir);
    void contains(string const& username, Container& container);
};
class RbTree{
    public:
        RbNode* root;
        RbTree(): root(nullptr){}
        ~RbTree(){delete root; root = nullptr;};
        void insert(string const& username, int ban_date);
        void contains(string const& username, Container& container);
};
void RbTree::insert(string const& username, int ban_date){
    if (this->root){
        this->root->insert(username, ban_date, root);
        if(this->root->is_red){this->root->is_red = false;}
    }
    else{
        this->root = new RbNode(username, ban_date, nullptr);
        this->root->is_red = false;
    }
}
void RbTree::contains(string const& username, Container& container){
    if(!root){return;}
    root->contains(username, container);
}
RbNode* RbNode::getSibling(RbNode* node){
    if(node->parent){
        if(node == node->parent->child[LEFT]){
            return node->parent->child[RIGHT];
        }
        else{
            return node->parent->child[LEFT];
        }
    }
    else{return nullptr;}
}
    
RbNode* RbNode::getUncle(RbNode* node){
    if(node->parent){
        return getSibling(node->parent);
    }
    else{return nullptr;}
    
}
void RbNode::insert(string const& username, int ban_date, RbNode *& root){
    int comparison = username.compare(this->username);
    if(comparison == 0){ //if username == this->username
        this->count++;
        if(this->ban_date < ban_date){
            this->ban_date = ban_date;
        }
        return;
    }
    else if(comparison < 0){ //if(username < this->username)
        if(this->child[LEFT]){
            this->child[LEFT]->insert(username, ban_date, root);
        }
        else{
            this->child[LEFT] = new RbNode(username, ban_date, this);
            this->child[LEFT]->caseCheck(root);//correct?
            return;
        }
    }
    //else, go right
    else{
        if(this->child[RIGHT]){
            this->child[RIGHT]->insert(username, ban_date, root);
        }
        else{
            this->child[RIGHT] = new RbNode(username, ban_date, this);
            this->child[RIGHT]->caseCheck(root);
            return;
        }

    }
    return;
}
void RbNode::insertFixup(RbNode * node, RbNode *& root){
    if (!node){return;}
    if(!node->is_red){insertFixup(node->parent, root); return;}
    swapColors(node, root);
}
void RbNode::swapColors(RbNode * node, RbNode *& root){
    if(node == root){
        node->is_red = false;
        node->child[LEFT]->is_red = true;
        node->child[RIGHT]->is_red = true;
        return;
    }
    if(node->parent->parent){//if grandparent exists
        node->parent->is_red = false;
        RbNode * u = getUncle(node);
        if (u){
            u->is_red = false;
        }
        node->parent->parent->is_red = true;
    }
}
RbNode* RbNode::caseCheck(RbNode *& root){
    RbNode * p = this->parent;
    RbNode * u = getUncle(this);
    RbNode * g = p->parent;
    if(!p->is_red){
        return this;
    }
    if(u && u->is_red){
        swapColors(this, root);
        insertFixup(this->parent->parent, root);
        return this;
    }
    if(!u || !u->is_red){
        RbNode* new_root;
        if(g){
            if(this == p->child[LEFT] && p == g->child[LEFT]){
                new_root = g->rotate(RIGHT);
                if(!new_root->parent){
                    root = new_root;
                }
                new_root->child[LEFT]->is_red = true;
                new_root->child[RIGHT]->is_red = true;
                new_root->is_red = false;
                insertFixup(this, root);
                return new_root;
            }
            else if(this == p->child[RIGHT] && p == g->child[LEFT]){
                p = p->rotate(LEFT);
                new_root = g->rotate(RIGHT);
                if(!new_root->parent){
                    root = new_root;
                }
                new_root->child[LEFT]->is_red = true;
                new_root->child[RIGHT]->is_red = true;
                new_root->is_red = false;
                insertFixup(this, root);
                return new_root;
            }
            else if(this == p->child[LEFT] && p == g->child[RIGHT]){
                p = p->rotate(RIGHT);
                new_root = g->rotate(LEFT); //parent should be updated
                if(!new_root->parent){
                    root = new_root;
                }
                new_root->child[LEFT]->is_red = true;
                new_root->child[RIGHT]->is_red = true;
                new_root->is_red = false;
                insertFixup(this, root);
                return  new_root;
            }
            else if(this == p->child[RIGHT] && p == g->child[RIGHT]){
                new_root = g->rotate(LEFT);
                if(!new_root->parent){
                    root = new_root;
                }
                new_root->child[LEFT]->is_red = true;
                new_root->child[RIGHT]->is_red = true;
                new_root->is_red = false;
                insertFixup(this, root);

                return new_root;
            }
        }
    }
    return this;
}
RbNode* RbNode::rotate(Dir dir){//had to check wikipedia rotations https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
    if (!child[1-dir]){
        return this;
    }
    auto g = this->parent;
    auto new_root = child[1-dir];//right child, S in wikipedia code
    auto c = new_root->child[dir];
    this->child[1-dir] = c;
    if (c){c->parent = this;}
    new_root->child[dir] = this;
    this->parent = new_root;
    new_root->parent = g;
    if(g){
        if(this == g->child[RIGHT]){
            g->child[RIGHT] = new_root;
        }
        else{g->child[LEFT] = new_root;}
    }
    return new_root;
}
void RbNode::contains(string const& username, Container& container){
    int comparison = username.compare(this->username);
    if(comparison == 0){ //if username == this->username
        container.exists = true;
        container.ban_date = this->ban_date;
        container.count = this->count;
        return;
    }
    if(this->child[LEFT] && comparison < 0){ this->child[LEFT]->contains(username, container);} //if username < this->username
    if(this->child[RIGHT] && comparison > 0){ this->child[RIGHT]->contains(username, container);}
    return;
}
void rbt(ifstream &ban_list){
    RbTree rb_tree = RbTree();
    string username;
    int ban_date, server_id;
    Container container;

    while (ban_list >> username){
        ban_list >> server_id;
        ban_list >> ban_date;
        rb_tree.insert(username, ban_date);
    }
    while(cin >> username){
        container.exists = false;
        rb_tree.contains(username, container);
        if(container.exists){
            cout << username << " was banned from " << container.count << " servers. most recently on: " << container.ban_date << "\n";
        }
        else{
            cout << username << " is not currently banned from any servers.\n";
        }
    }
}

struct Key{
    string username;
    int ban_date;
    unsigned short int count;
};
struct BNode{
    BNode* parent;
    vector<Key> keys;
    vector<BNode> children;
    bool isLeaf;
    BNode(BNode* parent): parent(parent), isLeaf(true){}
    BNode* insert(string &username, int ban_date);
    int findInsertPoint(string &username);
    bool mustSplit();
    void split();
};
bool BNode::mustSplit(){
    if (keys.size() >= 3){
        return true;
    }
    else return false;
}
void BNode::split(){
    int median = 3/2;
    auto left = new BNode(parent);
    //
}
BNode* BNode::insert(string &username, int ban_date){
    auto i = findInsertPoint(username);
    if (isLeaf){
        keys[i] = {username, ban_date, 1};
    }
    else{
        children[i].insert(username, ban_date);
    }
    if(mustSplit()){
        return split();
    }
    else{
        return this;
    }
}
int BNode::findInsertPoint(string &username){
    int i = 0;
    while (i < this->keys.size() && this->keys[i].username <= username){
        i++;
    }
    return i;
}
class BTree{
    public:
        BNode* root;
        BTree(int order): root(nullptr), order(order){}
        int order;
        void insert(string &username, int ban_date);
};
void BTree::insert(string &username, int ban_date){
    if(this->root){
        this->root->insert(username, ban_date);
    }
    else{
        this->root = new BNode(nullptr);
    }
}
void btree(ifstream &ban_list, ifstream &query_file){
    //hardcoding order to 3 for now
    BTree b_tree();
    string username;
    int ban_date, server_id;
    Container container;

    while (ban_list >> username){
        ban_list >> server_id;
        ban_list >> ban_date;
        b_tree.insert(username, ban_date);
    }
}