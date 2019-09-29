#ifndef TINYSTL_MULTISET_H
#define TINYSTL_MULTISET_H

#include <functional>

#include "alloc.h"
#include "utility"
#include "functional.h"
#include "rb_tree.h"

namespace tinySTL {
    template <class Key, class Compare = tinySTL::less<Key>, class Allocator = tinySTL::allocator<Key>>
    class multiset {
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
        multiset() = default;

        explicit multiset(const Compare& comp) : tree_(comp) {}

        template <class InputIterator>
        multiset(InputIterator first, InputIterator last) : tree_(Compare()) {
            tree_.insert_equal(first, last);
        }

        template <class InputIterator>
        multiset(InputIterator first, InputIterator last, const Compare& comp) : tree_(comp) {
            tree_.insert_equal(first, last);
        }

        multiset(const multiset& other) : tree_(other.tree_) {}

        multiset(multiset&& other) noexcept : tree_(std::move(other.tree_)) {}

        multiset& operator=(const multiset& other) {
            tree_ = other.tree_;

            return *this;
        }

        multiset& operator=(multiset&& other) noexcept {
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

        void swap(multiset& other) {
            tree_.swap(other.tree_);
        }

        using pair_iterator_bool = tinySTL::pair<iterator, bool>;
        tinySTL::pair<iterator, bool> insert(const_reference value) {
            return tree_.insert_equal(value);
        }

        tinySTL::pair<iterator, bool> insert(iterator position, const_reference value) {
            return tree_.insert_equal(position, value);
        }

        template <class InputIterator>
        tinySTL::pair<iterator, bool> insert(InputIterator first, InputIterator last) {
            return tree_.insert_equal(first, last);
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

        friend bool operator==(const multiset&, const multiset&);
        friend bool operator<(const multiset&, const multiset&);
    }; // class set

    template <class Key, class Compare, class Allocator>
    bool operator==(const multiset<Key, Compare, Allocator>& lhs, const multiset<Key, Compare, Allocator>& rhs) {
        return lhs.tree_ == rhs.tree_;
    }

    template <class Key, class Compare, class Allocator>
    bool operator<(const multiset<Key, Compare, Allocator>& lhs, const multiset<Key, Compare, Allocator>& rhs) {
        return lhs.tree_ < rhs.tree_;
    }

}


#endif //TINYSTL_MULTISET_H
