#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include "alloc.h"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "type_traits.h"

#include <cassert>

#include <initializer_list>
#include <limits>
#include <vector>
#include <memory>

namespace tinySTL {
    template <class T, class Allocator = allocator<T>>
    class vector {
    public:
        using value_type             = T;            // 数据类型
        using pointer                = T*;           // 指针
        using const_pointer          = const T*;     // 指向常量的指针
        using reference              = T&;           // 引用
        using const_reference        = const T&;     // 常量引用
        using size_type              = size_t;       // 数量类型
        using different_type         = ptrdiff_t;    // 用来保存两个指针（迭代器）的距离

        // 普通迭代器、反向迭代器
        using iterator               = T*;
        using const_iterator         = const T*;
        using reverse_iterator       = tinySTL::reverse_iterator<iterator>;
        using const_reverse_iterator = tinySTL::reverse_iterator<const_iterator>;

        vector() : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}

        vector(size_type n, const_reference value) {
            fill_initialize(n, value);
        }

        explicit vector(size_type n) : vector(n, T()) {}

        template <class InputIterator>
        vector(InputIterator first, InputIterator last) : vector() {
            copy_initialize(first, last, iterator_category(first));
        }

        vector(std::initializer_list<T> iList) : vector(iList.begin(), iList.end()) {}

        vector(const vector &other) : vector(other.begin(), other.end()) {}

        vector& operator=(const vector &rhs) {
            if (this != &rhs) {
                vector tmp(rhs);
                swap(tmp);
            }
            return *this;
        }

        vector& operator=(std::initializer_list<T> ilist) {
            return *this = vector(ilist);
        }

        vector(vector &&other) noexcept : vector() {
            // TODO 实参类型是右值，形参类型是左值？
            swap(other); // TODO 需要 move 吗？
        }

        vector& operator=(vector &&rhs) noexcept {
            if (this != &rhs) {
                swap(rhs); // TODO 需要 move 吗？
            }
            return *this;
        }

        ~vector() {
            clear(); // 析构 vector 内的对象
            if (start_ != nullptr) {
                dataAllocator.deallocate(start_, capacity()); // 回收内存空间
            }
        }


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

        pointer data() {
            return start_; // 迭代器的本质是 T*，所以返回目前使用空间的头，即起始地址
        }

        const_pointer data() const {
            return start_;  // 迭代器的本质是 T*，所以返回目前使用空间的头，即起始地址
        }

        reference at(size_type position) {
            assert(position >= 0);
//            return *next(start_, position);
            return *(begin() + position);
        }

        const_reference at(size_type position) const {
            assert(position >= 0);
            return *(begin() + position);
        }

