#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <stdexcept>

#include "alloc.h"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "type_traits.h"

namespace tinySTL {
    // 封装动态数组的顺序容器
    // 接口功能见：https://zh.cppreference.com/w/cpp/container/vector
    //
    // 常见操作的时间复杂度：
    // 随机访问——常数 O(1)
    // 在末尾插入或移除元素——均摊常数 O(1)
    // 插入或移除元素——与到 vector 结尾的距离成线性 O(n)
    //
    //  TODO 没有考虑输入的迭代器所指内存空间可能由不同的 allocator 分配
    template <class T, class Allocator = allocator<T>>
    class vector {
    public:
        using value_type             = T;               // 数据类型
        using pointer                = T*;              // 指针
        using const_pointer          = const T*;        // 指向常量的指针
        using reference              = T&;              // 引用
        using const_reference        = const T&;        // 常量引用
        using size_type              = std::size_t;     // 数量类型
        using different_type         = std::ptrdiff_t;  // 用来保存两个指针（迭代器）的距离

        // 普通迭代器、反向迭代器
        using iterator               = T*;
        using const_iterator         = const T*;
        using reverse_iterator       = tinySTL::reverse_iterator<iterator>;
        using const_reverse_iterator = tinySTL::reverse_iterator<const_iterator>;

        using allocator_type         = Allocator;

    private:
        // [start_, finish_) 范围的内存空间上元素
        iterator start_;            // 目前使用空间的头
        iterator finish_;           // 目前使用空间的尾
        iterator end_of_storage_;   // 目前可用空间的尾

        allocator_type dataAllocator;        // 内存空间配置器

    public:
        // 构造函数相关（部分实现）：https://zh.cppreference.com/w/cpp/container/vector/vector
        vector() : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}

        vector(size_type n, const_reference value) {
            fill_initialize(n, value);
        }

        explicit vector(size_type n) : vector(n, T()) {}

        template <class InputIterator>
        vector(InputIterator first, InputIterator last) : vector() {
            // 如果 InputIterator 为整数类型，则此构造函数拥有的效果同
            // vector(static_cast<size_type>(first), static_cast<value_type>(last))，
            // 实际调用 copy_initialize() 函数。
            // 如果 InputIterator 为迭代器类型，才是迭代器版本的构造函数，
            // 实际调用 initialize_aux() 函数。
            // 所以引入 is_integral，借助模板函数的类型推到功能，用来区分整数类型和迭代器类型。
            initialize_aux(first, last, is_integral<InputIterator>());
        }

        // 拷贝构造函数
        vector(const vector &other) : vector(other.begin(), other.end()) {}

        // 移动构造函数
        vector(vector &&other) noexcept : vector() {
            swap(other);
        }

        /**
         * 列表初始化
         * https://zh.cppreference.com/w/cpp/language/list_initialization
         * @param iList 列表
         */
        vector(std::initializer_list<T> iList) : vector(iList.begin(), iList.end()) {}

        // 析构函数
        // 调用元素的析构函数，然后回收所用的内存空间。
        // 注意，若元素是指针，则不销毁所指向的元素。
        ~vector() {
            clear(); // 析构 vector 内的元素
            if (start_ != nullptr) {
                dataAllocator.deallocate(start_, capacity()); // 回收内存空间
            }
        }

        // 重载 =
        vector& operator=(const vector &rhs) {
            if (this != &rhs) {
                vector tmp(rhs);
                swap(tmp);
            }
            return *this;
        }

        vector& operator=(vector &&rhs) noexcept {
            if (this != &rhs) {
                swap(rhs);
            }
            return *this;
        }

        vector& operator=(std::initializer_list<T> iList) {
            *this = vector(iList);
            return *this;
        }

        vector& assign(size_type n, const_reference value) {
            return *this = vector(n, value);
        }

        // 赋值函数
        template <class InputIterator>
        vector& assign(InputIterator first, InputIterator last) {
            return *this = vector(first, last);
        }

        vector& assign(std::initializer_list<T> ilist) {
            return *this = vector(ilist);
        }

        reference at(size_type position) {
            return check_and_at(position); // 返回位于指定位置 pos 的元素的引用，有边界检查
        }

