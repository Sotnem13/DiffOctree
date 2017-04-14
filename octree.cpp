//
// Created by sotnem on 13.04.17.
//

//#include "octree.h"
//#include <memory>
#include <cstring>
template <typename S, typename T>
Octree<S, T>::Octree(Octree::P min, Octree::P max, Octree::P cell_size):min(min), max(max), cell_size(cell_size) {
//    size = (max - min)/cell_size + Octree::P{1,1,1};
//    size = size/2*2;
    root = nullptr;
}
template <typename S, typename T>
Octree<S, T>::Octree(const Octree<S, T> &o): min(o.min),max(o.max), cell_size(o.cell_size) {
    Node *n = o.root;
    root = new Node;
    copy(n,root);
}

template <typename S, typename T>
Octree<S, T>::~Octree() {
    deleteNode(root);
}

template <typename S, typename T>
 void Octree<S, T>::deleteNode(Node *n) {

    if( n == nullptr  )
        return;
    if( n->type == Branch)
        for( auto c : n->child ) {
            deleteNode(c);
        }
    delete n;
};

template <typename S, typename T>
void Octree<S, T>::copy(const Node *from, Node *to) {
    if( from == nullptr )
        return;
    to->type = from->type;
    if(from->type == Leaf){
        to->value_ = from->value_;
//        to->value_ = memset(from->value_, sizeof(from->value_),1);
    } else {
        for (int i = 0; i < from->child.size(); i++) {
            if (from->child[i] == nullptr) continue;
            if (to->child[i] == nullptr) to->child[i] = new Node;
            copy(from->child[i], to->child[i]);
        }
    }
}


template <typename S, typename T>
void Octree<S, T>::set( int x, int y, int z, const S& value ){
    set({x,y,z},value);
};
template <typename S, typename T>
void Octree<S, T>::set( P point, const S& value){

    if (!root)
        root = new Node();

    Node* parent = nullptr;
    Node* currNode = root;

    P delta = max - min;
    P currMin = min;
    P currMax = max;

    while(delta >= cell_size){
        P mid = (delta * 0.5f) + currMin;

        Point<int> p{point.x >= mid.x ? 1 : 0, point.y >= mid.y ? 1 : 0, point.z >= mid.z ? 1 : 0};
        P newMin = currMin+delta*0.5f*p;
        P newMax = currMax + delta*0.5f*(p - Point<int>{1,1,1});

        Node **n = &currNode->children[p.x][p.y][p.z];

        if( *n == nullptr ){
            *n = new Node;
            currNode->type = Branch;
        }
        parent = currNode;
        currNode = *n;
        currMin = newMin;
        currMax = newMax;
        delta = currMax - currMin;
    }
    currNode->type=Leaf;
    currNode->value_ = value;
//    parent->sparse();
};
template <typename S, typename T>
S& Octree<S, T>::get( int x, int y, int z ) {
    return get({x,y,z});
};
template <typename S, typename T>
S&  Octree<S, T>::get(P point){
    if (!root)
        root = new Node();

    Node* currNode = root;

    P delta = max - min;
    P currMin = min;
    P currMax = max;

    while(delta >= cell_size && currNode->type != Leaf){

        P mid = (delta * 0.5f) + currMin;
        Point<int> p{point.x >= mid.x ? 1 : 0, point.y >= mid.y ? 1 : 0, point.z >= mid.z ? 1 : 0};
        P newMin = currMin + delta*0.5f*p;
        P newMax = currMax + delta*0.5f*(p - Point<int>{1,1,1});

        Node **n = &currNode->children[p.x][p.y][p.z];

        if( *n == nullptr )
            *n = new Node;

        currNode = *n;
        currMin = newMin;
        currMax = newMax;
        delta = currMax - currMin;
    }

    return currNode->value_;
};

template <typename S, typename T>
S&  Octree<S, T>::operator()(int x, int y, int z){
    return (*this)({x,y,z});
};
template <typename S, typename T>
S&  Octree<S, T>::operator()(P point){
    if (!root)
        root = new Node();

    Node* parent = nullptr;
    Node* currNode = root;

    P delta = max - min;
    P currMin = min;
    P currMax = max;

    while(delta >= cell_size){
        P mid = (delta * 0.5f) + currMin;
        Point<int> p{point.x >= mid.x ? 1 : 0, point.y >= mid.y ? 1 : 0, point.z >= mid.z ? 1 : 0};
        P newMin = currMin + delta*0.5f*p;
        P newMax = currMax + delta*0.5f*(p - Point<int>{1,1,1});

        Node **n = &currNode->children[p.x][p.y][p.z];

        if( *n == nullptr ){
            *n = new Node;
            currNode->type = Branch;
        }
        parent = currNode;
        currNode = *n;
        currMin = newMin;
        currMax = newMax;
        delta = currMax - currMin;
    }
    currNode->type=Leaf;
    return currNode->value_;
};

template <typename S, typename T>
bool  Octree<S, T>::contain(int x,int y, int z){
    return contain({x,y,z});
};

template <typename S, typename T>
bool Octree<S, T>::contain(P point){
    return point >= min && point <= max;
};

template <typename S, typename T>
std::vector<S*> Octree<S, T>::neighbors(P point){

    P px = {cell_size.x,0,0};
    P py = {0,cell_size.y,0};
    P pz = {0,0,cell_size.z};

    std::vector<S*> result = {&(*this)(point+px),&(*this)(point-px),
                              &(*this)(point+py),&(*this)(point-py),
                              &(*this)(point+pz),&(*this)(point-pz)};

    return result;
};