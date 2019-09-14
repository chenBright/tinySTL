#ifndef TINYSTL_DEQUE_H
#define TINYSTL_DEQUE_H

#include <cstddef> // size_t、ptrdiff_t

#include <limits>               // numeric_limits
#include <algorithm>            // swap、copy
#include <initializer_list>     // initializer_list
#include <utility>              // forward

#include "alloc.h"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "type_traits.h"

namespace tinySTL {
    namespace detail {
        constexpr std::size_t deque_node_size = 8;

        template <class T, class Reference, class Pointer>
        struct deque_iterator {
            using iterator_category = random_access_iterator_tag;   // 迭代器类型
            using value_type        = T;                            // 迭代器所指的对象的类型
            using difference_type   = std::ptrdiff_t;               // 两个迭代器之间的距离
            using pointer           = Pointer;                      // 迭代器所指的对象的指针
            using reference         = Reference;                    // 迭代器所指的对象的引用

            using iterator          = deque_iterator<T, T&, T*>;     // 迭代器
            using const_iterator    = deque_iterator<T, const T&, const T*>;
            using self              = deque_iterator<T, T&, T*>;

            pointer first_;
            pointer last_;
            pointer current_;
            T** node_;

            void set_node(T** newNode) {
                node_ = newNode;
                first_ = *node_;
                last_ = first_ + deque_node_size;
            }

            deque_iterator() : first_(nullptr), last_(nullptr), current_(nullptr), node_(nullptr) {}

            explicit deque_iterator(const iterator &other) {
                set_node(other.node_);
                current_ = other.current_;
            }

            self& operator=(const iterator &other) {
                if (this != &other) {
                    set_node(other.node_);
                    current_ = other.current_;
                }

                return *this;
            }

            reference operator*() const {
                return *current_;
            }

            pointer operator->() const {
                return &(operator*());
            }

            self& operator++() {
                ++current_;
                if (current_ == last_) {
                    set_node(node_ + 1);
                    current_ = first_;
                }

                return *this;
            }

            self operator++(int) {
                auto tmp = *this;
                ++*this;

                return tmp;
            }

            self& operator--() {
                if (current_ == first_) {
                    set_node(node_ - 1);
                    current_ = last_;
                }
                --current_;

                return *this;
            }

            self operator--(int) {
                auto tmp = *this;
                --*this;

                return tmp;
            }

            self& operator+=(difference_type n) {
                difference_type offset = n + (current_ - first_);
                if (offset >= 0 && offset < deque_node_size) {
                    current_ += n;

                    return *this;
                }

                difference_type nodeOffset = offset > 0 ?
                        offset / deque_node_size : -(-offset - 1) / deque_node_size + 1;
                set_node(node_ + nodeOffset);
                current_ = first_ + (offset - nodeOffset * deque_node_size);

                return *this;
            }

            self operator+(difference_type n) const {
                auto tmp = *this;
                tmp += n;

                return tmp;
            }

            self& operator-=(difference_type n) {
                *this += -n;

                return *this;
            }

            self operator-(difference_type n) {
                auto tmp = *this;
                tmp -= n;

                return tmp;
            }

            reference operator[](difference_type n) {
                return *(*this + n);
            }
        };

        template <class T, class ReferenceLeft, class PointerLeft, class ReferenceRight, class PointerRight>
        typename deque_iterator<T, ReferenceLeft, PointerLeft>::difference_type
        operator-(
                const deque_iterator<T, ReferenceLeft, PointerLeft> &left,
                const deque_iterator<T, ReferenceRight, PointerRight> &right) {

            if (left.node_ == right.node_) {
                return left.current_ - right.current_;
            }

            // 两个缓冲区之间的结点数 + left 中 [first_, current_) 范围的结点数 + right 中 [current_, last_) 范围的结点数
            return (left.node_ - right.node_ - 1) * deque_node_size
                + (left.current_ - left.first_)
                + (right.last_ - right.current_);
        }

        template <class T, class Reference, class Pointer>
        deque_iterator<T, Reference, Pointer> operator+(
                typename deque_iterator<T, Reference, Pointer>::difference_type n,
                const deque_iterator<T, Reference, Pointer> &right) {

            return right + n;
        }

        template <class T, class ReferenceLeft, class PointerLeft, class ReferenceRight, class PointerRight>
        bool operator==(
                const deque_iterator<T, ReferenceLeft, PointerLeft> &left,
                const deque_iterator<T, ReferenceRight, PointerRight> &right) {

            return left.current_ == right.current_;
        }

        template <class T, class ReferenceLeft, class PointerLeft, class ReferenceRight, class PointerRight>
        bool operator!=(
                const deque_iterator<T, ReferenceLeft, PointerLeft> &left,
                const deque_iterator<T, ReferenceRight, PointerRight> &right) {

            return left.current_ != right.current_;
        }

