#ifndef TINYSTL_UNORDERED_MAP_H
#define TINYSTL_UNORDERED_MAP_H

#include <initializer_list>
#include <algorithm>
#include <utility>

#include "functional_base.h"
#include "functional_hash.h"
#include "hashtable.h"
#include "alloc.h"
#include "algorithm_base.h"
#include "utility_move.h"

namespace tinySTL {
    // 关联容器，含有带唯一键的键-值 pair 。
    // 搜索、插入和元素移除拥有平均常数时间复杂度。
    // 元素在内部不以任何特定顺序排序，而是组织进桶中。
    // 元素放进哪个桶完全依赖于其键的哈希。
    // 这允许对单独元素的快速访问，因为一旦计算哈希，则它准确指代元素所放进的桶。
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/unordered_map
    //
    // 未实现的接口：
    // iterator insert(const_iterator hint, const value_type& value);
    // iterator insert(const_iterator hint, value_type&& value);
    // template< class P > iterator insert( const_iterator hint, P&& value );
    // max_load_factor();
    // void rehash( size_type count );
    // void reserve( size_type count );
    template <class Key, class T, class Hash = tinySTL::hash<Key>, class KeyEqual = tinySTL::equal_to<Key>,
            class Allocator = tinySTL::allocator<tinySTL::pair<const Key, T>>>
    class unordered_map {
    public:
        using allocator_type    = Allocator;
        using key_type          = Key;
        using mapped_type       = T;
        using value_type        = tinySTL::pair<const Key, T>;

    private:
        using hash_table_type   = hashtable<key_type, value_type, Hash, tinySTL::select1st<value_type>, KeyEqual, Allocator>;

        hash_table_type hashtable_;

    public:

        using hasher            = typename hash_table_type::hasher;
        using key_equal         = typename hash_table_type::key_equal;

        using size_type         = typename hash_table_type::size_type;
        using difference_type   = typename hash_table_type::difference_type;
        using reference         = typename hash_table_type::reference;
        using const_reference   = typename hash_table_type::const_reference;
        using pointer           = typename hash_table_type::pointer;
        using const_pointer     = typename hash_table_type::const_pointer;
        using iterator          = typename hash_table_type::iterator;
        using const_iterator    = typename hash_table_type::const_iterator;

    public:
        unordered_map() = default;

        explicit unordered_map(size_type bucketCount, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
            : hashtable_(bucketCount, hash, equal) {

        }

        template <class InputIterator>
        unordered_map(InputIterator first, InputIterator last, size_type bucketCount = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
        : hashtable_(bucketCount, hash, equal) {
            hashtable_.insert_unique(first, last);
        }

        unordered_map(const unordered_map& other) : hashtable_(other.hashtable_) {}

        unordered_map(unordered_map&& other) noexcept : hashtable_(tinySTL::move(other.hashtable_)) {}

        unordered_map(std::initializer_list<value_type> ilist, size_type bucketCount = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
        : hashtable_(bucketCount, hash, equal) {
            hashtable_.insert_unique(ilist.begin(), ilist.end());
        }

        ~unordered_map() = default;

        unordered_map& operator=(const unordered_map& other) {
            hashtable_ = other.hashtable_;

            return *this;
        }

        unordered_map& operator=(unordered_map&& other) noexcept {
            hashtable_ = tinySTL::move(other.hashtable_);

            return *this;
        }

        unordered_map& operator=(std::initializer_list<value_type> ilist) {
            clear();
            hashtable_.insert_unique(ilist.begin(), ilist.end());

            return *this;
        }

        iterator begin() noexcept {
            return hashtable_.begin();
        }

        const_iterator begin() const noexcept {
            return hashtable_.begin();
        }

        const_iterator cbegin() const noexcept {
            return hashtable_.cbegin();
        }

        iterator end() noexcept {
            return hashtable_.end();
        }

        const_iterator end() const noexcept {
            return hashtable_.end();
        }

        const_iterator cend() const noexcept {
            return hashtable_.cend();
        }

        bool empty() const noexcept {
            return hashtable_.empty();
        }

        size_type size() const noexcept {
            return hashtable_.size();
        }

        size_type max_size() const noexcept {
            return hashtable_.max_size();
        }

        void clear() {
            hashtable_.clear();
        }

        tinySTL::pair<iterator, bool> insert(const value_type& value) {
            return hashtable_.insert_unique(value);
        }

        tinySTL::pair<iterator, bool> insert(value_type&& value) noexcept {
            return hashtable_.insert_unique(tinySTL::move(value));
        }

        template <class P>
        tinySTL::pair<iterator, bool> insert(P&& value) {
            return hashtable_.insert_unique(std::forward<P>(value));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            hashtable_.insert_unique(first, last);
        }

        void insert(std::initializer_list<value_type> ilist) {
            hashtable_.insert_unique(ilist);
        }

        template <class... Args>
        tinySTL::pair<iterator, bool> emplace(Args&&... args) {
            return hashtable_.emplace_unique(std::forward<Args>(args)...);
        }

        template <class... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args) {
            return insert(hint, std::forward<Args>(args)...);
        }

        void swap(unordered_map& other) {
            hashtable_.swap(other.hashtable_);
        }

        iterator erase(const_iterator position) {
            return hashtable_.erase(position);
        }

        iterator erase(const_iterator first, const_iterator last) {
            return hashtable_.erase(first, last);
        }

        size_type erase(const key_type& key) {
            return hashtable_.erase(key);
        }

        T& at(const key_type& key) {
            return find(key)->second;
        }

        const T& at(const key_type& key) const {
            return find(key)->second;
        }

        T& operator[](const key_type& key) {
            auto it = emplace(key, T()).first;

            return it->second;
        }

        T& operator[](key_type&& key) {
            return operator=(key);
        }

        size_type count(const key_type& key) const {
            return hashtable_.count(key);
        }

        iterator find(const key_type& key) {
            return hashtable_.find(key);
        }

        const_iterator find(const key_type& key) const {
            return hashtable_.find(key);
        }

        tinySTL::pair<iterator, iterator> equal_range(const key_type& key) {
            return hashtable_.equal_range(key);
        }

        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
            return hashtable_.equal_range(key);
        }

        size_type bucket_count() const {
            return hashtable_.bucket_count();
        }

        size_type max_bucket_count() const {
            return hashtable_.max_bucket_count();
        }

        size_type bucket_size(size_type n) const {
            return hashtable_.bucket_size(n);
        }

        size_type bucket(const key_type& key) const {
            return hashtable_.bucket(key);
        }

        float load_factor() const {
            return size() / bucket_count();
        }

        hasher hash_function() const {
            return hashtable_.hash_finction();
        }

        key_equal key_eq() const {
            return hashtable_.key_eq();
        }
    }; // class unordered_map
    template <class Key, class Hash, class KeyEqual, class Allocator>
    bool operator==(const unordered_map<Key, Hash, KeyEqual, Allocator>& left, const unordered_map<Key, Hash, KeyEqual, Allocator>& right) {
        return left.size() == right.size() && tinySTL::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class Key, class Hash, class KeyEqual, class Allocator>
    bool operator!=(const unordered_map<Key, Hash, KeyEqual, Allocator>& left, const unordered_map<Key, Hash, KeyEqual, Allocator>& right) {
        return left.size() != right.size() || !tinySTL::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class Key, class Hash, class KeyEqual, class Allocator>
    void swap(unordered_map<Key, Hash, KeyEqual, Allocator>& left, unordered_map<Key, Hash, KeyEqual, Allocator>& right) {
        left.swap(right);
    }
} // namespace tinySTL


#endif //TINYSTL_UNORDERED_MAP_H
