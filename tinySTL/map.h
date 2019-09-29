#ifndef TINYSTL_MAP_H
#define TINYSTL_MAP_H

#include <functional>

#include "alloc.h"
#include "utility"
#include "functional.h"
#include "rb_tree.h"

namespace tinySTL {
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
        map(InputIterator first, InputIterator last) : tree_(Compare()) {
            tree_.insert_unique(first, last);
        }

        map(const map &other) : tree_(other.tree_) {}

        map(map &&other) noexcept : tree_(std::move(other.tree_)) {}

        map& operator=(const map& other) {
            tree_ = other.tree_;

            return *this;
        }

        map& operator=(map&& other) noexcept {
            tree_ = std::move(other.tree_);

            return *this;
        }

        key_compare key_comp() const {
            return tree_.key_comp();
        }

        value_compare value_comp() const {
            return value_compare(tree_.key_comp());
        }

        iterator begin() {
            return tree_.begin();
        }

        const_iterator begin() const {
            return tree_.begin();
        }

        iterator end() {
            return tree_.end();
        }

        iterator end() const {
            return tree_.end();
        }

        reverse_iterator rbegin() {
            return tree_.rbegin();
        }

        const_reverse_iterator rbegin() const {
            return tree_.rbegin();
        }

        reverse_iterator rend() {
            return tree_.rend();
        }

        const_reverse_iterator rend() const {
            return tree_.end();
        }

        bool empty() const {
            return tree_.empty();
        }

        size_type size() const {
            return tree_.size();
        }

        size_type max_size() const {
            return tree_.max_size();
        }

        T& operator[](const key_type& key) {
            auto itPair = insert(value_type(key, T()));
            return ((*itPair.first)).second;
        }

        void swap(map& other) {
            tree_.swap(other.tree_);
        }

        tinySTL::pair<iterator, bool> insert(const_reference value) {
            return tree_.insert_unique(value);
        }

        iterator insert(iterator position, const_reference value) {
            return tree_.insert_unique(position, value);
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_unique(first, last);
        }

        void erase(iterator position) {
            tree_.erase(position);
        }

        size_type erase(const_reference value) {
            return tree_.erase(value);
        }

        void erase(iterator first, iterator last) {
            tree_.erase(first, last);
        }

        void clear() {
            tree_.clear();
        }

        iterator find(const key_type& key) {
            return tree_.find(key);
        }

        const_iterator find(const key_type& key) const {
            return tree_.find(key);
        }

        size_type count(const key_type& key) {
            return tree_.count(key);
        }

        iterator lower_bound(const key_type& key) {
            return tree_.lower_bound(key);
        }

        const_iterator lower_bound(const value_type& key) const {
            return tree_.lower_bound(key);
        }

        iterator upper_bound(const key_type& key) {
            return tree_.upper_bound(key);
        }

        const_iterator upper_bound(const key_type& key) const {
            return tree_.upper_bound(key);
        }

        tinySTL::pair<iterator, iterator> equal_range(const key_type& key) {
            return tree_.equal_range(key);
        }

        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
            return tree_.equal_range(key);
        }

        friend bool operator==(const map&, const map&);
        friend bool operator<(const map&, const map&);

    }; // class map

    template <class Key, class T, class Compare, class Allocator>
    bool operator==(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return lhs.tree_ == rhs.tree_;
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator<(const map<Key, T, Compare, Allocator>& lhs, const map<Key, T, Compare, Allocator>& rhs) {
        return lhs.tree_ < rhs.tree_;
    }
}


#endif //TINYSTL_MAP_H
