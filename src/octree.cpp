//
// Created by sotnem on 13.04.17.
//
#include <iostream>

// TODO(Sotnem13): do something with \|/
static constexpr std::array<Point<int>, 6> dv{
        {       {1, 0, 0}, {-1, 0, 0},
                {0, 1, 0}, {0, -1, 0},
                {0, 0, 1}, {0, 0, -1}
        }};

template <typename S, typename T>
typename Octree<S, T>::Node*
Octree<S, T>::createLeaf(const S& value, const P& p) {

    auto l = &leafs.back();

    auto capdata = &l->first;
    if (capdata->first == capdata->second) {
        auto new_capacity = capdata->first*2;
        auto ptr = unique_ptr<Node[]> (new Node[new_capacity]);
        leafs.push_back({{0, new_capacity}, std::move(ptr)});
        l = &leafs.back();
        capdata = &l->first;
    }
    auto& index = capdata->first;
    auto  mem = l->second.get();

    auto node = &mem[index++];
    node->type = Leaf;
    node->value_ = value;
    node->pos = p;
    return node;
}

template <typename S, typename T>
typename Octree<S, T>::Node* Octree<S, T>::createNode() {
    //TODO(Sotnem13): Optimize Node creating
    auto node = new Node;
    node->type = Branch;
    return node;
}

template <typename S, typename T>
void Octree<S, T>::deleteNode(Node *n) {
    if ( n == nullptr )  return;
    if (n->type == Branch) {
        for (auto c : n->child) {
            deleteNode(c);
        }
        delete n;
    }
}


template <typename S, typename T>
Octree<S, T>::Octree(const S& emptyValue):
        Octree({0, 0, 0}, {100, 100, 100}, {1, 1, 1}) {}

template <typename S, typename T>
Octree<S, T>::Octree(
        Octree::P min, Octree::P max,
        Octree::P cell_size, const S& emptyValue)
        :
        min(min), max(max), cell_size(cell_size),
        emptyValue_(emptyValue), root(nullptr) {
    auto capacity = 64;
    auto ptr = unique_ptr<Node[]>(new Node[capacity]);
    leafs.push_back({{0, capacity}, std::move(ptr)});
}

template <typename S, typename T>
Octree<S, T>::Octree(const Octree<S, T> &o):
        Octree(o.min, o.max, o.cell_size, o.emptyValue_) {

    Node *from = o.root;

    auto capacity = 0;
    for (const auto& l : o.leafs) {
        auto capdata = l.first;
        capacity += capdata.first;
    }

    auto ptr = unique_ptr<Node[]>(new Node[capacity]);
    leafs.push_back({{0, capacity}, std::move(ptr)});

    copyTree(from, &root);

}

template <typename S, typename T>
Octree<S, T>::~Octree() {
    deleteNode(root);
}

template <typename S, typename T>
void Octree<S, T>::copyTree(const Node *from, Node **to) {
    if (from == nullptr) return;

    if (from->type == Leaf) {
        if (*to == nullptr)
            *to = createLeaf(from->value_, from->pos);

    } else {
        if (*to == nullptr)
            *to = createNode();
        for (int i = 0; i < from->child.size(); i++) {
            if (from->child[i] == nullptr) continue;
            copyTree(from->child[i], &(*to)->child[i]);
        }
    }
}

template <typename S, typename T>
typename Octree<S, T>::NodeInfo
Octree<S, T>::childrenInfo(const P& min, const P& max, const P& pos) {

    auto halfSize = (max - min)*0.5;
    auto center = min + halfSize;

    Point<int> p = {pos.x >= center.x, pos.y >= center.y, pos.z >= center.z};

    NodeInfo info;
    info.index = p.z << 2 | p.y << 1 | p.x;

    auto offset = halfSize * p;

    info.min = min + offset;
    info.max = center + offset;

    return info;
}

template <typename S, typename T>
typename Octree<S, T>::Node*
Octree<S, T>::find_or_create(const P& pos){
    auto node = &root;
    NodeInfo nodeInfo{min, max};
    auto nodeSize = nodeInfo.max - nodeInfo.min;
    while (nodeSize > cell_size) {
        if (*node == nullptr)
            *node = createNode();

        nodeInfo = childrenInfo(nodeInfo.min, nodeInfo.max, pos);
        nodeSize = nodeInfo.max - nodeInfo.min;
        node = &(*node)->child[nodeInfo.index];
    }
    if (*node == nullptr)
        *node = createLeaf(emptyValue_, nodeInfo.min + nodeSize*0.5f);

    return *node;
}


template <typename S, typename T>
S& Octree<S, T>::at(const P& pos) {
    // TODO(Sotnem13): Optimize neighbors creating
    for ( auto v : dv ) {
        auto neighbor_pos = pos + v*cell_size;
        find_or_create(neighbor_pos);
    }
    auto node = find_or_create(pos);
    return node->value_;
}

template <typename S, typename T>
const S& Octree<S, T>::at(const P& pos) const {
    Node* node = root;
    NodeInfo nodeInfo = {min, max};

    auto nodeSize = nodeInfo.max - nodeInfo.min;

    while (node != nullptr && nodeSize >= cell_size && node->type != Leaf) {
        nodeInfo = childrenInfo(nodeInfo.min, nodeInfo.max, pos);
        nodeSize = nodeInfo.max - nodeInfo.min;
        node = node->child[nodeInfo.index];
    }

    if (node == nullptr)
        return emptyValue_;

    return node->value_;
}

template <typename S, typename T>
S& Octree<S, T>::operator[](const P& pos) {
    return at(pos);
}

template <typename S, typename T>
const S& Octree<S, T>::operator[](const P& pos) const {
    return at(pos);
}

template <typename S, typename T>
bool  Octree<S, T>::contain(int x, int y, int z){
    return contain({x, y, z});
};

template <typename S, typename T>
bool Octree<S, T>::contain(P point){
    return point >= min && point <= max;
};

template <typename S, typename T>
void Octree<S, T>::clear(){
    deleteNode(root);
};

template <typename S, typename T>
size_t Octree<S, T>::size() const {
    int size = 0;
    std::stack<Node*> stack;
    if (!root)
        return 0;
    stack.push(root);

    while ( !stack.empty() ) {
        auto node = stack.top();
        stack.pop();
        ++size;
        if (node->type != Leaf)
            for (auto n : node->child)
                if (n != nullptr)
                    stack.push(n);
    }

    return size;
};


