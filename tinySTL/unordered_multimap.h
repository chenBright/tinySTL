#ifndef TINYSTL_UNORDERED_MULTIMAP_H
#define TINYSTL_UNORDERED_MULTIMAP_H

#include <initializer_list>
#include <algorithm>

#include "functional_base.h"
#include "functional_hash.h"
#include "hashtable.h"
#include "alloc.h"
#include "algorithm_base.h"

namespace tinySTL {
    // 无序关联容器，支持等价的关键（一个 unordered_multimap 可含有每个关键值的多个副本）和将关键与另一类型的值关联。
    // unordered_multimap 类支持向前迭代器。
    // 搜索、插入和移除拥有平均常数时间复杂度。
    // 元素在内部不以任何特定顺序排序，而是组织到桶中。
    // 元素被放进哪个桶完全依赖于其关键的哈希。这允许到单独元素的快速访问，因为哈希一旦计算，则它指代元素被放进的准确的桶。
    // 不要求此容器的迭代顺序稳定（故例如 std::equal 不能用于比较二个 std::unordered_multimap ），
    // 除了关键比较等价（以 key_eq() 为比较器比较相等）的每组元素在迭代顺序中组成相接的子范围，它亦可用 equal_range() 访问。
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/unordered_multimap
    //
    // 未实现的接口：
    // iterator insert(const_iterator hint, const value_type& value);
    // iterator insert(const_iterator hint, value_type&& value);
    // max_load_factor();
    // void rehash( size_type count );
    // void reserve( size_type count );
    template <class Key, class T, class Hash = tinySTL::hash<Key>, class KeyEqual = tinySTL::equal_to<Key>,
            class Allocator = tinySTL::allocator<tinySTL::pair<const Key, T>>>
    class unordered_multimap {
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
        unordered_multimap() = default;

        explicit unordered_multimap(size_type bucketCount, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
            : hashtable_(bucketCount, hash, equal) {

        }

        template <class InputIterator>
        unordered_multimap(InputIterator first, InputIterator last, size_type bucketCount = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
            : hashtable_(bucketCount, hash, equal) {
            hashtable_.insert_unique(first, last);
        }

        unordered_multimap(const unordered_multimap& other) : hashtable_(other.hashtable_) {}

        unordered_multimap(unordered_multimap&& other) noexcept : hashtable_(tinySTL::move(other.hashtable_)) {}

        unordered_multimap(std::initializer_list<value_type> ilist, size_type bucketCount = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
        : hashtable_(bucketCount, hash, equal) {
            hashtable_.insert_unique(ilist.begin(), ilist.end());
        }

        ~unordered_multimap() = default;

        unordered_multimap& operator=(const unordered_multimap& other) {
            hashtable_ = other.hashtable_;

            return *this;
        }

        unordered_multimap& operator=(unordered_multimap&& other) noexcept {
            hashtable_ = tinySTL::move(other.hashtable_);

            return *this;
        }

        unordered_multimap& operator=(std::initializer_list<value_type> ilist) {
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
            return hashtable_.insert_equal(value);
        }

        tinySTL::pair<iterator, bool> insert(value_type&& value) noexcept {
            return hashtable_.insert_equal(tinySTL::move(value));
        }

        template <class P>
        tinySTL::pair<iterator, bool> insert(P&& value) {
            return hashtable_.insert_equal(std::forward<P>(value));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            hashtable_.insert_equal(first, last);
        }

        void insert(std::initializer_list<value_type> ilist) {
            hashtable_.insert_equal(ilist);
        }

        template <class... Args>
        tinySTL::pair<iterator, bool> emplace(Args&&... args) {
            return hashtable_.emplace_equal(std::forward<Args>(args)...);
        }

        template <class... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args) {
            return insert(hint, std::forward<Args>(args)...);
        }

        void swap(unordered_multimap& other) {
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
    }; // class unordered_multimap
    template <class Key, class Hash, class KeyEqual, class Allocator>
    bool operator==(const unordered_multimap<Key, Hash, KeyEqual, Allocator>& left, const unordered_multimap<Key, Hash, KeyEqual, Allocator>& right) {
        return left.size() == right.size() && tinySTL::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class Key, class Hash, class KeyEqual, class Allocator>
    bool operator!=(const unordered_multimap<Key, Hash, KeyEqual, Allocator>& left, const unordered_multimap<Key, Hash, KeyEqual, Allocator>& right) {
        return left.size() != right.size() || !tinySTL::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class Key, class Hash, class KeyEqual, class Allocator>
    void swap(unordered_multimap<Key, Hash, KeyEqual, Allocator>& left, unordered_multimap<Key, Hash, KeyEqual, Allocator>& right) {
        left.swap(right);
    }
} // namespace tinySTL

#endif //TINYSTL_UNORDERED_MULTIMAP_H
