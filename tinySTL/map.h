#ifndef TINYSTL_MAP_H
#define TINYSTL_MAP_H

#include <initializer_list>
#include <functional>
#include <algorithm>

#include "alloc.h"
#include "utility"
#include "functional.h"
#include "rb_tree.h"

namespace tinySTL {
    // 有序键值对容器，它的元素的键是唯一的。
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/map
    template <class Key, class T, class Compare = tinySTL::less<Key>, class Allocator = tinySTL::allocator<tinySTL::pair<Key,T>>>
    class map {
    public:
        using key_type          = Key;
        using mapped_type       = T;
        using value_type        = tinySTL::pair<const Key, T>;
        using key_compare       = Compare;
        using allocator_type    = Allocator;

        class value_compare : public tinySTL::binary_finction<value_type, value_type, bool> {
            friend class map<Key, T, Compare, Allocator>;
        private:
            Compare comp_;
            explicit value_compare(Compare comp) : comp_(comp) {}

        public:
            bool operator()(const value_type& lhs, const value_type& rhs) const {
                return comp_(lhs, rhs);
            }
        };

    private:
        using rb_tree_type = rb_tree<key_type, value_type, tinySTL::select1st<value_type>, key_compare, Allocator>;
        rb_tree_type tree_;

    public:
        using size_type                 = typename rb_tree_type::size_type;
        using difference_type           = typename rb_tree_type::difference_type;
        using reference                 = typename rb_tree_type::reference;
        using const_reference           = typename rb_tree_type::const_reference;
        using pointer                   = typename rb_tree_type::pointer;
        using const_pointer             = typename rb_tree_type::const_pointer;
        using iterator                  = typename rb_tree_type::iterator;
        using const_iterator            = typename rb_tree_type::const_iterator;
        using reverse_iterator          = typename rb_tree_type::reverse_iterator;
        using const_reverse_iterator    = typename rb_tree_type::const_reverse_iterator;

    public:
        map() = default;

        explicit map(const Compare& comp) : tree_(comp) {}

        template <class InputIterator>
        map(InputIterator first, InputIterator last, const Compare& comp = Compare()) : tree_(comp) {
            tree_.insert_unique(first, last);
        }

        map(const map& other) : tree_(other.tree_) {}

        map(map&& other) noexcept : tree_(std::move(other.tree_)) {}

        map(std::initializer_list<value_type> ilist, const Compare& comp = Compare()) : tree_(comp) {
            tree_.insert_unique(ilist.begin(), ilist.end());
        }

        ~map() = default;

        map& operator=(const map& other) {
            tree_ = other.tree_;

            return *this;
        }

        map& operator=(map&& other) noexcept {
            tree_ = std::move(other.tree_);

            return *this;
        }

        map& operator=(std::initializer_list<value_type> ilist) {
            clear();
            insert(ilist.begin(), ilist.end());

            return *this;
        }

        const T& at(const key_type& key) {
            return lower_bound(key)->second;
        }

        const T& at(const key_type& key) const {
            return lower_bound(key)->second;
        }

        T& operator[](const key_type& key) {
            iterator it = lower_bound(key);
            if (it == end() || key_comp()(key, it->first)) {
                it = insert(value_type(key, T()));
            }

            return it->second;
        }

        T& operator[](Key&& key) {
            // rb_tree 未提供实现
        }

        iterator begin() {
            return tree_.begin();
        }

        const_iterator begin() const {
            return tree_.begin();
        }

        const_iterator cbegin() const {
            return tree_.cbegin();
        }

        iterator end() noexcept {
            return tree_.end();
        }

        iterator end() const noexcept {
            return tree_.end();
        }

        iterator cend() const noexcept {
            return tree_.cend();
        }

        reverse_iterator rbegin() noexcept {
            return tree_.rbegin();
        }

        const_reverse_iterator rbegin() const noexcept {
            return tree_.rbegin();
        }

        const_reverse_iterator crbegin() const noexcept {
            return tree_.crbegin();
        }

