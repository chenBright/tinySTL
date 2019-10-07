#ifndef TINYSTL_MULTIMAP_H
#define TINYSTL_MULTIMAP_H

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
    // 关联容器，含有关键-值 pair 的已排序列表，同时容许多个入口拥有同一关键。通常其底层实现为红黑树。
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/multimap
    template <class Key, class T, class Compare = tinySTL::less<Key>, class Allocator = tinySTL::allocator<tinySTL::pair<Key,T>>>
    class multimap {
    public:
        using key_type          = Key;
        using mapped_type       = T;
        using value_type        = tinySTL::pair<const Key, T>;
        using key_compare       = Compare;
        using allocator_type    = Allocator;

        // https://zh.cppreference.com/w/cpp/container/multimap/value_compare
        class value_compare : public tinySTL::binary_finction<value_type, value_type, bool> {
            friend class multimap<Key, T, Compare, Allocator>;
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
        multimap() = default;

        explicit multimap(const Compare& comp) : tree_(comp) {}

        template <class InputIterator>
        multimap(InputIterator first, InputIterator last, const Compare& comp = Compare()) : tree_(comp) {
            tree_.insert_equal(first, last);
        }

        multimap(const multimap &other) : tree_(other.tree_) {}

        multimap(multimap &&other) noexcept : tree_(tinySTL::move(other.tree_)) {}

        multimap(std::initializer_list<value_type> ilist, const Compare& comp = Compare()) : tree_(comp) {
            tree_.insert_equal(ilist.begin(), ilist.end());
        }

        multimap& operator=(const multimap& other) {
            tree_ = other.tree_;

            return *this;
        }

        multimap& operator=(multimap&& other) noexcept {
            tree_ = tinySTL::move(other.tree_);

            return *this;
        }

        multimap& operator=(std::initializer_list<value_type> ilist) {
            clear();
            tree_.insert_equal(ilist.begin(), ilist.end());

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

        iterator insert(const value_type& value) {
            return tree_.insert_equal(value);
        }

        iterator insert(value_type&& value) {
            return tree_.insert_equal(tinySTL::move(value));
        }

        template <class P>
        iterator insert(P&& value) {
            return tree_.insert_equal(std::forward<P>(value));
        }

        iterator insert(const_iterator hint, const value_type& value) {
            return tree_.insert_equal(hint, value);
        }

        template <class P>
        iterator insert(const_iterator hint, P&& value) {
            return tree_.insert_equal(hint, std::forward<P>(value));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_equal(first, last);
        }

        void insert(std::initializer_list<value_type> ilist) {
            tree_.insert_equal(ilist.begin(), ilist.end());
        }

        template <class... Args>
        iterator emplace(Args&&... args) {
            return tree_.emplace_equal(std::forward<Args>(args)...);
        }

        // 插入新元素到尽可能接近恰在 hint 前的位置。
        // 原位构造元素，即不进行复制或移动操作。
        template <class... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args) {
            return tree_.insert_equal(hint, std::forward<Args>(args)...);
        }

        // 迭代器 pos 必须合法且可解引用。
        // 所以 end() 迭代器（合法，但不可解引用）不能用作 pos 所用的值。
        iterator erase(const_iterator position) {
            return tree_.erase(position);
        }

        iterator erase(iterator first, iterator last) {
            return tree_.erase(first, last);
        }

        size_type erase(const key_type& key) {
            return tree_.erase(key);
        }

        void swap(multimap& other) {
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

        const_iterator lower_bound(const value_type& key) const {
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
    }; // class multimap

    template <class Key, class T, class Compare, class Allocator>
    bool operator==(const multimap<Key, T, Compare, Allocator>& lhs, const multimap<Key, T, Compare, Allocator>& rhs) {
        return lhs.size() == rhs.size() && tinySTL::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator!=(const multimap<Key, T, Compare, Allocator>& lhs, const multimap<Key, T, Compare, Allocator>& rhs) {
        return !(lhs == rhs);
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator<(const multimap<Key, T, Compare, Allocator>& lhs, const multimap<Key, T, Compare, Allocator>& rhs) {
        return tinySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator<=(const multimap<Key, T, Compare, Allocator>& lhs, const multimap<Key, T, Compare, Allocator>& rhs) {
        return !(rhs < lhs);
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator>(const multimap<Key, T, Compare, Allocator>& lhs, const multimap<Key, T, Compare, Allocator>& rhs) {
        return rhs < lhs;
    }

    template <class Key, class T, class Compare, class Allocator>
    bool operator>=(const multimap<Key, T, Compare, Allocator>& lhs, const multimap<Key, T, Compare, Allocator>& rhs) {
        return !(rhs > lhs);
    }

    template <class Key, class T, class Compare, class Allocator>
    void swap(const multimap<Key, T, Compare, Allocator>& lhs, const multimap<Key, T, Compare, Allocator>& rhs) {
        lhs.swap(rhs);
    }
} // namespace tinySTL

#endif //TINYSTL_MULTIMAP_H
