//
// Created by 陈光明 on 2019-08-15.
//

#ifndef TINYSTL_LIST_H
#define TINYSTL_LIST_H

#include <cstddef> // std::ptrdiff_t、std::size_t

#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "alloc.h"
#include "type_traits.h"

#include <limits>               // numeric_limits
#include <algorithm>            // swap
#include <initializer_list>     // initializer_list
#include <utility>              // forward

namespace tinySTL {
    namespace detail {
        template <class T>
        struct list_node {
            list_node *previous;   // 前一个结点
            list_node *next;       // 后一个结点
            T data;                // 值
        };

        template <class T, class Reference, class Pointer>
        struct list_iterator {
            using iterator_category = bidirectional_iterator_tag;   // 迭代器类型
            using value_type        = T;                            // 迭代器所指的对象的类型
            using difference_type   = std::ptrdiff_t;               // 两个迭代器之间的距离
            using pointer           = Pointer;                      // 迭代器所指的对象的指针
            using reference         = Reference;                    // 迭代器所指的对象的引用

            using iterator          = list_iterator<T, T&, T*>;     // 迭代器
            using const_iterator    = list_iterator<T, const T&, const T*>;
            using self              = list_iterator<T, T&, T*>;

            using link_type         = detail::list_node<T>*;        // 结点指针类型

            link_type node_;                                         // 迭代器所指向的结点的指针

            list_iterator() = default;

            explicit list_iterator(link_type x) : node_(x) {}

            explicit list_iterator(const iterator &x) : node_(x.node_) {}

            reference operator*() const {
                return node_->data;
            }

            pointer operator->() const {
                return &(operator*());
            }

            self& operator++() {
                node_ = node_->next;

                return *this;
            }

            // TODO 加 const：https://stackoverflow.com/questions/52871026/overloaded-operator-returns-a-non-const-and-clang-tidy-complains
            self operator++(int) {
                self tmp = *this;
                node_ = node_->next;

                return tmp;
            }

            self& operator--() {
                node_ = node_->previous;
                return *this;
            }

            self operator--(int) {
                self tmp = *this;
                node_ = node_->previous;

                return tmp;
            }

            bool operator==(const self &other) const {
                return node_ == other.node_;
            }

            bool operator==(const const_iterator &other) const {
                return node_ == other.node_;
            }

            bool operator!=(const self &other) const {
                return node_ != other.node_;
            }

            bool operator!=(const const_iterator &other) const {
                return node_ != other.node_;
            }
        };
    }


    template <class T, class Allocator = allocator<T>>
    class list {
    public:
        using value_type             = T;               // 数据类型
        using pointer                = T*;              // 指针
        using const_pointer          = const T*;        // 指向常量的指针
        using reference              = T&;              // 引用
        using const_reference        = const T&;        // 常量引用
        using size_type              = std::size_t;     // 数量类型
        using different_type         = std::ptrdiff_t;  // 用来保存两个指针（迭代器）的距离

        // 普通迭代器、反向迭代器
        using iterator               = detail::list_iterator<T, T&, T*>;
        using const_iterator         = detail::list_iterator<T, const T&, const T*>;
        using reverse_iterator       = tinySTL::reverse_iterator<iterator>;
        using const_reverse_iterator = tinySTL::reverse_iterator<const_iterator>;

        using link_type = detail::list_node<T>*;

        using allocator_type = Allocator;

        list() {
            node_ = allocate_node();
            // 空链表中，node_ 的头尾都指向 node_ 自己。
            node_->previous = node_->next = node_;
        }

        list(size_type count, const_reference value) : list() {
            insert(begin(), count, value);
        }

        explicit list(size_type count) : list(count, T()) {}

        template <class InputIterator>
        list(InputIterator first, InputIterator last) : list() {
            insert(begin(), first, last);
        }

        list(const list &other) : list(other.begin(), other.end()) {}

        list(std::initializer_list<T> ilist) : list(ilist.begin(), ilist.end()) {}

