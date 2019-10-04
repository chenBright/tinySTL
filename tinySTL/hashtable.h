#ifndef TINYSTL_HASHTABLE_H
#define TINYSTL_HASHTABLE_H

#include <cstddef>

#include <limits>

#include "alloc.h"
#include "vector.h"
#include "utility.h"
#include "iterator_base.h"

namespace tinySTL {
    template <class T>
    struct hashtable_node {
        T data_;
        hashtable_node* next_;
    };

    template <class Key, class Value, class Hash, class KeyOfValue, class KeyEqual,
            class Allocator = tinySTL::allocator<Value>>
    class hashtable;

    template <class Key, class Value, class Reference, class Pointer, class Hash, class KeyOfValue, class KeyEqual, class Allocator>
    struct hashtable_iterator {
        using value_type = Value;
        using reference = Reference;
        using pointer = Pointer;
        using difference_type = std::ptrdiff_t;
        using iterator_category = forward_iterator_tag;

        using iterator = hashtable_iterator<Key, Value, Value&, Value*, Hash, KeyOfValue, KeyEqual, Allocator>;
        using const_iterator = hashtable_iterator<Key, Value, const Value&, const Value*, Hash, KeyOfValue, KeyEqual, Allocator>;
        using self = hashtable_iterator<Key, Value, Reference, Pointer, Hash, KeyOfValue, KeyEqual, Allocator>;

        using hashtable_type = hashtable<Key, Value, Hash, KeyOfValue, KeyEqual, Allocator>;

        hashtable_node<value_type>* node_;
        const hashtable_type* hashtable_;

        hashtable_iterator() : node_(nullptr), hashtable_(nullptr) {}

        hashtable_iterator(hashtable_node<value_type>* node, const hashtable_type* ht) : node_(node), hashtable_(ht) {}

        explicit hashtable_iterator(const iterator& other) : node_(other.node_), hashtable_(other.hashtable_) {}

        reference operator*() const {
            return node_->data_;
        }

        pointer operator->() const {
            return &(operator*());
        }

        self& operator++() {
            auto old = node_;
            node_ = node_->next_;
            if (node_ == nullptr) { // 原 node_ 最后一个结点，需要指向下一个桶
                auto index = hashtable_->bucket(KeyOfValue()(node_->data_));
                while (node_ == nullptr && ++index < hashtable_->bucket_count()) {
                    node_ = hashtable_->buckets_;
                }
            }

            return *this;
        }

        self operator++(int) {
            auto tmp = *this;
            ++*this;

            return tmp;
        }

        bool operator==(const iterator& right) const {
            return node_ == right.node_;
        }

        bool operator==(const const_iterator& right) const {
            return node_ == right.node_;
        }

        bool operator!=(const iterator& right) const {
            return node_ != right.node_;
        }

        bool operator!=(const const_iterator& right) const {
            return node_ != right.node_;
        }
    };

    template <class Key, class Value, class HashFunction, class KeyOfValue, class KeyEqual, class Allocator>
    class hashtable {
        friend struct hashtable_iterator<Key, Value, Value&, Value*, HashFunction, KeyOfValue, KeyEqual, Allocator>;
        friend struct hashtable_iterator<Key, Value, const Value&, const Value*, HashFunction, KeyOfValue, KeyEqual, Allocator>;
    public:
        using key_type          = Key;
        using value_type        = Value;
        using allocator_type    = Allocator;
        using size_type         = std::size_t;
        using difference_type   = std::ptrdiff_t;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;

        using hasher            = HashFunction;
        using key_equal         = KeyEqual;

        using iterator          = hashtable_iterator<Key, Value, Value&, Value*, HashFunction, KeyOfValue, KeyEqual, Allocator>;
        using const_iterator    = hashtable_iterator<Key, Value, const Value&, const Value*, HashFunction, KeyOfValue, KeyEqual, Allocator>;

    private:
        using hashtable_node_allocator = typename Allocator::template rebind<hashtable_node<value_type>>::other;
        using hashtable_node_pointer_allocator = typename Allocator::template rebind<hashtable_node<value_type>*>::other;

        hasher hash_;
        key_equal equals_;

        hashtable_node_allocator nodeAllocator;

        tinySTL::vector<hashtable_node<value_type>*, hashtable_node_pointer_allocator> buckets_;
        size_type numElements_{};

    public:
        explicit hashtable(size_type n = 100, const HashFunction& hash = HashFunction(), const KeyEqual& equals = KeyEqual())
            : hash_(hash),
              equals_(equals),
              buckets_(n, nullptr),
              numElements_(0) {

        }

        hashtable(const hashtable& other) {
            copy_form(other);
        }

        hashtable(hashtable&& other) noexcept {
            move_from(std::move(other));
        }

