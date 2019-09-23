#ifndef TINYSTL_DEQUE_H
#define TINYSTL_DEQUE_H

#include <cstddef> // size_t、ptrdiff_t

#include <limits>               // numeric_limits
#include <algorithm>            // swap、copy
#include <initializer_list>     // initializer_list
#include <utility>              // forward
#include <initializer_list>
#include <stdexcept>            // out_of_range

#include "alloc.h"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "type_traits.h"

namespace tinySTL {
    namespace detail {
        constexpr std::size_t deque_node_size = 8; // 一个缓冲区的容量

        template <class T, class Reference, class Pointer>
        struct deque_iterator {
            using iterator_category = random_access_iterator_tag;   // 迭代器类型
            using value_type        = T;                            // 迭代器所指的对象的类型
            using difference_type   = std::ptrdiff_t;               // 两个迭代器之间的距离
            using pointer           = Pointer;                      // 迭代器所指的对象的指针
            using reference         = Reference;                    // 迭代器所指的对象的引用

            // 迭代器
            using iterator          = deque_iterator<T, T&, T*>;
            using const_iterator    = deque_iterator<T, const T&, const T*>;
            using self              = deque_iterator<T, T&, T*>;

            // 具体迭代器内部指针的指向，见《STL源码剖析》P147
            pointer first_;     // 缓冲区的头
            pointer last_;      // 缓冲区的尾
            pointer current_;   // 迭代器所指的元素
            T** node_;          // 迭代器所在的缓冲区

            deque_iterator() : first_(nullptr), last_(nullptr), current_(nullptr), node_(nullptr) {}

            explicit deque_iterator(const iterator &other) {
                set_node(other.node_);
                current_ = other.current_;
            }

            ~deque_iterator() = default;