        const_reference at(size_type position) const {
            return check_and_at(position);
        }

        reference operator[](size_type position) {
            // STL 标准中是不进行边界检查的。
            // 但因为调用了 at() 函数，所以也会进行边界检查
            return at(position);
        }

        const_reference operator[](size_type position) const {
            return at(position);
        }

        reference front() {
            return *begin();
        }

        const_reference front() const {
            return *begin();
        };

        reference back() {
            return *(end() - 1);
        }

        const_reference back() const {
            return *(end() - 1);
        }

        // 元素访问相关函数
        pointer data() {
            return start_; // 迭代器的本质是 T*，所以返回目前使用空间的头，即起始地址
        }

        const_pointer data() const {
            return start_;  // 迭代器的本质是 T*，所以返回目前使用空间的头，即起始地址
        }

        // 迭代器相关函数
        iterator begin() {
            return start_;
        }

        const_iterator begin() const {
            return start_;
        }

        const_iterator cbegin() const {
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

        reverse_iterator rbegin(){
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
            return cbegin() == cend();
        }

        size_type size() const {
            return cend() - cbegin();
        }

        // 返回可容纳的最大元素数（由系统决定）
        size_type max_size() const {
            return std::numeric_limits<size_type>::max();
        }

        // 返回当前存储空间能够容纳的元素数
        size_type capacity() const {
            return end_of_storage_ - start_;
        }

        /**
         * 增加 vector 的容量到大于或等于 new_cap 的值。
         * 若 new_cap 大于当前的 capacity() ，则分配新存储，否则该方法不做任何事。
         * reserve() 不更改 vector 的 size 。
         * 若 new_cap 大于 capacity() ，则所有迭代器，包含尾后迭代器和所有到元素的引用都被非法化。
         * 否则，没有迭代器或引用被非法化。
         *
         * 不能用 reserve() 减少容器容量。为该目的提供的是 shrink_to_fit() 。
         *
         * @param newCapacity 新容量
         */
        void reserve(size_type newCapacity) {
            if (newCapacity > capacity()) {
                realloctae(newCapacity);
            }
        }

        /**
         * 通过释放未使用的内存减少内存的使用.
         * 若发生重分配，则所有迭代器，包含尾后迭代器，和所有到元素的引用都失效。
         * 若不发生重分配，则没有迭代器或引用失效。
         */
        void shrink_to_fit() {
            if (capacity() > size()) {
                realloctae(size());
            }
        }

        // 擦除所有元素
        void clear() {
            erase(begin(), end());
        }

        iterator insert(const_iterator position, const_reference value) {
            vector tmp(1, value);
            return insert(position, tmp.cbegin(), tmp.cend());
        }

        iterator insert(const_iterator position, const T &&value) {
            if (capacity() > size()) {
                auto newPosition = const_cast<iterator>(position);
                dataAllocator.construct(finish_++, value);
                std::copy_backward(position, cend() - 1, end());
                *newPosition = std::move(value);

                return newPosition;
            } else {
                const size_type newSize = empty() ? 1 : 2 * size();
                vector tmp;
                tmp.start_ = dataAllocator.allocate(newSize);
                auto newPosition = tmp.start_ + (position - cbegin());
                tmp.finish_ = std::uninitialized_copy(cbegin(), position, tmp.start_);
                dataAllocator.construct(tmp.finish_++, std::move(value));
                tmp.finish_ = std::uninitialized_copy(position, cend(), tmp.finish_);
                tmp.end_of_storage_ = tmp.start_ + newSize;
                swap(tmp);

                return newPosition;
            }
        }

        iterator insert(const_iterator position, size_type count, const_reference value) {
            vector tmp(count, value);
            return insert(position, tmp.begin(), tmp.end());
        }

        // 若 InputItertor 为整数类型，则此重载与
        // terator insert(iterator position, size_type count, const_reference value)
        // 拥有相同效果。
        // 此重载若为迭代器是才参与重载决议，以免有歧义。
        template <class InputItertor>
        iterator insert(const_iterator position, InputItertor first, InputItertor last) {
            // 如果目前可用空间可以容纳得下 [first, last)
            if (last - first <= capacity() - size()) {
                auto newPosition = const_cast<iterator>(position);
                if (cend() - position >= last - first) {
                    // [position, cend()) 可以容纳得下 [first, last)：
                    // 1. 插入新元素后，[position, end()) 中有一部分元素，
                    //    即 [cend() - (last - first), cend()) 超出了 end()，将其复制到 end() 外；
                    // 2. 将剩下部分额元素，即 [position, cend() - (last - first))，
                    //    复制到当前使用空间的尾部，并与 end() 对齐
                    // 3. 最后，将 [first, last) 范围的元素插入到 newPosition 的位置

                    // TODO tinySTL 版本的 uninitialized_fill_n，注意：此处传入的迭代器有const，有non-const
                    std::uninitialized_copy(cend() - (last - first), cend(), end());
                    // TODO tinySTL 版本的 max，注意：此处传入的迭代器有const，有non-const
                    std::copy_backward(position, cend() - (last - first), end());
                    std::copy(first, last, newPosition);
                    finish_ += last - first;
                } else {
                    // [position, cend()) 可以容纳得不下 [first, last)：
                    // 1. 将容纳不下的 [first, last) 的部分先复制到 end() 起始位置 ；
                    // 2. 将 [position, cend()) 范围的元素复制到步骤1复制过去的元素后面；
                    // 3. 将 [first, last) 剩下部分复制到 newPosition 的位置
                    auto insertEndIterator = std::uninitialized_copy(first + (cend() - position), last, end());
                    std::uninitialized_copy(position, cend(), insertEndIterator);
                    std::copy(first, first + (last - first), newPosition);
                }

                return newPosition;
            }

            // 目前可用空间可以容纳得不下 [first, last)

            // TODO tinySTL 版本的 max
            size_type newSize = size() + std::max(size(), last - first);

            vector tmp;
            tmp.start_ = dataAllocator.allocate(newSize);
            iterator newPosition = tmp.start_ + (position - cbegin());

            // "commit or rollback" semantics
            try {
                tmp.finish_ = std::uninitialized_copy(cbegin(), position, tmp.start_);
                tmp.finish_ = std::uninitialized_copy(first, last, tmp.finish_);
                tmp.finish_ = std::uninitialized_copy(position, cend(), tmp.finish_);
                tmp.end_of_storage_ = tmp.start_ + newSize;
            } catch (...) {
                dataAllocator.destoty(tmp.start_, tmp.finish_);
                dataAllocator.dealloctae(tmp.start_, newSize);
                throw;
            }


            swap(tmp);

            return newPosition;
        }

        iterator insert(const_iterator position, std::initializer_list<value_type> ilist) {
            return insert(position, ilist.begin(), ilist.end());
        }

        // 在容器就地构造元素
        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            // TODO tinySTL 版本的 forward
            return insert(position, T(std::forward<Args>(args)...));
        }