        ~hashtable() {
            delete_hashtable();
        }

        hashtable& operator=(const hashtable& other) {
            if (this != &other) {
                copy_form(other);
            }

            return *this;
        }

        hashtable& operator=(hashtable&& other) noexcept {
            if (this != &other) {
                move_from(std::move(other));
            }

            return *this;
        }

        iterator begin() {
            for (size_type i = 0; i < buckets_.size(); ++i) {
                if (buckets_[i] != nullptr) {
                    return iterator(buckets_[i], this);
                }
            }

            return end();
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator cbegin() const {
            for (size_type i = 0; i < buckets_.size(); ++i) {
                if (buckets_[i] != nullptr) {
                    return const_iterator(buckets_[i], this);
                }
            }

            return cend();
        }

        iterator end() {
            return iterator(nullptr, this);
        }

        const_iterator end() const {
            return const_iterator(nullptr, this);
        }

        const_iterator cend() const {
            return const_iterator(nullptr, this);
        }

        size_type size() const {
            return numElements_;
        }

        bool empty() const {
            return size() == 0;
        }

        size_type max_size() const {
            return std::numeric_limits<value_type>::max();
        }

        iterator find(const key_type& key) {
            auto index = bucket_num(key);
            for (auto current = buckets_[index]; current != nullptr; current = current->next_) {
                if (equals_(KeyOfValue()(current->data_), key)) {
                    return iterator(current, this);
                }
            }

            return end();
        }

        const_iterator find(const key_type& key) const {
            auto index = bucket_num(key);
            for (auto current = buckets_[index]; current != nullptr; current = current->next_) {
                if (equals_(KeyOfValue()(current->data_), key)) {
                    return const_iterator(current, this);
                }
            }

            return cend();
        }

        tinySTL::pair<iterator, iterator> equal_range(const key_type& key) {
            auto tmp = static_cast<const hashtable&>(*this).equal_range(key); // 调用 const 版本
            return {const_cast<iterator>(tmp.first), const_cast<iterator>(tmp.second)};
        }

        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
            auto first = find(key);
            auto last = first;
            while (last != end() && equals_(KeyOfValue()(last->data_), key)) {
                ++last;
            }

            return {first, last};
        }

        size_type count(const key_type& key) const {
            auto tmp = equal_range(key);
            return distance(tmp.first, tmp.second);
        }

        iterator insert_equal(const value_type& value) {
            return insert_equal_aux(value);
        }

        iterator insert_equal(value_type&& value) {
            return insert_equal_aux(std::move(value));
        }

        template <class InputIterator>
        iterator insert_equal(InputIterator first, InputIterator last) {
            while (first != last) {
                insert_equal(*first++);
            }
        }

        void insert_equal(std::initializer_list<value_type> ilist) {
            for (auto it = ilist.begin(); it != end(); ++it)  {
                insert_equal(*it);
            }
        }

        template <class... Args>
        iterator emplace_equal(Args&&... args) {
            return insert_equal(value_type(std::forward<Args>(args)...));
        }

        tinySTL::pair<iterator, bool> insert_unique(const value_type& value) {
            return insert_unique_aux(value);
        }

        tinySTL::pair<iterator, bool> insert_unique(value_type&& value) {
            return insert_unique_aux(std::move(value));
        }

        void insert_unique(std::initializer_list<value_type> ilist) {
            for (auto it = ilist.begin(); it != end(); ++it)  {
                insert_unique(*it);
            }
        }

        template <class... Args>
        iterator emplace_unique(Args&&... args) {
            return insert_unique(value_type(std::forward<Args>(args)...));
        }

        iterator erase(const_iterator position) {
            auto resultIterator = iterator(position.node_, this);
            ++resultIterator;
            --numElements_;

            auto index = bucket_num(KeyOfValue()(*position));
            if (position.node_ == buckets_[index]) {
                buckets_[index] = position.node_->next_;
                destory_node(position.node_);

                return resultIterator;
            }

            for (auto current = buckets_[index]; current->next_ != nullptr; current = current->next_) {
                if (current->next_ == position.node_) {
                    current->next_ = position.node_->next_;
                    destory_node(position.node_);

                    return resultIterator;
                }
            }

            return resultIterator;
        }

        iterator erase(const_iterator first, const_iterator last) {
            while (first != last) {
                first = erase(*first);
            }

            return iterator(first.node_, this);
        }

        size_type erase(const key_type& key) {
            auto itPair = equal_range(key);
            size_type count = tinySTL::distance(itPair.first, itPair.second);
            erase(itPair.first, itPair.second);

            return count;
        }

        void clear() {
            erase(cbegin(), cend());
        }

