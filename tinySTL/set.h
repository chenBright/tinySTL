#ifndef TINYSTL_SET_H
#define TINYSTL_SET_H

#include <functional>
#include <initializer_list>
#include <algorithm>
#include <utility>

#include "alloc.h"
#include "utility_move.h"
#include "functional_base.h"
#include "rb_tree.h"
#include "algorithm_base.h"

namespace tinySTL {
    // 关联容器，含有 Key 类型对象的已排序集。通常其底层实现为红黑树。
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/set
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
        set(InputIterator first, InputIterator last, const Compare& comp = Compare()) : tree_(comp) {
            tree_.insert_unique(first, last);
        }

        set(const set& other) : tree_(other.tree_) {}

        set(set&& other) noexcept : tree_(tinySTL::move(other.tree_)) {}

        set(std::initializer_list<value_type> ilist, const Compare& comp = Compare()) : tree_(comp) {
            tree_.insert_unique(ilist.begin(), ilist.end());
        }

        ~set() = default;

        set& operator=(const set& other) {
            tree_ = other.tree_;

            return *this;
        }

        set& operator=(set&& other) noexcept {
            tree_ = tinySTL::move(other.tree_);

            return *this;
        }

        set& operator=(std::initializer_list<value_type> ilist) {
            clear();
            insert(ilist.begin(), ilist.end());

            return *this;
        }

        iterator begin() noexcept {
            return tree_.begin();
        }

        const_iterator begin() const noexcept {
            return tree_.begin();
        }

        const_iterator cbegin() const noexcept {
            return tree_.cbegin();
        }

        iterator end() noexcept {
            return tree_.end();
        }

        const_iterator end() const noexcept {
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

        tinySTL::pair<iterator, bool> insert(value_type&& value) {
            return tree_.insert_unique(tinySTL::move(value));
        }

        iterator insert(const_iterator hint, const value_type& value) {
            return tree_.insert_unique(hint, value);
        }

        iterator insert(const_iterator hint, value_type&& value) {
            return tree_.insert_unique(hint, tinySTL::move(value));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_unique(first, last);
        }

        void insert(std::initializer_list<value_type> ilist) {
            tree_.insert_unique(ilist.begin(), ilist.begin());
        }

        template <class... Args>
        std::pair<iterator, bool> emplace(Args&&... args) {
            return tree_.emplace_unique(tinySTL::forward<Args>(args)...);
        }

        // 插入新元素到容器中尽可能接近于恰在 hint 前的位置。
        template <class... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args) {
            return insert(hint, tinySTL::forward<Args>(args)...);
        }

        iterator erase(const_iterator position) {
            return tree_.erase(position);
        }

        iterator erase(const_iterator first, const_iterator last) {
            return tree_.erase(first, last);
        }

        size_type erase(const_reference value) {
            return tree_.erase(value);
        }

        size_type count(const_reference value) const {
            return tree_.count(value);
        }

        iterator find(const_reference value) {
            return tree_.find(value);
        }

        const_iterator find(const_reference value) const {
            return tree_.find(value);
        }

        tinySTL::pair<iterator, iterator> equal_range(const_reference value) {
            return tree_.equal_range(value);
        }

        tinySTL::pair<const_iterator, const_iterator> equal_range(const_reference value) const {
            return tree_.equal_range(value);
        }

        iterator lower_bound(const key_type& value) {
            return tree_.lower_bound(value);
        }

        const_iterator lower_bound(const key_type& value) const {
            return tree_.lower_bound(value);
        }

        iterator upper_bound(const key_type& value) {
            return tree_.upper_bound(value);
        }

        const_iterator upper_bound(const key_type& value) const {
            return tree_.upper_bound(value);
        }

        key_compare key_comp() const {
            return tree_.key_comp();
        }

        value_compare value_comp() const {
            return tree_.key_comp();
        }

        void swap(set& other) {
            tree_.swap(other.tree_);
        }
    }; // class set

    template <class Key, class Compare, class Allocator>
    bool operator==(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return lhs.size() == rhs.size() && tinySTL::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template <class Key, class Compare, class Allocator>
    bool operator!=(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return !(lhs == rhs);
    }

    template <class Key, class Compare, class Allocator>
    bool operator<(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return tinySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template <class Key, class Compare, class Allocator>
    bool operator<=(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return !(rhs < lhs);
    }

    template <class Key, class Compare, class Allocator>
    bool operator>(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return rhs < lhs;
    }

    template <class Key, class Compare, class Allocator>
    bool operator>=(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        return !(rhs > lhs);
    }

    template <class Key, class Compare, class Allocator>
    void swap(const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs) {
        lhs.swap(rhs);
    }
} // namespace tinySTL


#endif //TINYSTL_SET_H