        list(list &&other) noexcept {
            move_from(other);
        }

        list& operator=(list &&other) noexcept {
            if (this != other) {
                return *this;
            }

            return *this;
        }

        list& operator=(const list &other) {
            if (this != &other) {
                move_from(other);
            }

            return *this;
        }

        list& operator=(std::initializer_list<T> ilist) {
            *this = list(ilist);

            return *this;
        }


        ~list() {
            delete_list();
        }

        iterator begin() {
            // list_iterator<T, T&, T*> 和 list_node<T> 都是只有一个成员：
            // link_type node_;
            // 所以可以相互转换。
            return static_cast<iterator>(node_->next);
        }

        const_iterator begin() const {
            return static_cast<const_iterator>(node_->next);
        }

        const_iterator cbegin() const {
            return static_cast<const_iterator>(node_->next);
        }

        iterator end() {
            return static_cast<iterator>(node_);
        }

        const_iterator end() const {
            return static_cast<const_iterator>(node_);
        }

        const_iterator cend() const {
            return static_cast<const_iterator>(node_);
        }

        reverse_iterator rbegin() {
            return static_cast<reverse_iterator>(end());
        }

        const_reverse_iterator rbegin() const {
            return static_cast<const_reverse_iterator>(end());
        }

        const_reverse_iterator crbegin() const {
            return static_cast<const_reverse_iterator>(end());
        }

        reverse_iterator rend() {
            return static_cast<reverse_iterator>(begin());
        }

        const_reverse_iterator rend() const {
            return static_cast<const_reverse_iterator>(begin());
        }

        const_reverse_iterator crend() const {
            return static_cast<const_reverse_iterator>(begin());
        }

        bool empty() const {
            return node_->next == node_;
        }

        size_type size() const {
            // TODO 使用一个属性记录链表长度，这样 size() 的时间复杂度为 O(1)
            size_type result = 0;
            result = distance(begin(), end());

            return result;
        }

        size_type max_size() const {
            return std::numeric_limits<size_type>::max();
        }

        reference front() {
            return *begin();
        }

        const_reference front() const {
            return *begin();
        }

        reference back() {
            return *--end();
        }

        const_reference back() const {
            return *(--end());
        }

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            *this = list(first, last);
        }

        void assign(size_type count, const_reference value) {
            *this = list(count, value);
        }

        void assign(std::initializer_list<T> ilist) {
            *this = list(ilist);
        }

        iterator insert(const_iterator position, const_reference value) {
            return insert_aux(position, value);
        }

        iterator insert(const_iterator position, T &&value) {
            return insert_aux(position, std::move(value));
        }

        iterator insert(const_iterator position, size_type count, const_reference value) {
            while (count-- > 0) {
                position = insert(position, value);
            }

            return static_cast<iterator>(position.node_);
        }

        // TODO is_integral
        template <class InputIterator>
        iterator insert(iterator position, InputIterator first, InputIterator last) {
            return insert_range_aux(position, first, last, __false_type());
        }

