#ifndef TINYSTL_UNORDERED_MULTISET_H
#define TINYSTL_UNORDERED_MULTISET_H

#include <initializer_list>
#include <algorithm>

#include "functional.h"
#include "functional_hash.h"
#include "hashtable.h"
#include "alloc.h"

namespace tinySTL {
    // 关联容器，含有可能非唯一 Key 类型对象的集合。
    // 搜索、插入和移除拥有平均常数时间复杂度。
    // 元素在内部并不以任何顺序排序，只是被组织到桶中。
    // 元素被放入哪个桶完全依赖其值的哈希。这允许快速访问单独的元素，因为一旦计算哈希，它就指代放置该元素的准确的桶。
    // 不要求此容器的迭代顺序稳定（故例如 std::equal 不能用于比较二个 std::unordered_multiset ），
    // 除了关键比较等价（以 key_eq() 为比较器比较相等）的每组元素组成迭代顺序中的相接子范围，它可用 equal_range() 访问。
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/unordered_multiset
    //
    // 未实现的接口：
    // iterator insert(const_iterator hint, const value_type& value);
    // iterator insert(const_iterator hint, value_type&& value);
    // max_load_factor();
    // void rehash( size_type count );
    // void reserve( size_type count );
    template <class Key, class Hash = tinySTL::hash<Key>, class KeyEqual = tinySTL::equal_to<Key>,
            class Allocator = tinySTL::allocator<Key>>
    class unordered_multiset {
    public:
        using allocator_type    = Allocator;
        using key_type          = Key;
        using value_type        = Key;

    private:
        using hash_table_type   = hashtable<key_type, value_type, Hash, tinySTL::identity<value_type>, KeyEqual, Allocator>;

        hash_table_type hashtable_;

    public:

        using hasher            = typename hash_table_type::hasher;
        using key_equal         = typename hash_table_type::key_equal;

        using size_type         = typename hash_table_type::size_type;
        using difference_type   = typename hash_table_type::difference_type;
        using reference         = typename hash_table_type::const_reference;
        using const_reference   = typename hash_table_type::const_reference;
        using pointer           = typename hash_table_type::const_pointer;
        using const_pointer     = typename hash_table_type::const_pointer;
        using iterator          = typename hash_table_type::const_iterator;
        using const_iterator    = typename hash_table_type::const_iterator;

    public:
        unordered_multiset() = default;
        explicit unordered_multiset(size_type bucketCount, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
            : hashtable_(bucketCount, hash, equal) {

        }

        template <class InputIterator>
        unordered_multiset(InputIterator first, InputIterator last, size_type bucketCount = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
            : hashtable_(bucketCount, hash, equal) {
            hashtable_.insert_unique(first, last);
        }

        unordered_multiset(const unordered_multiset& other) : hashtable_(other.hashtable_) {}

        unordered_multiset(unordered_multiset&& other) noexcept : hashtable_(std::move(other.hashtable_)) {}

        unordered_multiset(std::initializer_list<value_type> ilist, size_type bucketCount = 100, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
            : hashtable_(bucketCount, hash, equal) {
            hashtable_.insert_unique(ilist.begin(), ilist.end());
        }

        ~unordered_multiset() = default;

        unordered_multiset& operator=(const unordered_multiset& other) {
            hashtable_ = other.hashtable_;

            return *this;
        }

        unordered_multiset& operator=(unordered_multiset&& other) noexcept {
            hashtable_ = std::move(other.hashtable_);

            return *this;
        }

        unordered_multiset& operator=(std::initializer_list<value_type> ilist) {
            clear();
            hashtable_.insert_unique(ilist.begin(), ilist.end());

            return *this;
        }

        iterator begin() noexcept {
            return hashtable_.begin();
        }

        const_iterator begin() const noexcept {
            return hashtable_.cbegin();
        }

        const_iterator cbegin() const noexcept {
            return hashtable_.begin();
        }

        iterator end() noexcept {
            return hashtable_.end();
        }

        const_iterator end() const noexcept {
            return hashtable_.cend();
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

        iterator insert(const value_type& value) {
            return hashtable_.insert_equal(value);
        }

        iterator insert(value_type&& value) noexcept {
            return hashtable_.insert_equal(std::move(value));
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

        iterator erase(const_iterator position) {
            return hashtable_.erase(position);
        }

        iterator erase(const_iterator first, const_iterator last) {
            return hashtable_.erase(first, last);
        }

        size_type erase(const key_type& key) {
            return hashtable_.erase(key);
        }

        void swap(unordered_multiset& other) {
            hashtable_.swap(other.hashtable_);
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
    }; // class unordered_multiset
    template <class Key, class Hash, class KeyEqual, class Allocator>
    bool operator==(const unordered_multiset<Key, Hash, KeyEqual, Allocator>& left, const unordered_multiset<Key, Hash, KeyEqual, Allocator>& right) {
        return left.size() == right.size() && tinySTL::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class Key, class Hash, class KeyEqual, class Allocator>
    bool operator!=(const unordered_multiset<Key, Hash, KeyEqual, Allocator>& left, const unordered_multiset<Key, Hash, KeyEqual, Allocator>& right) {
        return left.size() != right.size() || !tinySTL::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class Key, class Hash, class KeyEqual, class Allocator>
    void swap(unordered_multiset<Key, Hash, KeyEqual, Allocator>& left, unordered_multiset<Key, Hash, KeyEqual, Allocator>& right) {
        left.swap(right);
    }
}


#endif //TINYSTL_UNORDERED_MULTISET_H