        reference operator[](size_type position) {
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

        bool empty() const {
            return cbegin() == cend();
        }

        size_type size() const {
            return cend() - cbegin();
        }

        size_type max_size() const {
            return std::numeric_limits<size_type>::max();
        }

        size_type capacity() const {
            return end_of_storage_ - start_;
        }

        // TODO tinySTL 版本的 swap、move
        // TODO 为什么 swap、move 函数可以访问对象的私有成员？因为是友元函数？
        void swap(vector &other) noexcept {
            std::swap(start_, other.start_);
            std::swap(finish_, other.finish_);
            std::swap(end_of_storage_, other.end_of_storage_);
        }


        void push_back(const_reference value) {
            insert(cend(), value);
        }

        void push_back(const T &&value) {
            // TODO tinySTL 版本的 move
            insert(cend(), std::move(value));
        }

        void pop_back() {
            --finish_;
            dataAllocator.destory(finish_);
        }

        iterator erase(const_iterator position) {
            auto newPosition = const_cast<iterator>(position);
            if (position + 1 == finish_) {
                // TODO tinySTL 版本的 copy
                // 将 [position + 1, finish_) 范围的对象复制到以 position 为起始的位置
                std::copy(position + 1, finish_, newPosition);
            }
            --finish_;
            dataAllocator.destory(finish_);

            return position;
        }

        iterator erase(const_iterator first, const_reference last) {
            auto newFirst = const_cast<iterator>(first); // 去 const
            auto newEnd = std::copy(last, cend(), newFirst);
            dataAllocator.destory(newEnd, end());

            return newFirst;
        }

        void reverse(size_type newCapacity) {
           if (newCapacity > capacity()) {
               realloctae(newCapacity);
           }
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

        // TODO 几个 insert 函数的代码很相似，需要将重复的逻辑抽象出函数
        iterator insert(const_iterator position, const vector &other) {
            return insert(position, other.begin(), other.end());
        }

        template <class InputItertor>
        iterator insert(const_iterator position, InputItertor first, InputItertor last) {
            if (last - first <= capacity() - size()) {
                auto newPosition = const_cast<iterator>(position);
                if (cend() - position >= last - first) { // [position, cend()) 可以装得下 [first, last)
                    // TODO tinySTL 版本的 uninitialized_fill_n，注意：此处传入的迭代器有const，有non-const
                    std::uninitialized_copy(cend() - (last - first), cend(), end());
                    // TODO tinySTL 版本的 max，注意：此处传入的迭代器有const，有non-const
                    std::copy_backward(position, cend() - (last - first), end());
                    std::copy(first, last, newPosition);
                    finish_ += last - first;
                } else {
                    auto insertEndIterator = std::uninitialized_copy(first + (cend() - position), last, end());
                    std::uninitialized_copy(position, cend(), insertEndIterator);
                    std::copy(first, first + (last - first), newPosition);
                }

                return newPosition;
            }

            // TODO tinySTL 版本的 max
            size_type newSize = size() + std::max(size(), last - first);
            // TODO 遇到异常，还原现场
            vector tmp;
            tmp.start_ = dataAllocator.allocate(newSize);
            iterator newPosition = tmp.start_ + (position - cbegin());
            tmp.finish_ = std::uninitialized_copy(cbegin(), position, tmp.start_);
            tmp.finish_ = std::uninitialized_copy(first, last, tmp.finish_);
            tmp.finish_ = std::uninitialized_copy(position, cend(), tmp.finish_);
            tmp.end_of_storage_ = tmp.start_ + newSize;

            swap(tmp);

            return newPosition;
        }

        iterator insert(const_iterator position, std::initializer_list<value_type> ilist) {
            return insert(position, ilist.begin(), ilist.end());
        }

        iterator insert(const_iterator position, size_type n, const_reference value) {
            vector tmp(n, value);
            return insert(position, tmp.begin(), tmp.end());
        }

        iterator insert(const_iterator position, const_reference value) {
            // 改为
            // vector tmp(1, value);
            // insert(position, tmp.cbegin(), tmp.cend());

            if (capacity() > size()) {
                dataAllocator.construct(finish_, value);
                ++finish_;
                // 将 [position, cend() - 1) 范围内所有元素向后挪一个位置
                std::copy_backward(position, cend() - 1, cend());

                auto newPosition = const_cast<iterator>(position);
                *position = value;

                return newPosition;
            }

            size_type newSize = empty() == 0 ? 1 : 2 * size();
            vector tmp;
            tmp.start_ = dataAllocator.allocate(newSize);
            iterator newPosition = tmp.start_ + (position - cbegin());
            tmp.finish_ = std::uninitialized_copy(cbegin(), position, tmp.start_);
            tmp.finish_ = std::uninitialized_fill_n(tmp.finish_, 1, value);
            tmp.finish_ = std::uninitialized_copy(position, cend(), tmp.finish_);
            tmp.end_of_storage_ = tmp.start_ + newSize;

            swap(tmp);

            return newPosition;
        }

        iterator insert(const_iterator position, const T &&value) {
            if (capacity() > size()) {
                dataAllocator.construct(finish_, value);
                ++finish_;
                // 将 [position, cend() - 1) 范围内所有元素向后挪一个位置
                std::copy_backward(position, cend() - 1, cend());

                auto newPosition = const_cast<iterator>(position);
                *position = std::move(value);

                return newPosition;
            }

            size_type newSize = empty() == 0 ? 1 : 2 * size();
            vector tmp;
            tmp.start_ = dataAllocator.allocate(newSize);
            iterator newPosition = tmp.start_ + (position - cbegin());
            tmp.finish_ = std::uninitialized_copy(cbegin(), position, tmp.start_);
            tmp.finish_ = std::uninitialized_fill_n(tmp.finish_, 1, std::move(value));
            tmp.finish_ = std::uninitialized_copy(position, cend(), tmp.finish_);
            tmp.end_of_storage_ = tmp.start_ + newSize;

            swap(tmp);

            return newPosition;
        }

        template <class InputIterator>
        vector& assign(InputIterator first, InputIterator last) {
            return *this = vector(first, last);
        }

        vector& assign(size_type n, const_reference value) {
            return *this = vector(n, value);
        }

        vector& assign(std::initializer_list<T> ilist) {
            return *this = vector(ilist);
        }

        void clear() {
            erase(begin(), end());
        }


    private:
        // [start_, finish_) 范围的内存空间上有对象内容
        iterator start_;            // 目前使用空间的头
        iterator finish_;           // 目前使用空间的尾
        iterator end_of_storage_;   // 目前可用空间的尾

        Allocator dataAllocator;        // 内存空间配置器


        void fill_initialize(size_type n, const_reference x) {
            iterator result = dataAllocator.allocate(n); // 分配可以容纳 n 个 T 类型大小的内存空间
            // TODO tinySTL 版本的 uninitialized_fill_n
            std::uninitialized_fill_n(result, n, x); // 在内存空间上构造 n 个 T 类型的对象
            // 配置 vector<T> 的内存地址信息
            start_ = result;
            finish_ = start_ + n;
            end_of_storage_ = finish_;
        }

        template <class ForwardIterator>
        void copy_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
            auto copySize = static_cast<size_type >(distance(first, last)); // TODO 为什么不分配多一倍的空间
            start_ = dataAllocator.allocate(copySize);
            finish_ = std::uninitialized_copy(first, last, start_);
            end_of_storage_ = finish_;
        }

        template <class InputIterator>
        void copy_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
            while (first != last) {
                if (capacity() == size()) {
                    realloctae(empty() ? 1 : 2 * size());
                }
                push_back(*first++);
            }
        }

        void realloctae(size_type newSize) {
            vector tmp;
            tmp.start_ = dataAllocator.allocate(newSize);
            tmp.finish_ = std::uninitialized_copy(start_, finish_, tmp.start_);
            tmp.end_of_storage_ = tmp.start_ + newSize;

            swap(tmp);
        }

    };
}

#endif //TINYSTL_VECTOR_H