            /**
             * 设置迭代器所在的缓冲区。
             * @param newNode 缓冲区
             */
            void set_node(T** newNode) {
                node_ = newNode;
                first_ = *node_;
                last_ = first_ + deque_node_size;
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
                    // 到达尾部，切换到下一个缓冲区
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
                    // 到达头部，切换到上一个缓冲区
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
                // 加上 n 之后，不超出当前缓冲区。
                if (offset >= 0 && offset < deque_node_size) {
                    current_ += n;

                    return *this;
                }

                // 计算向左还是向右超出了多少个缓冲区。
                difference_type nodeOffset = offset > 0 ?
                        offset / deque_node_size : -(-offset - 1) / deque_node_size + 1;
                set_node(node_ + nodeOffset); // 更新缓冲区
                current_ = first_ + (offset - nodeOffset * deque_node_size); // 更新在缓冲区的位置

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

    // 双端队列，支持随机访问。
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/vector
    //
    // deque 的元素是分段存储的，不是想 vector 一样，相接存储。
    // 与 vector 的不同：
    // - deque 没有容量（capacity）的概念， 因为 deque 的元素是分段存储的，
    //   它的存储是按需自动扩容及收缩。
    // - deque 没有 reserve 功能。
    //
    // deque 上常见操作的复杂度（效率）如下：
    // - 随机访问——常数 O(1)
    // - 在结尾或起始插入或移除元素——常数 O(1)
    // - 插入或移除元素——线性 O(n)
    //
    // 虽然 deque 支持随机访问，但它的迭代器不是指针，随机访问是 deque 迭代器经过处理后结果。
    // 所以，使用的时候，尽量选择 vector 而不是 deque。
    //
    // 注意：迭代器失效完全看实现，不同的实现可能有不同的结果。
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

        static constexpr size_type min_map_size_ = 8;   // map 的最小容量

        node_allocator nodeAllocator;                   // 缓冲区元素的空间配置器
        map_allocator mapAllocator;                     // 缓冲区 map 空间配置器

        // [start_, finish_)
        iterator start_;                                // 起始迭代器
        iterator finish_;                               // 末尾迭代器

        // map 的内存分布见《STL源码剖析》P147
        T **map_ = nullptr;                             // 缓冲区数组
        size_type mapSize_ = 0;                         // 缓冲区数组大小

    public:
        deque() {
            empty_initialize();
        }

        explicit deque(size_type count) {
            fill_initialize(count, T());
        }

        deque(size_type count, const_reference value) {
            fill_initialize(count, value);
        }

        template <class InputIterator>
        deque(InputIterator first, InputIterator last) {
            fill_initialize(first, last, is_integral<InputIterator>());
        }

        deque(std::initializer_list<T> ilist) : deque(ilist.begin(), ilist.end()) {

        }

        deque(const deque &other) : deque(other.cbegin(), other.cend()) {

        }

        deque(deque &&other) noexcept {
            move_from(other);
        }

        ~deque() {
            delete_deque();
        }

        deque& operator=(const deque &other) {
            if (this != &other) {
                deque tmp(other);
                swap(tmp);
            }

            return *this;
        }

        deque& operator=(deque &&other) noexcept {
            if (this != &other) {
                move_from(other);
            }

            return *this;
        }

        deque& operator=(std::initializer_list<T> ilist) {
            // 调用了 operator=()
            *this = deque(ilist);

            return *this;
        }

        void assign(size_type count, const_reference value) {
            // 调用了 operator=()
            *this = deque(count, value);
        }

        template <typename InputIterator>
        void assign(InputIterator first, InputIterator last) {
            // 调用了 operator=()
            *this = deque(first, last);
        }

        void assign(std::initializer_list<T> ilist) {
            // 调用了 operator=()
            *this = deque(ilist);
        }

        reference at(size_type position) {
            if (position >= size()) {
                throw std::out_of_range("deque: reference at(size_type position), std::out_of_range");
            }

            return start_[position];
        }

        const_reference at(size_type position) const {
            if (position >= size()) {
                throw std::out_of_range("deque: reference at(size_type position), std::out_of_range");
            }

            return start_[position];
        }

        // deque:operator[] 是不进行边界检查的。
        // 如果越界了，程序会发生段错误。

        reference operator[](size_type position) {
            return start_[static_cast<different_type>(position)];
        }

        const_reference operator[](size_type position) const {
            return start_[static_cast<different_type>(position)];
        }

        reference front() {
            return *start_;
        }

        const_reference front() const {
            return *start_;
        }

        reference back() {
            return *(finish_ - 1);
        }

        const_reference back() const {
            return *(finish_ - 1);
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
            return finish_ == start_;
        }

        size_type size() const {
            return finish_ - start_;
        }

        size_type max_size() const {
            return std::numeric_limits<size_type>::max();
        }

        /**
         * 请求移除未使用的缓冲区。
         * 所有迭代器都会失效。
         */
        void shrink_to_fit() {
            for (auto current = map_; current < start_.node_; ++current) {
                nodeAllocator.deallocate(*current, detail::deque_node_size);
                *current = nullptr;
            }

            for (auto current = finish_.node_ + 1; current < map_ + mapSize_; ++current) {
                nodeAllocator.deallocate(*current, detail::deque_node_size);
                *current = nullptr;
            }
        }

        // 所有迭代器都会失效。
        // TODO 最终需要保留一个缓冲区。这是 deque的策略，也是 deque的初始状态。
        void clear() {
            // TODO 调用 erase()，效率很低。因为 erase() 中有一个将所有元素拷贝到新的起点的操作。而 clear() 完全没必要拷贝。
            erase(start_, finish_);
        }

        // insert() 可能使所有迭代器都会失效。
        // 注意：实际上不一定全都实现，但是存在可能性，所有需要谨慎使用。
        iterator insert(const_iterator position, const_reference value) {
            auto tmp = deque(1, value);
            return insert(position, tmp.begin(), tmp.end());
        }

        iterator insert(iterator position, const value_type &&value) {
            auto tmp = deque(1, value);
            return insert(position, tmp.begin(), tmp.end());
        }

        iterator insert(const_iterator position, size_type count, const_reference value) {
            auto tmp = deque(count, value);
            return insert(position, tmp.begin(), tmp.end());
        }

        template <class InputIterator>
        iterator insert(const_iterator position, InputIterator first, InputIterator last) {
            size_type elementsBefore = position - start_;
            size_type insertSize = last - first;

            // 因为 position 是 const_iterator，需要转成 iterator 才能修改迭代器指向的内容。
            // 不能 const_cast，因为 const_iterator 不是 const iterator。
            auto newPosition = start_ + elementsBefore;
            if (elementsBefore < size() / 2) { // 插入位置离 start_ 更近
                iterator newStart = reserve_elements_at_front(insertSize);
                // 分配好 map 之后，在 newStart 前已经预留好 insertSize 个位置。
                if (elementsBefore < insertSize) {
                    // 可以一次性将 [start_, position) 范围的元素拷贝到以 newStart 的起点的空间上。
                    auto it = std::uninitialized_copy_n(start_, elementsBefore, newStart);
                    std::uninitialized_copy_n(first, insertSize - elementsBefore, it); // TODO 当 first == it 时，还会拷贝吗？
                    std::copy(last - elementsBefore, last, start_);
                } else {
                    // 不可以一次性将 [start_, position) 范围的元素拷贝到以 newStart 的起点的空间上。
                    // 一部分拷贝到为构造的内存空间上，一部分拷贝到已经构造过的内存空间上。
                    std::uninitialized_copy_n(start_, insertSize, newStart);
                    auto it = std::copy(start_ + insertSize, newPosition, start_);
                    std::copy(first, last, it);
                }
                start_ = newStart;
                return newPosition - insertSize;
            } else {

                // 因为 newFinish 是 const_iterator，需要转成 iterator 才能修改迭代器指向的内容。
                // 不能 const_cast，因为 const_iterator 不是 const iterator。
                iterator newFinish = reserve_elements_at_back(insertSize);
                auto elementsAfter = size() - elementsBefore;
                if (elementsAfter < insertSize) {
                    // 拷贝 (last - first - elementsAfter) 个元素
                    auto it = std::uninitialized_copy(first + elementsAfter, last, finish_);
                    // 一次性将 [position, finish_) 范围的元素拷贝到未构造的空间上。
                    std::uninitialized_copy(newPosition, finish_, it);
                    std::copy_n(first, first + elementsAfter, newPosition);
                } else {
                    // 不可以一次性将 [position, finish_) 范围的元素拷贝到未构造的空间上。
                    // 一部分拷贝到为构造的内存空间上，一部分拷贝到已经构造过的内存空间上。
                    std::uninitialized_copy(end() - insertSize, end(), finish_);
                    std::copy_backward(newPosition, end() - insertSize, finish_);
                    std::copy(first, last, newPosition);
                }
                finish_ = newFinish;
                return newPosition;
            }
        }

        iterator insert(const_iterator position, std::initializer_list<T> ilist) {
            auto tmp = deque(ilist);
            return insert(position, tmp.begin(), tmp.begin(), tmp.end());
        }

        // erase 会使所有迭代器都会失效。
        // position == begin()，则 begin() 失效；
        // position == end()，则 end() 失效；
        // 否则，则只有未被 erase 的元素不失效；
        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            insert(position, T(std::forward<Args>(args)...));
        }

