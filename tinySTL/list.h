//
// Created by 陈光明 on 2019-08-15.
//

#ifndef TINYSTL_LIST_H
#define TINYSTL_LIST_H

#include <cstddef> // std::ptrdiff_t、std::size_t

#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "alloc.h"

#include <limits>       // numeric_limits
#include <algorithm>    // swap

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
            using iterator          = list_iterator<T, T&, T*>;     // 迭代器
            using self              = list_iterator<T, T&, T*>;

            using iterator_category = bidirectional_iterator_tag;   // 迭代器类型
            using value_type        = T;                            // 迭代器所指的对象的类型
            using difference_type   = std::ptrdiff_t;               // 两个迭代器之间的距离
            using pointer           = Pointer;                      // 迭代器所指的对象的指针
            using reference         = Reference;                    // 迭代器所指的对象的引用

            using size_type         = std::size_t;                  // 数值大小的类型
            using link_type         = detail::list_node<T>*;        // 结点指针类型

            link_type node;                                         // 迭代器所指向的结点的指针

            list_iterator() = default;

            explicit list_iterator(link_type x) : node(x) {}

            explicit list_iterator(const iterator &x) : node(x.node) {}

            bool operator==(const self &other) const {
                return node == other.node;
            }

            bool operator!=(const self &x) const {
                return node != x.node;
            }

            reference operator*() const {
                return node->data;
            }

            pointer operator->() const {
                return &(operator*());
            }

            self& operator++() {
                node = node->next;

                return *this;
            }

            // TODO 加 const：https://stackoverflow.com/questions/52871026/overloaded-operator-returns-a-non-const-and-clang-tidy-complains
            const self operator++(int) {
                self tmp = *this;
                node = node->next;

                return tmp;
            }

            self& operator--() {
                node = node->previous;
                return *this;
            }

            const self operator--(int) {
                self tmp = *this;
                node = node->previous;

                return tmp;
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

        iterator begin() {
            // list_iterator<T, T&, T*> 和 list_node<T> 都是只有一个成员：
            // link_type node_;
            // 所以可以相互转换。
            return static_cast<iterator>(node_->next);
        }

        iterator end() {
            return node_;
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

        reference back() {
            return *--end();
        }

        iterator insert(iterator position, const_reference value) {
            link_type newNode = create_node(value);
            newNode->previous = position.node->previous;
            newNode->next = position.node;
            newNode->previous->next = newNode->next->previous = newNode;

            return static_cast<iterator>(newNode);
        }

        void push_back(const_reference value) {
            insert(end(), value);
        }

        void push_front(const_reference value) {
            insert(begin(), value);
        }

        iterator erase(iterator first, iterator last) {

        }

        iterator erase(iterator position) {
            link_type currentNode = position.node;
            link_type previousNode = currentNode->previous;
            link_type nextNode = currentNode->next;

            // 在链表中断开结点
            previousNode->next = nextNode;
            nextNode->previous = previousNode;

            // 销毁结点
            destroy_node(currentNode);

            return static_cast<iterator>(nextNode);
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

        void swap(list &other) {
            std::swap(node_, other.node_);
        }

        void remove(const_reference value) {
            auto first = begin();
            auto last = end();
            while (first != last) {
                if (*first == value) {
                    first = erase(first);
                } else {
                    ++first;
                }
            }
        }

        // 移除数值相同的连续的元素。
        // 注意：只有"连续且相同"的元素才会被移除剩一个。
        void unique() {
            auto first = begin();
            auto last = end();

            if (first != last) {
                return;
            }

            auto next = first + 1;
            while (next != last) {
                if (*first == *next) {
                    next = erase(next);
                } else {
                    ++first;
                    ++next;
                }
            }
        }


        // 将链表 other 接合到 position 所指的位置之前， other 必须不同于 *this。
        void splice(iterator position, list &other) {
            if (!other.empty()) {
                transfer(position, other.begin(), other.end());
            }
        }

        void splice(iterator position, list&, iterator it) {
            // position == it，表示将 position 所指的结点插入到它自己的前面。
            // position == it + 1，表示将 position 所指的结点插入到它自己的后面。
            if (position == it || position == it + 1) {
                return;
            }

            transfer(position, it, it + 1);
        }

        void splice(iterator position, list&, iterator first, iterator last) {
            if (first != last) {
                transfer(position, first, last);
            }
        }

        void merge(list &other) {
            if (*this == other) {
                return;
            }

            auto first1 = begin();
            auto last1 = end();
            auto first2 = other.begin();
            auto last2 = other.end();

            while (first1 != last1 && first2 != last2) {
                if (*first2 < *first1) {
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

        void reverse() {
            // node_->next = node_ || node_->next->next = node_
            if (size() <= 1) {
                return;
            }

            std::swap(node_->previous, node_->next);
            auto it = node_->previous; // 原 node_->next
            while (it != node_) {
                std::swap(it->previous, it->next);
                it = it->previous; // 原 it->next
            }
        }

        // merge sort
        void sort() {
            if (size() <= 1) {
                return;
            }

            list carry;
            list counter[64];
            size_type fill = 0; // 记录存放有链表的最右边的索引
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin());
                size_type i = 0;
                while (i < fill && !counter[i].empty()) {
                    counter[i].merge(carry);
                    carry.swap(counter[i]);
                }
                carry.swap(counter[i]); // 将链表交换到 counter[i]（空链表）
                if (i == fill) {
                    ++fill;
                }
            }

            for (size_type j = 0; j < fill; ++j) {
                counter[j] .merge(counter[j - 1]);
            }

            swap(counter[fill - 1]);
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

        void transfer(iterator position, iterator first, iterator last) {
            auto positionNode = position.node;
            auto firstNode = first.node;
            auto lastNode = last.node->previous;

            firstNode->previous->next = last.node;
            last.node->previous = firstNode->previous;

            firstNode->previous = positionNode->previous;
            lastNode->next = positionNode;
            firstNode->previous->next = firstNode;
            lastNode->next->previous = lastNode;
        }
    };
}

#endif //TINYSTL_LIST_H
