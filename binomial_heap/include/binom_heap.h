#ifndef BINOM_HEAP_H
#define BINOM_HEAP_H
#include <memory>
#include <vector>
#include <iostream>

template<typename T>
using Uptr = std::unique_ptr<T>;

template<typename T>
class Binom_Heap{
    struct Node{
            T value;
            Uptr<Node> child;
            Uptr<Node> neighbor;
            size_t size; //power of two

            Node(T value) : value(value), size(0) {}
    };

    std::vector<Uptr<Node>> roots;
    size_t size_;

    void add(Uptr<Node>, size_t);
    size_t find_min_pow() const;

    public:
        Binom_Heap() : size_(0) {roots.resize(1);}
        ~Binom_Heap() = default;

        Binom_Heap(const Binom_Heap&) = delete;
        Binom_Heap& operator=(const Binom_Heap&) = delete;
        Binom_Heap(Binom_Heap&&) noexcept = default;
        Binom_Heap& operator=(Binom_Heap&&) noexcept = default;

        void insert(const T&);
        void move(Binom_Heap&);
        size_t size() const;
        T top() const;
        T pop();
};

template<typename T>
void Binom_Heap<T>::add(Uptr<Node> other_root, size_t pow){
    if(!other_root) return;
    if(roots.size() <= pow) roots.resize(pow<<1);

    if(roots[pow] == nullptr){ 
        roots[pow] = std::move(other_root);
        roots[pow]->size = pow;
        return;
    }
    
    if(other_root->value < roots[pow]->value){
        roots[pow]->neighbor = std::move(other_root->child);
        other_root->child = std::move(roots[pow]);
        add(std::move(other_root), pow+1);
    }
    else{
        other_root->neighbor = std::move(roots[pow]->child);
        roots[pow]->child = std::move(other_root);
        add(std::move(roots[pow]), pow+1);
    }
}

template<typename T>
size_t Binom_Heap<T>::find_min_pow() const{
    size_t min_pow = SIZE_T_MAX;
    for(size_t pow = 0; pow < roots.size(); pow++){
        if(!roots[pow]) continue;
        else if(roots[pow] && min_pow == SIZE_T_MAX) min_pow = pow;
        else if(roots[pow]->value < roots[min_pow]->value) min_pow = pow;
    }
    return min_pow;
}

template<typename T>
void Binom_Heap<T>::insert(const T& val){
    Uptr<Node> root_ = std::make_unique<Node>(val);
    add(std::move(root_), 0);
    size_ += 1;
}

template<typename T>
void Binom_Heap<T>::move(Binom_Heap& other){
    if(&other == this) return;
    for(size_t pow = 0; pow < other.roots.size(); pow++){
        add(std::move(other.roots[pow]), pow);
        size_ += (1<<pow);
    }

    other.roots.resize(1);
    other.size_ = 0;
}

template<typename T>
size_t Binom_Heap<T>::size() const{
    return size_;
}


template<typename T>
T Binom_Heap<T>::top() const{
    return roots[find_min_pow()]->value;
}

template<typename T>
T Binom_Heap<T>::pop(){
    Uptr<Node> root_ = std::move(roots[find_min_pow()]);
    
    Uptr<Node> cur_ = std::move(root_->child);
    while(cur_){
        Uptr<Node> next_ = std::move(cur_->neighbor);
        size_t cur_size = cur_->size;
        add(std::move(cur_), cur_size);
        cur_ = std::move(next_);
    }

    size_ -= 1;
    return root_->value;
}



#endif