        iterator erase(const_iterator first, const_iterator last) {
            if (first == last) {
                return last;
            }

            // 因为 first、last 是 const_iterator，需要转成 iterator 才能修改迭代器指向的内容。
            // 不能 const_cast，因为 const_iterator 不是 const iterator。
            auto newFirst = begin() + (first - cbegin());
            auto newEnd = std::copy(last, cend(), newFirst);

            // 销毁 newEnd 所在的缓冲区中 newEnd 后面的元素。
            for (auto it = newEnd; it != finish_; ++it) {
                nodeAllocator.destory(&*it);
            }

            // 整块地回收整块缓冲区的内存空间。
            for (auto i = newEnd.node_ + 1; i <= finish_.node_; ++i) {
                // TODO 不需要小析构对象再回收内存空间？在 C 中，没有析构，可以直接 free 内存。
                deallocate_node(*i);
            }
            finish_ = newEnd;

            return newFirst;
        }

        iterator erase(const_iterator position) {
            return erase(position, position + 1);
        }

        void push_back(const_reference value) {
            insert(cend(), value);
        }

        // push_back()、emplace_back()、push_front()、emplace_front() 会使所有迭代器失效。
        // 注意：实际上不一定全都实现，但是存在可能性，所有需要谨慎使用。
        // pop_front() 只使 begin() 失效。
        // pop_back() 会是 back()、end() 失效。