        iterator insert(iterator position, std::initializer_list<T> ilist) {
            return insert(position, ilist.begin(), ilist.end());
        }

        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            return insert(position, T(std::forward<Args>(args)...));
        }

        void push_back(const_reference value) {
            insert(end(), value);
        }

        void push_back(T &&value) {
            insert(end(), std::move(value));
        }

        template <class... Args>
        void emplace_back(Args&&... args) {
            emplace(end(), std::forward<Args>(args)...);
        }

        void push_front(const_reference value) {
            insert(begin(), value);
        }

        void push_front(T &&value) {
            insert(begin(), std::move(value));
        }

        template <class... Args>
        void emplace_front(Args&&... args) {
            emplace(begin(), std::forward<Args>(args)...);
        }

        iterator erase(const_iterator position) {
            link_type currentNode = position.node_;
            link_type previousNode = currentNode->previous;
            link_type nextNode = currentNode->next;

            // 在链表中断开结点
            previousNode->next = nextNode;
            nextNode->previous = previousNode;

            // 销毁结点
            destroy_node(currentNode);

            return static_cast<iterator>(nextNode);
        }


        iterator erase(const_iterator first, const_iterator last) {
            while (first != last) {
                first = erase(first);
            }

            return static_cast<iterator>(last.node_);
        }

        void clear() {
            erase(begin(), end());
        }

        void pop_back() {
            erase(--end());
        }

        void pop_front() {
            erase(begin());
        }

        void resize(size_type count, const_reference value) {
            if (count == size()) {
                return;
            } else if (count < size()) {
                erase(tinySTL::next(begin(), count), end());
            } else {
                insert(end(), count - size(), value);
            }
        }

        void resize(size_type count) {
            resize(count, T());
        }

        void swap(list &other) {
            std::swap(node_, other.node_);
        }

        template <class UnaryPredicate>
        size_type remove_if(UnaryPredicate p) {
            size_type oldSize = size();
            auto first = begin();
            auto last = end();
            while (first != last) {
                if (p(*first)) {
                    first = erase(first);
                } else {
                    ++first;
                }
            }

            return oldSize - size();
        }

        size_type remove(const_reference value) {
            return remove_if([value](const_reference other) {
                return value == other;
            });
        }

        template <class BinaryPredicate>
        size_type unique(BinaryPredicate p) {
            if (node_->next == node_ || node_->next->next == node_) {
                return 0;
            }

            auto first = begin();
            auto last = end();

            auto next = first + 1;
            while (next != last) {
                if (p(*first, *next)) {
                    next = erase(next);
                } else {
                    ++first;
                    ++next;
                }
            }
        }

        // 移除数值相同的连续的元素。
        // 注意：只有"连续且相同"的元素才会被移除剩一个。
        size_type unique() {
            // TODO tinySTL 版本的 equal_to
            return unique(std::equal_to<T>());
        }


        // 将链表 other 接合到 position 所指的位置之前， other 必须不同于 *this。
        void splice(const_iterator position, list &other) {
            if (!other.empty()) {
                transfer(position, other.begin(), other.end());
            }
        }

        void splice(const_iterator position, list &&other) {
            splice(position, other);
        }

        // 以下四个函数中的 list& 或者 list&& 形参，在函数体内没被有使用。
        // 主要作用：用于区分重载版本。

        void splice(iterator position, list&, iterator it) {
            // position == it，表示将 position 所指的结点插入到它自己的前面。
            // position == it + 1，表示将 position 所指的结点插入到它自己的后面。
            if (position == it || position == it + 1) {
                return;
            }

            transfer(position, it, it + 1);
        }

        void splice(iterator position, list &&other, iterator it) {
            splice(position, other, it);
        }

        void splice(iterator position, list&, iterator first, iterator last) {
            if (first != last) {
                transfer(position, first, last);
            }
        }

        void splice(iterator position, list &&other, iterator first, iterator last) {
            splice(position, other, first, last);
        }

        template <class Compare>
        void merge(list &other, Compare compare) {
            if (this == &other) {
                return;
            }

            auto first1 = begin();
            auto last1 = end();
            auto first2 = other.begin();
            auto last2 = other.end();

            while (first1 != last1 && first2 != last2) {
                if (compare(*first2, *first1)) {
                    auto next = first2 + 1;
                    transfer(first1, first2, next);
                    first2 = next;
                } else {
                    ++first1;
                }
            }
            if (first2 != last2) {
                transfer(last1, first2, last2);
            }
        }

        template <class Compare>
        void merge(list &&other, Compare compare) {
            merge(other, compare);
        }

        void merge(list &other) {
            merge(other, std::less<T>());
        }

        void merge(list &&other) {
            // TODO tinySTL 版本的 less、greater
            merge(other);
        }

        void reverse() {
            // 表示 size() <= 1
            if (node_->next == node_ || node_->next->next == node_) {
                return;
            }

            std::swap(node_->previous, node_->next);
            auto it = node_->previous; // 原 node_->next
            while (it != node_) {
                std::swap(it->previous, it->next);
                it = it->previous; // 原 it->next
            }
        }


        // merge sort 迭代实现
        // 参考
        // https://blog.csdn.net/chenhanzhun/article/details/39337331
        // https://www.kancloud.cn/digest/stl-sources/177269
        // http://feihu.me/blog/2014/sgi-std-sort/
        template <class Compare>
        void sort(Compare compare) {
            if (node_->next == node_ || node_->next->next == node_) {
                return;
            }

            list carry; // 起到中间缓存的作用
            // counter[i] 最多保存 2^i 个结点。
            // 如果超出了，则将结点归并到 counter[i + 1] 中
            list counter[64];
            size_type fill = 0; // 记录存放有链表的最右边的索引
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin()); // 取出当前链表的第一个结点
                size_type i = 0;
                // 将取出的结点，从 i = 0 开始合并到 counter 中，
                // 直到遇到空的 counter[i] 或者 最右边。
                while (i < fill && !counter[i].empty()) {
                    counter[i].merge(carry, compare);
                    carry.swap(counter[i]);
                }
                carry.swap(counter[i]); // 将链表交换到 counter[i]（空链表）
                if (i == fill) {
                    ++fill;
                }
            }

            // 合并 counter 中所有的链表
            for (size_type j = 0; j < fill; ++j) {
                counter[j] .merge(counter[j - 1]);
            }

            // 最后把已排序好的链表内容交换到当前链表
            swap(counter[fill - 1]);
        }
        void sort() {
            sort(std::less<T>());
        }



    private:
        using list_node_allocator =
                typename Allocator::template rebind<detail::list_node<T>>::other;

        // node_ 指向末尾的空白结点，即 end()。
        // 它前一个结点是 begin()，后一个结点是 back()。
        link_type node_ = nullptr;

        list_node_allocator listNodeAllocator;

        link_type allocate_node() {
            return listNodeAllocator.allocate(1);
        }

        void deallocate_node(link_type node) {
            listNodeAllocator.deallocate(node);
        }

        link_type create_node(const_reference value) {
            auto newNode = allocate_node();
            return listNodeAllocator.construct(newNode, value);
        }

        void destroy_node(link_type node) {
            listNodeAllocator.destroy(node);
            deallocate_node(node);
        }

        template <class Y>
        iterator insert_aux(const_iterator position, Y &&value) {
            link_type newNode = create_node(std::forward<Y>(value));
            newNode->previous = position.node_->previous;
            newNode->next = position.node_;
            newNode->previous->next = newNode->next->previous = newNode;

            return static_cast<iterator>(newNode);
        }

        template <class InputIterator>
        iterator insert_range_aux(const_iterator position, InputIterator first, InputIterator last, __true_type) {

        }

        template <class InputIterator>
        iterator insert_range_aux(const_iterator position, InputIterator first, InputIterator last, __false_type) {

        }

        void transfer(iterator position, iterator first, iterator last) {
            auto positionNode = position.node_;
            auto firstNode = first.node_;
            auto lastNode = last.node_->previous;

            firstNode->previous->next = last.node_;
            last.node_->previous = firstNode->previous;

            firstNode->previous = positionNode->previous;
            lastNode->next = positionNode;
            firstNode->previous->next = firstNode;
            lastNode->next->previous = lastNode;
        }

        void delete_list() {
            if (node_ == nullptr) {
                return;
            }

            clear();
            // node_->data 为 nullptr，所以只需要回收内存空间即可。
            deallocate_node(node_);
            node_ = nullptr;
        }

        void move_from(list &other) {
            delete_list();
            node_ = other.node_;
            other.node_ = nullptr;
        }
    };
}

#endif //TINYSTL_LIST_H
