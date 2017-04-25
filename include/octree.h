//
// Created by sotnem on 13.04.17.
//

#ifndef OCTEE_OCTREE_H
#define OCTEE_OCTREE_H

#include <memory>
#include <vector>
#include <stack>
#include <array>

#include "point.h"

#include "octree_iterator.h"

using std::unique_ptr;
using std::vector;
using std::pair;

template <typename S, typename T = float >
class Octree {
public:
    enum NodeType { Branch, Leaf };
    struct Node;
    using P = Point<T>;
    using Iterator = OctreeIterator<S, T>;

//    Ctor
    Octree(const S& emptyValue = S{});
    Octree(P min, P max, P cellSize = {1,1,1}, const S& emptyValue = S{});
    Octree(const Octree<S,T>& o);

//    Dtor
    ~Octree();


//    Element access
    S& at( const P& pos );
    S& operator[]( const P& pos );

    const S& at( const P& pos ) const;
    const S& operator[]( const P& pos ) const;

//    Capacity

    //  checks whether the container is empty
    bool empty() const;
    //  returns the number of elements
    size_t size() const;
    // returns the maximum possible number of elements
    size_t max_size() const;

//    Modifiers

    // Clears the contents
    void clear();
    // Inserts element into the container
    bool insert( const P& pos, S& value );
    // Removes the element at pos
    void erase( const P& pos );

// Lookup

//    iterator find( const P& pos );

//    Iterators

    Iterator begin(){ return Iterator(this); };
    Iterator end()  { return Iterator(nullptr); };

    Iterator begin() const { return Iterator(this); };
    Iterator end()   const { return Iterator(nullptr); };
//
//    std::vector< S* > neighbors(P point);

//    Iterator begin_voxel();
//    Iterator end_voxel();

//    S& operator()(int x,int y, int z);
//    S& operator()(P point);

//    const S& operator()(int x,int y, int z) const;

//    void set( int x, int y, int z, const S& value );
//    void  set( P point, const S& value );
//    S&  get( int x, int y, int z );
//    S&  get( P point);

    void erase( int x, int y, int z );

    bool contain(int x,int y, int z);
    bool contain(P point);

//    const Node* getRoot() {
//        return root;
//    }
    const P& cellSize(){ return cell_size; }

//private:
    struct Node {
        NodeType type;
//        std::unique_ptr< std::array<Node*, 8> > child;
        union {
            Node* children[2][2][2];
            std::array<Node*, 8> child; //  8b*8   64b
            struct {
                S value_;
                P pos;  // 12b
//                P center; // 12b
            };
        };

        Node(){ for( auto& c: child ) c = nullptr; }

//        bool sparse(){
//            if(type == Leaf) return false;
//            for( auto& c: child ) {
//                if( c == nullptr || c->type != Leaf) return false;
//                if( c->value_ != child[0]->value_  ) return false;
//            }
//
//            S temp_value = child[0]->value_;
//            for( auto& c: child ) { delete c; c = nullptr; }
//            type = Leaf;
//            value_ = temp_value;
//        }
    };

    struct NodeInfo {
        P min, max;
        int index;
    };

 private:

    static NodeInfo childrenInfo(const P& min, const P& max, const P& pos);
    Node* createNode();
    Node* createLeaf(const S& value, const P&);
    Node* find_or_create(const P&);
    static void deleteNode(Node *n);
    void copyTree(const Node *from, Node **to);

    Node* root;  // 8b
    S emptyValue_;
    P min,max,cell_size; // 36b


    using uiui = pair<unsigned int, unsigned int>;
    using NodeArray = unique_ptr<Node[]>;

    vector<pair<uiui, NodeArray >> leafs;

//    std::unique_ptr<Node*[]> leafs;

    friend class OctreeIterator<S,T>;
};




#include "../src/octree.cpp"

#endif //OCTEE_OCTREE_H