        void push_back(const T &&value) {
            insert(cend(), std::move(value));
        }

        template <class... Args>
        iterator emplace_back(Args&&... args) {
            push_back(std::forward(args)...);
        }

        void pop_back() {
            erase(cend() - 1);
        }

        void push_front(const_reference value) {
            insert(cbegin(), value);
        }

        void push_front(const T &&value) {
            insert(cbegin(), std::move(value));
        }

        template <class... Args>
        iterator emplace_front(Args&&... args) {
            push_front(std::forward(args)...);
        }

        void pop_front() {
            erase(cbegin());
        }

        // count > size()，所有迭代器失效；
        // count < size()，被 erase 元素 和 end() 失效；
        // 否则，不失效。
        void resize(size_type count, const_reference value) {
            if (count < size()) {
                erase(cbegin() + count, cend());
            } else {
                insert(cend(), count - size(), value);
            }
        }

        void resize(size_type count) {
            resize(count, T());
        }

        // 迭代器可能失效。
        void swap(deque &other) {
            std::swap(start_, other.start_);
            std::swap(finish_, other.finish_);
            std::swap(map_, other.map_);
            std::swap(mapSize_, other.mapSize_);
        }

    private:
        /**
         * 初始化一个空 deque
         */
        void empty_initialize() {
            create_map_and_nodes();
        }

        /**
         * 初始化，填充 first 个 last 值。
         * @tparam InputIterator
         * @param first 个数
         * @param last 数值
         */
        template <class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, __true_type) {
            fill_initialize(first, last);
        }

        /**
         * 初始化，将 [first, last) 范围的数字拷贝过来。
         * @tparam InputIterator 迭代器类型
         * @param first 起始迭代器
         * @param last 末尾迭代器
         */
        template <class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, __false_type) {
            copy_initialize(first, last, iterator_category(first));
        }

        /**
         * 初始化，填充 count 个 vale 值。
         * @param count 元素个数
         * @param value 值
         */
        void fill_initialize(size_type count, const_reference value) {
            create_map_and_nodes(count);
            for (auto it = start_.node_; it < finish_.node_; ++it) {
                // 整块初填充
                std::uninitialized_fill(*it, *it + detail::deque_node_size, value);
            }
            // 填充最后一个缓冲区
            std::uninitialized_fill(finish_.first_, finish_.current_, value);
        }

