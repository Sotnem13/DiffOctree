//
// Created by sotnem on 24.04.17.
//

#ifndef OCTEE_OCTREE_INCLUDE_H_H
#define OCTEE_OCTREE_INCLUDE_H_H

#include <iterator>
#include <stack>
#include <vector>
#include <functional>
#include <utility>

#include "octree.h"

//struct N {
//
//    int _x = 1;
//    std::function<int()> f;
//
//    N(int a, std::function<int()> f): _x(a), f(f) {};
//
//    N next() {
////        f.
//        if( _x != -1 )
//            return {
//                    -1, [=](){ cout << _x; return _x + 1; }
//            };
//        else
//            return {
//                    -1, [=](){ auto i = f(); cout << i; return i + 1; }
//            };
//
//    }
//
//    int x(){
//
//        if ( _x == -1 ) {
//            _x = f();
//        }
//        return _x;
//    }
//};

using std::pair;

template <typename S, typename T >
class Octree;

template <typename S, typename T >
class OctreeIterator :
        public std::iterator<  std::input_iterator_tag,
                               pair<const typename Octree<S, T>::P&, S& > > {
 public:
    using OctreeType = Octree<S, T>;
    using Node = typename OctreeType::Node;
    using NodeInfo = typename OctreeType::NodeInfo;
    using P = typename OctreeType::P;
    using RetType  = pair<const typename Octree<S, T>::P&, S& >;
    using StorType = pair<const typename Octree<S, T>::P, S* >;

//    RetType* operator->() {
//        return &it;
//    }
    const RetType operator* () const {
        return {node->pos, node->value_};
    }

    OctreeIterator& operator++() {   // Pre-increment
        auto &leafs = octree->leafs;

        if (index.first >= leafs.size()) {
            node = nullptr;
        } else {
            const auto &l = leafs[index.first];
            const auto &capdata = l.first;
            if (index.second < capdata.first) {
                node = &l.second.get()[index.second];
                ++index.second;
            } else {
                ++index.first;
                index.second = 0;
                ++(*this);
            }
        }
        return *this;
    }
//    OctreeIterator& operator++() {
//        node = nullptr;
//        while (node == nullptr && !stack.empty()) {
//            auto node_ = stack.top();
//            stack.pop();
//            if (node_ == nullptr || node_->type == OctreeType::Leaf) {
//                node = node_;
//            } else {
//                for (auto n : node_->child)
//                    if (n != nullptr)
//                        stack.push(n);
//            }
//
//        }
//        return *this;
//    }

    bool operator==(const OctreeIterator& it) const {
        return it.node == node;
    }
    bool operator!=(const OctreeIterator& it) const  {
        return it.node != node;
    }

private:

    std::stack<Node*> stack;
    Node *node;
    const OctreeType* octree;
    std::pair<unsigned int, unsigned int> index;

    OctreeIterator(const OctreeType* octree):
            octree(octree), node(nullptr), index{0, 0} {
        if( octree != nullptr )
            node = &octree->leafs[index.first].second.get()[index.second];
//        stack.push(node);
//        ++(*this);
    }

    friend class Octree<S,T>;
};

#endif //OCTEE_OCTREE_INCLUDE_H_H