        void swap(hashtable& other) {
            using std::swap;
            swap(hash_, other.hash_);
            swap(equals_, other.equals_);
            swap(buckets_, other.buckets_);
            swap(numElements_, other.numElements_);
        }

        size_type bucket_count() const {
            return buckets_.size();
        }

        size_type max_bucket_count() const {
            return buckets_.max_size();
        }

        size_type bucket_size(size_type index) const {
            size_type count = 0;
            for (auto current = buckets_[index]; current != nullptr; current = current->next_) {
                ++count;
            }

            return count;
        }

        size_type bucket(const key_type& key) const {
            return bucket_num(key);
        }

        hasher hash_finction() const {
            return hash_;
        }

        key_equal key_eq() const {
            return equals_;
        }

    private:
        hashtable_node<value_type>* allocate_node() {
            return nodeAllocator.allocate(1);
        }

        void deallocate_node(hashtable_node<value_type>* node) {
            nodeAllocator.deallocate(node, 1);
        }

        hashtable_node<value_type>* create_node(const value_type& value) {
            auto newNode = allocate_node();
            nodeAllocator.constrcut(&newNode->data_, value);

            return newNode;
        }

        hashtable_node<value_type>* create_node(value_type&& value) {
            auto newNode = allocate_node();
            nodeAllocator.constrcut(&newNode->data_, std::move(value));

            return newNode;
        }

        void destory_node(hashtable_node<value_type>* node) {
            nodeAllocator.destory(node->data_);
            deallocate_node(node);
        }

        void resize(size_type newNumElements) {
            if (newNumElements < numElements_) {
                return;
            }

            size_type newSize = buckets_.size() + tinySTL::max(newNumElements, buckets_.size());
            decltype(buckets_) tmp(newNumElements, nullptr);
            for (size_type i = 0; i < buckets_.size(); ++i) {
                auto first = buckets_[i];
                while (first != nullptr) {
                    auto newIndex = bucket_num(KeyOfValue()(first->data_), newSize);
                    buckets_[i] = first->next;
                    first->next_ = tmp[newIndex]->next_;
                    tmp[newIndex] = first;
                    first = buckets_[i];
                }
            }

            tmp.swap(buckets_);
        }

        template <class Y>
        iterator insert_equal_aux(Y&& value) {
            resize(numElements_ + 1);

            auto index = bucket_num(KeyOfValue()(value));
            auto newNode = create_node(std::forward<Y>(value));
            ++numElements_;
            for (auto current = buckets_[index]; current != nullptr; current = current->next_) {
                if (equals_(KeyOfValue()(value), KeyOfValue()(current->data_))) {
                    newNode->next_ = current->next_;
                    current->next_ = newNode;

                    return iterator(newNode, this);
                }
            }

            newNode->next_ = buckets_[index];
            buckets_[index] = newNode;

            return iterator(newNode, this);
        }

        template <class Y>
        tinySTL::pair<iterator, bool> insert_unique_aux(Y&& value) {
            resize(numElements_ + 1);

            auto index = bucket_num(KeyOfValue()(value));
            for (auto current = buckets_[index]; current != nullptr; current = current->next_) {
                if (equals_(KeyOfValue()(value), KeyOfValue()(current->data_))) {
                    return {end(), false};
                }
            }

            auto newNode = create_node(std::forward<Y>(value));
            newNode->next_ = buckets_[index];
            buckets_[index] = newNode;
            ++numElements_;

            return {iterator(newNode, this), true};
        }

        void delete_hashtable() {
            clear();
        }

        void move_from(hashtable&& other) {
            delete_hashtable();

            hash_ = other.hash_;
            equals_ = other.equals_;
            buckets_ = std::move(other.buckets_);
            numElements_ = other.numElements_;
        }

        void copy_form(const hashtable& other) {
            delete_hashtable();

            hash_ = other.hash_;
            equals_ = other.equals_;
            numElements_ = other.numElements_;
            buckets_.reserve(other.bucket_count());
            buckets_.resize(other.bucket_count(), nullptr);
            for (size_type i = 0; i < other.bucket_count(); ++i) {
                auto current = other.buckets_[i];
                if (current != nullptr) {
                    auto copyNode = create_node(current->data_);
                    buckets_[i] = copyNode;
                    for (current = current->next_; current != nullptr; current = current->next_) {
                        copyNode->next_ = create_node(current->data_);
                        copyNode = copyNode->next_;
                    }
                    copyNode->next_ = nullptr;
                }
            }
        }

        size_type bucket_num(const key_type& key, size_type n) {
            return hash_(key) % n;
        }

        size_type bucket_num(const key_type& key) {
            return bucket_num(key, bucket_count());
        }
    }; // class hashtable

} // namespace tinySTl

#endif //TINYSTL_HASHTABLE_H
