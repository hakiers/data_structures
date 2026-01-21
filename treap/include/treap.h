#ifndef TREAP_H
#define TREAP_H
#include <memory>
#include <random>
#include <cstddef>

inline std::mt19937_64 rng{std::random_device{}()};

template<typename T> 
struct Node{
    T value;
    size_t priority;
    std::unique_ptr<Node<T>> left;
    std::unique_ptr<Node<T>> right;

    Node(T value = {}) 
        :
            value(value), 
            priority(rng()),
            left(nullptr),
            right(nullptr)
        {}
};

template<typename T> 
std::unique_ptr<Node<T>> join(std::unique_ptr<Node<T>>, std::unique_ptr<Node<T>>);

template<typename T>
std::pair<std::unique_ptr<Node<T>>, std::unique_ptr<Node<T>>> split(std::unique_ptr<Node<T>>, const T&);

template<typename T> 
class Treap{
    private:
        std::unique_ptr<Node<T>> root;
    public:
        Treap() : root(nullptr) {};
        void insert(const T&);
        void erase(const T&);
        bool contains(const T&);
        //todo
        //iterator 
        //lower/upper bound 
};





template<typename T>
std::unique_ptr<Node<T>> join(std::unique_ptr<Node<T>> x, std::unique_ptr<Node<T>> y){
    if(!x) return y;
    if(!y) return x;

    //assume that every key in x is less then or equal to any key in y
    if(x->priority > y->priority){
        x->right = join(std::move(x->right), std::move(y));
        return std::move(x);
    }
    else{
        y->left = join(std::move(x), std::move(y->left));
        return std::move(y);
    }
}

template<typename T>
std::pair<std::unique_ptr<Node<T>>, std::unique_ptr<Node<T>>> split(std::unique_ptr<Node<T>> x, const T &key){
    if(!x) return {nullptr, nullptr};

    if(key <= x->value){
        auto [a, b] = split(std::move(x->left), key);
        x->left = std::move(b);
        return {std::move(a), std::move(x)};
    }
    else{
        auto [a, b] = split(std::move(x->right), key);
        x->right = std::move(a);
        return {std::move(x), std::move(b)};
    }
}

template<typename T>
void Treap<T>::insert(const T& value){
    auto new_node = std::make_unique<Node<T>>(value);

    auto [a, b] = split(std::move(root), value);
    root = join(join(std::move(a), std::move(new_node)), std::move(b));
}

template<typename T>
void Treap<T>::erase(const T& value){
    auto [a, bc] = split(std::move(root), value);
    auto [b, c] = split(std::move(bc), value);

    root = join(std::move(a), std::move(c));
}

template<typename T>
bool Treap<T>::contains(const T& value){
    Node<T> *cur = root.get();
    while(cur){
        if(cur->value == value) return true;
        else if(cur->value < value) cur = cur->left.get();
        else cur = cur->right.get();
    }

    return false;
}


#endif