        // 擦除指定位置的元素
        iterator erase(const_iterator position) {
            // 去 const 的原因：
            // 1. 函数的返回值类型为 iteration，不是 const_iteration；
            // 2. first 迭代器所指的位置需要被覆盖。
            auto newPosition = const_cast<iterator>(position);
            if (position + 1 == finish_) {
                // TODO tinySTL 版本的 copy
                // 将 [position + 1, finish_) 范围的元素复制到以 position 为起始的位置
                std::copy(position + 1, finish_, newPosition);
            }
            --finish_;
            dataAllocator.destory(finish_);

            return newPosition;
        }

        iterator erase(const_iterator first, const_reference last) {
            auto newFirst = const_cast<iterator>(first); // 去 const，原因同上
            auto newEnd = std::copy(last, cend(), newFirst);
            dataAllocator.destory(newEnd, end()); // 销毁元素

            return newFirst;
        }

        // 将元素添加到容器末尾
        void push_back(const_reference value) {
            insert(cend(), value);
        }

        void push_back(const T &&value) {
            // TODO tinySTL 版本的 move
            insert(cend(), std::move(value));
        }

        // 在容器末尾就地构造元素
        template <class... Args>
        void emplace_back(Args&&... args) {
            push_back(T(std::forward<Args>(args)...));
        }