        template <class T, class ReferenceLeft, class PointerLeft, class ReferenceRight, class PointerRight>
        bool operator<(
                const deque_iterator<T, ReferenceLeft, PointerLeft> &left,
                const deque_iterator<T, ReferenceRight, PointerRight> &right) {

            if (left.node_ == right.node_) {
                return left.current_ < right.current_;
            }

            return left.node_ < right.node_;
        }

        template <class T, class ReferenceLeft, class PointerLeft, class ReferenceRight, class PointerRight>
        bool operator<=(
                const deque_iterator<T, ReferenceLeft, PointerLeft> &left,
                const deque_iterator<T, ReferenceRight, PointerRight> &right) {

            return !(right < left);
        }

        template <class T, class ReferenceLeft, class PointerLeft, class ReferenceRight, class PointerRight>
        bool operator>(
                const deque_iterator<T, ReferenceLeft, PointerLeft> &left,
                const deque_iterator<T, ReferenceRight, PointerRight> &right) {

            return right < left;
        }

        template <class T, class ReferenceLeft, class PointerLeft, class ReferenceRight, class PointerRight>
        bool operator>=(
                const deque_iterator<T, ReferenceLeft, PointerLeft> &left,
                const deque_iterator<T, ReferenceRight, PointerRight> &right) {

            return !(left < right);
        }

    }

    template <class T, class Allocator = allocator<T>>
    class deque {
    public:
        using value_type             = T;               // 数据类型
        using pointer                = T*;              // 指针
        using const_pointer          = const T*;        // 指向常量的指针
        using reference              = T&;              // 引用
        using const_reference        = const T&;        // 常量引用
        using size_type              = std::size_t;     // 数量类型
        using different_type         = std::ptrdiff_t;  // 用来保存两个指针（迭代器）的距离

        // 普通迭代器、反向迭代器
        using iterator               = detail::deque_iterator<T, T&, T*>;
        using const_iterator         = detail::deque_iterator<T, const T&, const T*>;
        using reverse_iterator       = tinySTL::reverse_iterator<iterator>;
        using const_reverse_iterator = tinySTL::reverse_iterator<const_iterator>;

    private:
        using node_allocator = Allocator;
        using map_allocator = typename Allocator::template rebind<T*>::other;

        static constexpr size_type min_map_size_ = 8;

        node_allocator nodeAllocator;
        map_allocator mapAllocator;

        iterator start_;
        iterator finish_;

        T **map_ = nullptr;
        size_type mapSize_ = 0;

    public:
        deque() {
            empty_initialize();
        }

        deque(size_type count, const_reference value) {
            fill_initialize(count, value);
        }

        explicit deque(size_type count) {
            fill_initialize(count, T());
        }

        iterator begin() {
            return start_;
        }

        const_iterator begin() const {
            return start_;
        }

        const_iterator cbegin() {
            return start_;
        }

        iterator end() {
            return finish_;
        }

        const_iterator end() const {
            return finish_;
        }

        const_iterator cend() {
            return finish_;
        }

        reference operator[](size_type n) {
            return start_[static_cast<different_type>(n)];
        }

        reference front() {
            return *start_;
        }

        reference back() {
            return *(finish_ - 1);
        }

        size_type size() const {
            return finish_ - start_;
        }

        size_type max_size() const {
            return std::numeric_limits<size_type>::max();
        }

        bool empty() const {
            return finish_ == start_;
        }

        void push_back(const_reference value) {
            insert(cend(), value);
        }

        void push_front(const_reference value) {
            insert(cbegin(), value);
        }

        void pop_back() {
            erase(cend() - 1);
        }

        void pop_front() {
            erase(cbegin());
        }

        template <class InputIterator>
        iterator insert(const_iterator position, InputIterator first, InputIterator last) {
            size_type elementsBefore = position - start_;
            size_type insertSize = last - first;
            auto newPosition = start_ + elementsBefore;
            if (elementsBefore < size() / 2) { // 插入位置离 start_ 更近
                iterator newStart = reserve_elements_at_front(insertSize);
                // 分配好 map 之后，在 newStart 前已经预留好 insertSize 个位置。
                if (elementsBefore < insertSize) {
                    // 可以一次性将 [start, position) 范围的元素拷贝到以 newStart 的起点的空间上。
                    auto it = std::uninitialized_copy_n(start_, elementsBefore, newStart);
                    std::uninitialized_copy_n(first, insertSize - elementsBefore, it);
                    std::copy(last - elementsBefore, last, start_);
                } else {
                    std::uninitialized_copy_n(start_, insertSize, newStart);
                    auto it = std::copy(start_ + insertSize, newPosition, start_);
                    std::copy(first, last, it);
                }
                start_ = newStart;
                return newPosition - insertSize;
            } else {
                iterator newFinish = reserve_elements_at_back(insertSize);
                auto elementsAfter = size() - elementsBefore;
                if (elementsAfter < insertSize) {
                    auto it = std::uninitialized_copy(first + elementsAfter, last, finish_);
                    std::uninitialized_copy(newPosition, finish_, it);
                    std::copy_n(first, first + elementsAfter, newPosition);
                }
                finish_ = newFinish;
                return newPosition;
            }
        }

