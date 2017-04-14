//
// Created by sotnem on 13.04.17.
//

#ifndef OCTEE_OCTREE_H
#define OCTEE_OCTREE_H

#include <vector>
#include <stack>
#include "point.h"

template <typename S, typename T >
class Octree;

//template <typename S, typename T >
//using OctreePair = std::pair< typename Octree<S,T>::P, typename Octree<S,T>::* >;

template <typename S, typename T >
class OctreeIterator
        : public std::iterator<std::input_iterator_tag, std::pair< typename Octree<S,T>::P, S* > > {
public:
    using OctreeType = Octree<S,T>;
    using NodeType = typename OctreeType::Node;
    using P = typename OctreeType::P;
    using RetType = std::pair< typename Octree<S,T>::P, S* >;

    RetType* operator->() {
        return &it;
    }
    const RetType & operator* () const {
        return it;
    }

    OctreeIterator& operator++ () // Pre-increment
    {
        while( !stack1.empty() ) {
            auto n = stack1.top();
            stack1.pop();
            if( n.node == nullptr || n.node->type == OctreeType::Leaf ){
                currNode = n;
                it.first = (currNode.max - currNode.min)* 0.5f + currNode.min;
                it.second = &currNode.node->value_;
                break;
            } else {
                P delta = n.max - n.min;
                for(int i = 0; i < 8; i++ ){
                    auto &c = n.node->child[i];
                    if (c != nullptr) {
                        P p{i/4,(i/2)%2, i%2};
                        P newMin = n.min + delta*0.5f*p;
                        P newMax = n.max + delta*0.5f*(p - P{1,1,1});
                        stack1.push({newMin,newMax,c});
                    }
                }
            }
        }
        return *this;
    }

    bool operator==(const OctreeIterator& it) const{
        return it.currNode.node == currNode.node;
    }
    bool operator!=(const OctreeIterator& it) const {
        return it.currNode.node != currNode.node;
    }

private:

//    std::stack<NodeType*> stack1;
    struct Node {
        P min,max;
        NodeType *node;
    };

    std::stack<Node> stack1;
    P cell_size;

    RetType it;

    Node currNode;
    OctreeIterator(NodeType *node, P min, P max, P cell_size): currNode{min,max,node}, cell_size(cell_size) {

        P delta = max - min;
        P currMin = min;
        P currMax = max;
        P mid = (delta * 0.5f) + currMin;
        it.first = mid;
        it.second = &(node->value_);


        stack1.push({{0,0,0},{0,0,0},nullptr});
        if(node == nullptr) return;
        if( node->type == OctreeType::Branch) {
            for(int i = 0; i < 8; i++ ){
                auto &c = node->child[i];
                if (c != nullptr) {
                    P p{i/4,(i/2)%2, i%2};
                    P newMin = currMin+delta*0.5f*p;
                    P newMax = currMax + delta*0.5f*(p - P{1,1,1});
                    stack1.push({newMin,newMax,c});
                }
            }
        }
        while( !stack1.empty() ) {
            auto n = stack1.top();
            stack1.pop();

            delta = n.max - n.min;
            currMin = n.min;
            currMax = n.max;

            if (n.node->type == OctreeType::Leaf) {
                currNode = n;
                it.first = (currNode.max - currNode.min)* 0.5f + currNode.min;
                it.second = &currNode.node->value_;
                break;
            } else {
                for (int i = 0; i < 8; i++) {
                    auto &c = n.node->child[i];
                    if (c != nullptr) {
                        P p{i/4,(i/2)%2, i%2};
                        P newMin = currMin + delta * 0.5f * p;
                        P newMax = currMax + delta * 0.5f * (p - P{1, 1, 1});
                        stack1.push({newMin, newMax, c});
                    }
                }
            }
        }
    }

    friend class Octree<S,T>;
};


template <typename S, typename T = float >
class Octree {
public:
    enum NodeType { Branch,Leaf };
    struct Node;
    using P = Point<T>;

    using Iterator = OctreeIterator<S,T>;
    Octree(P min, P max, P cellSize = {1,1,1});
    Octree(const Octree<S,T>& o);

    ~Octree();

    S& operator()(int x,int y, int z);
    S& operator()(P point);

    const S& operator()(int x,int y, int z) const;

    void set( int x, int y, int z, const S& value );
    void  set( P point, const S& value );
    S&  get( int x, int y, int z );
    S&  get( P point);

    void erase( int x, int y, int z );

    bool contain(int x,int y, int z);
    bool contain(P point);

    Iterator begin(){ return Iterator(root,min,max,cell_size); };
    Iterator end(){ return Iterator(nullptr,{0,0,0},{0,0,0},{0,0,0}); };

    std::vector< S* > neighbors(P point);

//    Iterator begin_voxel();
//    Iterator end_voxel();
    const Node* getRoot(){
        return root;
    }
    const P& cellSize(){ return cell_size; }


//private:
    struct Node {
        NodeType type : 1;
        union {
            Node* children[2][2][2];
            std::array<Node*,8> child;
            S value_;
        };
        Node(){ for( auto& c: child ) c = nullptr; }

        bool sparse(){
            if(type == Leaf) return false;
            for( auto& c: child ) {
                if( c == nullptr || c->type != Leaf) return false;
                if(c->value_ != child[0]->value_) return false;
            }

            S temp_value = child[0]->value_;
            for( auto& c: child ) { delete c; c = nullptr; }
            type = Leaf;
            value_ = temp_value;
        }
    };

private:
    static void deleteNode(Node *n);
    static void copy(const Node *from, Node *to);

    Node* root;
    P min,max,cell_size;
};




#include "octree.cpp"

#endif //OCTEE_OCTREE_H