        void pop_back() {
            --finish_;
            dataAllocator.destory(finish_); //
        }

        void resize(size_type n, const_reference value) {
           if (n < size()) {
               erase(begin() + n, end());
           } else {
               insert(cend(), n - size(), value);
           }
        }

        void resize(size_type n) {
           resize(n, T());
        }

        /**
         * 交换容器的元素
         * @param other 容器
         */
        void swap(vector &other) noexcept {
            // TODO tinySTL 版本的 swap
            using std::swap;
            swap(start_, other.start_);
            swap(finish_, other.finish_);
            swap(end_of_storage_, other.end_of_storage_);
        }

    private:
        /**
         * 分配内存空间，以及在内存空间中构造元素。
         * @param n 元素个数
         * @param x 元素值
         */
        void fill_initialize(size_type n, const_reference x) {
            iterator result = dataAllocator.allocate(n); // 分配可以容纳 n 个 T 类型大小的内存空间
            // TODO tinySTL 版本的 uninitialized_fill_n
            std::uninitialized_fill_n(result, n, x); // 在内存空间上构造 n 个 T 类型的元素
            // 配置 vector<T> 的内存地址信息
            start_ = result;
            finish_ = start_ + n;
            end_of_storage_ = finish_;
        }

        /**
         * 分配内存空间，并将 [first, last) 复制到内存空间上
         * @tparam ForwardIterator ForwardIterator 以及比 ForwardIterator 强的迭代器类型
         * @param first 起始迭代器
         * @param last 最后一个要析构的迭代器的后一个迭代器
         */
        template <class ForwardIterator>
        void copy_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
            auto copySize = static_cast<size_type>(distance(first, last));
            start_ = dataAllocator.allocate(copySize);
            finish_ = std::uninitialized_copy(first, last, start_);
            end_of_storage_ = finish_;
        }

        // InputIterator 类型的特化版本
        template <class InputIterator>
        void copy_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
            while (first != last) {
                if (capacity() == size()) {
                    realloctae(empty() ? 1 : 2 * size());
                }
                push_back(*first++);
            }
        }

        template <class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, __true_type) {
            fill_initialize(static_cast<size_type>(first), static_cast<value_type>(last));
        }

        template <class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, __false_type) {
            copy_initialize(first, last, iterator_category(first));
        }

        /**
         * 重新分配内存空间
         * @param newSize 新内存空间的大小
         */
        void realloctae(size_type newSize) {
            vector tmp; // 新内存空间
            tmp.start_ = dataAllocator.allocate(newSize);
            tmp.finish_ = std::uninitialized_copy(start_, finish_, tmp.start_);
            tmp.end_of_storage_ = tmp.start_ + newSize;

            swap(tmp);
        }

        /**
         * 返回位于指定位置 pos 的元素的引用，有边界检查。
         * @param position 指定位置
         * @return 元素的引用
         */
        reference check_and_at(size_type position) const {
            // 如果越界，则抛出 std::out_of_range
            if (position >= size()) {
                throw std::out_of_range("Vector");
            }

            return *(begin() + position);
        }

    }; // class vector

    // 全局函数
    // 重载 == != < <= > >=

    template <class T, class Allocator>
    bool operator==(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        // TODO tinySTL 版本的 equal
        return left.size() == right.size() && std::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class T, class Allocator>
    bool operator!=(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return left.size() != right.size() && !std::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    template <class T, class Allocator>
    bool operator<(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        // TODO tinySTL 版本的 lexicographical_compare
        return std::lexicographical_compare(left.cbegin(), left.cend(), right.cbegin(), right.cend());
    }

    template <class T, class Allocator>
    bool operator<=(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return !(right < left); // left 不大于 right => left 小于等于 right
    }

    template <class T, class Allocator>
    bool operator>(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return right < left;
    }

    template <class T, class Allocator>
    bool operator>=(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return right <= left;
    }

    template <class T, class Allocator>
    void swap(vector<T, Allocator> &left, vector<T, Allocator> &right) {
        left.swap(right);
    }

} // namespace tinySTL

#endif //TINYSTL_VECTOR_H