        iterator insert(iterator position, const_reference value) {
            return insert(position, deque(1, value));
        }

        iterator insert(iterator position, const value_type &&value) {
            return insert(position, deque(1, value));
        }

        iterator erase(const_iterator first, const_iterator last) {
            auto newFirst = begin() + (first - cbegin());
            auto newEnd = std::copy(last, cend(), newFirst);

            for (auto it = newEnd; it != finish_; ++it) {
                nodeAllocator.destory(&*it);
            }

            for (auto i = newEnd.node_ + 1; i <= finish_.node_; ++i) {
                deallocate_node(*i);
            }
            finish_ = newEnd;

            return newFirst;
        }

        iterator erase(const_iterator position) {
            return erase(position, position + 1);
        }

        // TODO 最终需要保留一个缓冲区。这是 deque的策略，也是 deque的初始状态。
        void clear() {
            erase(start_, finish_);
        }

    private:
        void empty_initialize() {
            create_map_and_nodes();
        }

        void fill_initialize(size_type count, const_reference value) {
            create_map_and_nodes(count);
            for (auto it = start_.node_; it < finish_.node_; ++it) {
                std::uninitialized_fill(*it, *it + detail::deque_node_size, value);
            }
            std::uninitialized_fill(finish_.first_, finish_.current_, value);
        }

        void create_map_and_nodes(size_type numElements = 0) {
            size_type numNodes = numElements / detail::deque_node_size + 1;
            mapSize_ = std::max(numNodes + 2, min_map_size_);
            map_ = allocate_map();

            T **nStart = map_ + (mapSize_ - numNodes) / 2;
            T **nFinish = nStart + numNodes - 1;

            for (auto it = nStart; it <= nFinish; ++it) {
                *it = allocate_node();
            }
            start_.set_node(nStart);
            finish_.set_node(nFinish);
            start_.current_ = start_.first_;
            finish_.current_ = finish_.first_ + numElements % detail::deque_node_size;
        }

        T** allocate_map() {
            return mapAllocator.allocate(mapSize_);
        }

        void deallocate_map() {
            mapAllocator.deallocate(map_, mapSize_);
        }

        T* allocate_node() {
            return nodeAllocator.allocate(detail::deque_node_size);
        }

        void deallocate_node(pointer ptr) {
            nodeAllocator.deallocate(ptr, detail::deque_node_size);
        }

        iterator reserve_elements_at_front(size_type n) {
            size_type nBefore = start_.current_ - start_.first_;
            if (n > nBefore) {
                new_elements_at_front(n - nBefore);
            }

            return start_ - n;
        }

        iterator reserve_elements_at_back(size_type n) {
            size_type nAfter = finish_.last_ - finish_.current_ - 1;
            if (n > nAfter) {
                new_elements_at_back(n - nAfter);
            }

            return finish_ + n;
        }

        void new_elements_at_front(size_type n) {
            size_type newNodes = (n + detail::deque_node_size - 1)
                    / detail::deque_node_size;
            reserve_map_at_front(newNodes);
            for (size_type i = 1; i <= newNodes; ++i) {
                *(start_ - i) = allocate_node();
            }
        }

        void new_elements_at_back(size_type n) {
            size_type newNodes = (n + detail::deque_node_size - 1)
                    / detail::deque_node_size;
            reserve_map_at_back(newNodes);
            for (size_type i = 1; i < newNodes; ++i) {
                *(finish_.node_ + i) = allocate_node();
            }
        }

        void reserve_map_at_front(size_type n = 1) {
            if (n > start_.node_ - map_) {
                reallocate_map(n, true);
            }
        }

        void reserve_map_at_back(size_type n = 1) {
            if (n > mapSize_ - (finish_.node_ - map_ + 1)) {
                reallocate_map(n, false);
            }
        }

        void reallocate_map(size_type nodesToAdd, bool addToFront) {
            size_type oldNumNodes = finish_.node_ - start_.node_ + 1;
            size_type newNumNodes = oldNumNodes + nodesToAdd;

            T **newStart;
            if (mapSize_ > 2 * newNumNodes) {
                newStart = map_ + (mapSize_ - newNumNodes) / 2
                        + (addToFront ? nodesToAdd : 0);
                if (newStart < start_.node_) {
                    std::copy(start_.node_, finish_.node_ + 1, newStart);
                } else {
                    std::copy_backward(start_.node_,
                                       finish_.node_ + 1,
                                       newStart + nodesToAdd);
                }
            } else {
                mapSize_ += std::max(mapSize_, nodesToAdd) + 2;
                auto newMap = allocate_map();
                newStart = newMap + (mapSize_ - newNumNodes) / 2
                        + (addToFront ? nodesToAdd : 0);
                std::copy(start_.node_, finish_.node_ + 1, newStart);
                deallocate_map();
                map_ = newMap;
            }
            start_.set_node(newStart);
            finish_.set_node(newStart + oldNumNodes - 1); // ?
        }
    };
}

#endif //TINYSTL_DEQUE_H