        /**
         * 拷贝初始化
         * @tparam InputIterator 迭代器类型
         * @param first 起始迭代器
         * @param last 末尾迭代器
         */
        template <typename InputIterator>
        void copy_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
            create_map_and_nodes();
            while (first != last) {
                reserve_elements_at_back(1);
                // input_iterator_tag 类型迭代器只能逐个遍历。
                nodeAllocator.construct(&*finish_++, *first);
            }
        }

        /**
         * 拷贝初始化
         * @tparam ForwardIterator 迭代器类型
         * @param first 起始迭代器
         * @param last 末尾迭代器
         */
        template <typename ForwardIterator>
        void copy_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
            size_type n = distance(first, last);
            create_map_and_nodes(n);
            for (auto it = start_.node_; it < finish_.node_; ++it) {
                auto nextNode = next(first, detail::deque_node_size);
                // 整块初始化
                std::uninitialized_copy(first, nextNode, it);
                first = next;
            }
            // 初始化最后一个缓冲区
            std::uninitialized_copy(first, last, finish_.first_);
        }

        /**
         * 创建并组织好 deque 结构
         * @param numElements 元素个数
         */
        void create_map_and_nodes(size_type numElements = 0) {
            // 缓冲区个数 = (元素个数 / 每个缓冲区的容量（默认为 8）) + 1
            size_type numNodes = numElements / detail::deque_node_size + 1;
            // 最多会多配置两个缓冲区。
            mapSize_ = std::max(numNodes + 2, min_map_size_);
            map_ = allocate_map();

            // 使 [nStart, nFinish_) 位于中间。
            T **nStart = map_ + (mapSize_ - numNodes) / 2;
            T **nFinish = nStart + numNodes - 1;

            // 分配缓冲区内存，并设置迭代器。
            // 注意：只有 [nStart, nFinish_) 范围分配了缓冲区，
            // 其他 map 位置还未分配缓冲区。当需要的时候，动态扩展。
            for (auto it = nStart; it <= nFinish; ++it) {
                *it = allocate_node();
            }
            start_.set_node(nStart);
            finish_.set_node(nFinish);
            start_.current_ = start_.first_;
            finish_.current_ = finish_.first_ + numElements % detail::deque_node_size;
        }

        /**
         * 删除 deque，并回收内存空间。
         */
        void delete_deque() {
            if (map_ == nullptr) {
                return;
            }
            clear();
            deallocate_node(*start_.node_);
            deallocate_map();
            map_ = nullptr;
        }

        /**
         * 设置 deque 的信息。
         * @param start 起始迭代器
         * @param finish 末尾迭代器
         * @param map map 指针
         * @param mapSize 缓冲区个数
         */
        void set(iterator start, iterator finish, T **map, size_type mapSize) {
            start_ = start;
            finish_ = finish;
            map_ = map;
            mapSize_ = mapSize;
        }

        /**
         * 移动 other 到当前 deque
         * @param other 要移动的 deque
         */
        void move_from(deque &other) {
            delete_deque();
            set(other.start_, other.finish_, other.map_, other.mapSize_);
            other.map_ = nullptr;
        }

        /**
         * 分配 map 的内存空间。
         * @return 内存指针
         */
        T** allocate_map() {
            return mapAllocator.allocate(mapSize_);
        }

        /**
         * 回收 map 的内存空间。
         */
        void deallocate_map() {
            mapAllocator.deallocate(map_, mapSize_);
        }

        /**
         * 分配缓冲区的内存空间。
         * @return 内存指针
         */
        T* allocate_node() {
            return nodeAllocator.allocate(detail::deque_node_size);
        }

        /**
         * 回收缓冲区的内存空间。
         */
        void deallocate_node(pointer ptr) {
            nodeAllocator.deallocate(ptr, detail::deque_node_size);
        }

        /**
         * 在前面预留 n 个元素的位置。
         * @param n 个数
         * @return 更新位置后的 start_ 迭代器
         */
        iterator reserve_elements_at_front(size_type n) {
            // 当前缓冲区剩下的位置的个数
            size_type nBefore = start_.current_ - start_.first_;
            if (n > nBefore) {
                // 空间不够，需要分配新的缓冲区。
                new_elements_at_front(n - nBefore);
            }

            return start_ - n;
        }

        /**
         * 在后面预留 n 个元素的位置。
         * @param n 个数
         * @return 更新位置后的 finish_ 迭代器
         */
        iterator reserve_elements_at_back(size_type n) {
            // 当前缓冲区剩下的位置的个数
            size_type nAfter = finish_.last_ - finish_.current_ - 1;
            if (n > nAfter) {
                // 空间不够，需要分配新的缓冲区。
                new_elements_at_back(n - nAfter);
            }

            return finish_ + n;
        }

        /**
         * 在前面分配几个位置
         * @param n 个数
         */
        void new_elements_at_front(size_type n) {
            // 需要增加的缓冲区的个数
            size_type newNodes = (n + detail::deque_node_size - 1)
                    / detail::deque_node_size;
            reserve_map_at_front(newNodes);
            for (size_type i = 1; i <= newNodes; ++i) {
                *(start_ - i) = allocate_node();
            }
        }

        /**
         * 在后面分配几个位置
         * @param n 个数
         */
        void new_elements_at_back(size_type n) {
            // 需要增加的缓冲区的个数
            size_type newNodes = (n + detail::deque_node_size - 1)
                    / detail::deque_node_size;
            reserve_map_at_back(newNodes);
            for (size_type i = 1; i < newNodes; ++i) {
                *(finish_.node_ + i) = allocate_node();
            }
        }

        /**
         * 在前面预留 n 个缓冲区位置（
         * 只是在 map 中分配了位置，但为实际分配缓冲区的内存空间）。
         * @param n 个数
         */
        void reserve_map_at_front(size_type n = 1) {
            // 如果前面没有足够的位置，才重新分配 map。
            if (n > start_.node_ - map_) {
                reallocate_map(n, true);
            }
        }

        /**
         * 在后面预留 n 个缓冲区。
         * 只是在 map 中分配了位置，但为实际分配缓冲区的内存空间）。
         * @param n 个数
         */
        void reserve_map_at_back(size_type n = 1) {
            // 如果后面没有足够的位置，才重新分配 map。
            if (n > mapSize_ - (finish_.node_ - map_ + 1)) {
                reallocate_map(n, false);
            }
        }

        /**
         * 重新分配 map
         * @param nodesToAdd 增加的缓冲区的个数
         * @param addToFront true：增加到前面；false：增加到后面
         */
        void reallocate_map(size_type nodesToAdd, bool addToFront) {
            size_type oldNumNodes = finish_.node_ - start_.node_ + 1; // 原缓冲区的个数
            size_type newNumNodes = oldNumNodes + nodesToAdd; // 新缓冲区的个数

            T **newStart;
            if (mapSize_ > 2 * newNumNodes) {
                // 现在 map 有足够的位置，所以只需要移动元素即可。
                // 将新的 [start_, finish_) 放到中间。
                // 如果是在前面增加缓冲区，还要将 newStart 向后移动 nodesToAdd 个位置，
                // 预留出来给将要增加的缓冲区。
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
                // 现在 map 没有足够的位置，需要重新分配。
                mapSize_ += std::max(mapSize_, nodesToAdd) + 2;
                auto newMap = allocate_map();
                newStart = newMap + (mapSize_ - newNumNodes) / 2
                        + (addToFront ? nodesToAdd : 0);
                std::copy(start_.node_, finish_.node_ + 1, newStart);
                deallocate_map();
                map_ = newMap;
            }
            start_.set_node(newStart);
            finish_.set_node(newStart + oldNumNodes - 1);
        }
    }; // class deque

    template <class T, class Allocator>
    bool operator==(const deque<T, Allocator> &left, const deque<T, Allocator> &right) {
        return left.size() == right.size() && std::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class T, class Allocator>
    bool operator!=(const deque<T, Allocator> &left, const deque<T, Allocator> &right) {
        return !(left == right);
    }

    template <class T, class Allocator>
    bool operator<(const deque<T, Allocator> &left, const deque<T, Allocator> &right) {
        return std::lexicographical_compare(left.cbegin(), left.cend(), right.cbegin(), right.cend());
    }

    template <class T, class Allocator>
    bool operator<=(const deque<T, Allocator> &left, const deque<T, Allocator> &right) {
        return !(right < left);
    }

    template <class T, class Allocator>
    bool operator>(const deque<T, Allocator> &left, const deque<T, Allocator> &right) {
        return right < left;
    }

    template <class T, class Allocator>
    bool operator>=(const deque<T, Allocator> &left, const deque<T, Allocator> &right) {
        return !(right > left);
    }

    template <class T, class Allocator>
    void swap(const deque<T, Allocator> &left, const deque<T, Allocator> &right) {
        left.swap(right);
    }

} // namespace tinySTL

#endif //TINYSTL_DEQUE_H