        reverse_iterator rend() noexcept {
            return tree_.rend();
        }

        const_reverse_iterator rend() const noexcept {
            return tree_.rend();
        }

        const_reverse_iterator crend() const noexcept {
            return tree_.crend();
        }

        bool empty() const noexcept {
            return tree_.empty();
        }

        size_type size() const noexcept {
            return tree_.size();
        }

        size_type max_size() const noexcept {
            return tree_.max_size();
        }

        void clear() noexcept {
            tree_.clear();
        }

        tinySTL::pair<iterator, bool> insert(const value_type& value) {
            return tree_.insert_unique(value);
        }

        template <class P>
        tinySTL::pair<iterator, bool> insert(P&& value) {
            return tree_.insert_unique(std::forward<P>(value));
        }

        tinySTL::pair<iterator, bool> insert(value_type&& value) {
            return tree_.insert_unique(std::move(value));
        }

        // 插入 value 到尽可能接近，恰好前于(C++11 起) hint 的位置
        iterator insert(const_iterator hint, const value_type& value) {
            return tree_.insert_unique(hint, value);
        }

        template <class P>
        tinySTL::pair<iterator, bool> insert(const_iterator hint, P&& value) {
            return tree_.insert_unique(hint, std::forward<P>(value));
        }

        iterator insert(const_iterator hint, value_type&& value) {
            return tree_.insert_unique(hint, std::move(value));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_unique(first, last);
        }

        void insert(std::initializer_list<value_type> ilist) {
            tree_.insert_unique(ilist.begin(), ilist.end());
        }

        // 若容器中无拥有该关键的元素，则插入以给定的 args 原位构造的新元素到容器。
        template <class... Args>
        tinySTL::pair<iterator, bool> emplace(Args&&... args) {
            return tree_.emplace_unique(std::forward<Args>(args)...);
        }

        template <class... Args>
        iterator emplace_hint( const_iterator hint, Args&&... args ) {
            return insert(hint, std::forward<Args>(args)...);
        }

        // 迭代器 pos 必须合法且可解引用。从而 end() 迭代器（合法，但不可解引用）不能用作 position 所用的值。
        void erase(const_iterator position) {
            tree_.erase(position);
        }

        void erase(iterator first, iterator last) {
            tree_.erase(first, last);
        }

        size_type erase(const key_type& key) {
            return tree_.erase(key);
        }

        void swap(map& other) {
            tree_.swap(other.tree_);
        }

        size_type count(const key_type& key) const {
            return tree_.count(key);
        }

        iterator find(const key_type& key) {
            return tree_.find(key);
        }

        const_iterator find(const key_type& key) const {
            return tree_.find(key);
        }

        tinySTL::pair<iterator, iterator> equal_range(const key_type& key) {
            return tree_.equal_range(key);
        }

        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
            return tree_.equal_range(key);
        }

        iterator lower_bound(const key_type& key) {
            return tree_.lower_bound(key);
        }

        const_iterator lower_bound(const key_type& key) const {
            return tree_.lower_bound(key);
        }

        iterator upper_bound(const key_type& key) {
            return tree_.upper_bound(key);
        }

        const_iterator upper_bound(const key_type& key) const {
            return tree_.upper_bound(key);
        }

        key_compare key_comp() const {
            return tree_.key_comp();
        }

        value_compare value_comp() const {
            return value_compare(tree_.key_comp());
        }
    }; // class map

    template <class Key, class T, class Compare, class Allocator>
    bool operator==(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return lhs.size() == rhs.size() && tinySTL::equal(rhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator!=(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return !(lhs == rhs);
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator<(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return tinySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator<=(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return !(rhs < lhs);
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator>(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return rhs < lhs;
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator>=(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return !(rhs > lhs);
    }

    template <class Key, class T, class Compare, class Allocator>
    void swap(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        lhs.swap(rhs);
    }
} // namespace tinySTL

#endif //TINYSTL_MAP_H
