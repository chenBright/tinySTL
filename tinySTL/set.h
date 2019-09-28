#ifndef TINYSTL_SET_H
#define TINYSTL_SET_H

#include <cstddef> // size_t、ptrdiff_t

#include <functional>

#include "alloc.h"
#include "functional.h"
#include "rb_tree.h"

namespace tinySTL {
    template <class Key, class Compare = tinySTL::less<Key>, class Allocator = tinySTL::allocator<Key>>
    class set {
    public:
        using key_type          = Key;
        using value_type        = Key;
        using key_compare       = Compare;
        using value_compare     = Compare;
        using allocator_type    = Allocator;

    private:
        using rb_tree_type = rb_tree<key_type, value_type, tinySTL::identity<value_type>, key_compare, Allocator>;
        rb_tree_type tree_;

    public:
        using size_type                 = typename rb_tree_type::size_type;
        using difference_type           = typename rb_tree_type::difference_type;
        using reference                 = typename rb_tree_type::const_reference;
        using const_reference           = typename rb_tree_type::const_reference;
        using pointer                   = typename rb_tree_type::const_pointer;
        using const_pointer             = typename rb_tree_type::const_pointer;
        using iterator                  = typename rb_tree_type::const_iterator;
        using const_iterator            = typename rb_tree_type::const_iterator;
        using reverse_iterator          = typename rb_tree_type::const_reverse_iterator;
        using const_reverse_iterator    = typename rb_tree_type::const_reverse_iterator;

    public:
        set() = default;

        explicit set(const Compare& comp) : tree_(comp) {}

        template <class InputIterator>
        set(InputIterator first, InputIterator last) : tree_(Compare()) {
            tree_.insert_unique(first, last);
        }

        template <class InputIterator>
        set(InputIterator first, InputIterator last, const Compare& comp) : tree_(comp) {
            tree_.insert_unique(first, last);
        }

        set(const set& other) : tree_(other.tree_) {}

        set(set&& other) noexcept : tree_(std::move(other.tree_)) {}

        set& operator=(const set& other) {
            tree_ = other.tree_;

            return *this;
        }

        set& operator=(set&& other) noexcept {
            tree_ = std::move(other.tree_);

            return *this;
        }

        key_compare key_comp() const {
            return tree_.key_comp();
        }

        value_compare value_comp() const {
            return tree_.key_comp();
        }

        iterator begin() const {
            return tree_.begin();
        }

        iterator end() const {
            return tree_.end();
        }

        reverse_iterator rbegin() const {
            return tree_.rbegin();
        }

        reverse_iterator rend() const {
            return tree_.rend();
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

        void swap(set& other) {
            tree_.swap(other.tree_);
        }

        using pair_iterator_bool = tinySTL::pair<iterator, bool>;
        tinySTL::pair<iterator, bool> insert(const_reference value) {
            return tree_.insert_unique(value);
        }

        tinySTL::pair<iterator, bool> insert(iterator position, const_reference value) {
            return tree_.insert_unique(position, value);
        }

        template <class InputIterator>
        tinySTL::pair<iterator, bool> insert(InputIterator first, InputIterator last) {
            return tree_.insert_unique(first, last);
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

        iterator find(const_reference value) const {
            return tree_.find(value);
        }

        size_type count(const_reference value) const {
            return tree_.count(value);
        }

        iterator lower_bound(const_reference value) const {
            return tree_.lower_bound(value);
        }

        iterator upper_bound(const_reference value) const {
            return tree_.upper_bound(value);
        }

        tinySTL::pair<iterator, iterator> equal_range(const_reference value) const {
            return tree_.equal_range(value);
        }

        friend bool operator==(const set&, const set&);
        friend bool operator<(const set&, const set&);
    }; // class set

    template <class Key, class Compare, class Allocator>
    bool operator==(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return lhs.tree_ == rhs.tree_;
    }

    template <class Key, class Compare, class Allocator>
    bool operator<(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return lhs.tree_ < rhs.tree_;
    }

}


#endif //TINYSTL_SET_H
