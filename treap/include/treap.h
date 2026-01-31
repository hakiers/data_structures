#ifndef TREAP_H
#define TREAP_H
#include <memory>
#include <random>
#include <cstddef>
#include <concepts>
inline std::mt19937_64 rng{std::random_device{}()};

template <typename T>
using Uptr = std::unique_ptr<T>;

template <typename T>
concept Comparable = requires(const T& x, const T& y){
    {x < y} -> std::convertible_to<bool>;
    {x > y} -> std::convertible_to<bool>;
    {x <= y} -> std::convertible_to<bool>;
    {x >= y} -> std::convertible_to<bool>;
    {x == y} -> std::convertible_to<bool>;
};


template<Comparable T> 
class Treap{
    struct Node{
        T value;
        size_t priority;
        Uptr<Node> left;
        Uptr<Node> right;

        Node(T value = {}) 
        :
            value(value), 
            priority(rng()),
            left(nullptr),
            right(nullptr)
        {}
    };

    Uptr<Node> root;

    static Uptr<Node> join(Uptr<Node>, Uptr<Node>);

    template<typename Compare>
    static std::pair<Uptr<Node>, Uptr<Node>> split(Uptr<Node>, const T &);

    static std::pair<Uptr<Node>, Uptr<Node>> splitL(Uptr<Node> x, const T& k) { return split<std::less_equal<T>>(std::move(x), k); }
    static std::pair<Uptr<Node>, Uptr<Node>> splitU(Uptr<Node> x, const T& k) { return split<std::less<T>>(std::move(x), k); }

    public:
        Treap() = default;
        ~Treap() = default;
        void insert(const T&);
        void erase(const T&);
        bool contains(const T&) const;
        //todo
        //iterator 
        //lower/upper bound 
};

template<Comparable T>
Uptr<typename Treap<T>::Node> Treap<T>::join(Uptr<Node> x, Uptr<Node> y){
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

template<Comparable T>
template<typename Compare>
std::pair<Uptr<typename Treap<T>::Node>, Uptr<typename Treap<T>::Node>> Treap<T>::split(Uptr<Node> x, const T &key){
    if(!x) return {nullptr, nullptr};
    
    if(Compare{}(key,x->value)){
        auto [a, b] = split<Compare>(std::move(x->left), key);
        x->left = std::move(b);
        return {std::move(a), std::move(x)};
    }
    else{
        auto [a, b] = split<Compare>(std::move(x->right), key);
        x->right = std::move(a);
        return {std::move(x), std::move(b)};
    }
}

template<Comparable T>
void Treap<T>::insert(const T& value){
    if(this->contains(value)) return; //Needs a better solution 
    auto new_node = std::make_unique<Node>(value);

    auto [a, b] = splitL(std::move(root), value);
    root = join(join(std::move(a), std::move(new_node)), std::move(b));
}

template<Comparable T>
void Treap<T>::erase(const T& value){
    auto [a, bc] = splitL(std::move(root), value);
    auto [b, c] = splitU(std::move(bc), value);

    root = join(std::move(a), std::move(c));
}

template<Comparable T>
bool Treap<T>::contains(const T& value) const{
    Node *cur = root.get();
    while(cur){
        if(cur->value == value) return true;
        else if(cur->value > value) cur = cur->left.get();
        else cur = cur->right.get();
    }

    return false;
}
#endif