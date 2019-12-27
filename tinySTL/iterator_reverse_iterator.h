#ifndef TINYSTL_ITERATOR_REVERSE_ITERATOR_H
#define TINYSTL_ITERATOR_REVERSE_ITERATOR_H

#include "iterator_base.h"

namespace tinySTL {
    // 反向迭代器
    // 除了 forward_list 之外，其他容器都支持反向迭代器。
    // 反向迭代器与普通迭代器之间的关系：
    // &*(reverse_iterator(current)) == &*(current - 1)，
    // 即普通迭代器 - 1 所指的位置即使反向迭代器所指的位置
    template <class Iterator>
    class reverse_iterator {
    public:
        using iterator_category = typename iterator_traits<Iterator>::iterator_category ;
        using value_type        = typename iterator_traits<Iterator>::value_type;
        using difference_type   = typename iterator_traits<Iterator>::difference_type;
        using pointer           = typename iterator_traits<Iterator>::pointer;
        using reference         = typename iterator_traits<Iterator>::reference;

        using iterator_type     = Iterator;

        reverse_iterator() : current_() {}
        explicit reverse_iterator(iterator_type current) : current_(current) {}
        template <class U>
        explicit reverse_iterator(const reverse_iterator<U> &other) : current_(other.base()) {}

        template <typename U>
        reverse_iterator& operator=(const reverse_iterator<U> &other) {
            current_ = other.base();
            return *this;
        }

        /**
         * 获取普通迭代器
         * @return 普通迭代器
         */
        iterator_type base() const {
            return current_;
        }

        // 重载 * ->

        reference operator*() {
            return *(current_ - 1);
        }

        pointer operator->() const {
            return &(operator*());
        }


        // 重载 ++ --

        reverse_iterator& operator++() {
            --current_;
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator tmp = *this;
            --current_;
            return tmp;
        }

        reverse_iterator& operator--() {
            ++current_;
            return *this;
        }

        reverse_iterator operator--(int) {
            reverse_iterator tmp = *this;
            ++current_;
            return tmp;
        }

        reverse_iterator operator+(difference_type n) const {
            return reverse_iterator(current_ - n);
        }

        reverse_iterator& operator+=(difference_type n) {
            current_ -= n;
            return *this;
        }

        reverse_iterator operator-(difference_type n) const {
            return reverse_iterator(current_ + n);
        }

        reverse_iterator& operator-=(difference_type n) {
            current_ += n;
            return *this;
        }

        reference operator[](difference_type n) const {
            return *operator+(n);
        }

    private:
        // &*(reverse_iterator(current)) == &*(current - 1)
        Iterator current_; // 普通迭代器
    };

    // 全局函数
    // 重载 == != > >= < <= + -

    template <class LeftIterator, class RightIterator>
    bool operator==(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() == right.base();
    }

    template <class LeftIterator, class RightIterator>
    bool operator!=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() != right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator>(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() > right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator>=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() >= right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator<(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() < right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator<=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() <= right.base();
    }

    template <class Iterator>
    reverse_iterator<Iterator> operator+(
            typename reverse_iterator<Iterator>::difference_type n,
            const reverse_iterator<Iterator> &it) {
        return it + n;
    }

    template <class Iterator>
    reverse_iterator<Iterator> operator-(
            typename reverse_iterator<Iterator>::difference_type n,
            const reverse_iterator<Iterator> &it) {
        return it - n;
    }

    /**
     * C++ 14 起
     * 将普通迭代器转换为反向迭代器
     * @tparam Iterator 普通迭代器类型
     * @param it 迭代器
     * @return 反向迭代器
     */
    template <class Iterator>
    reverse_iterator<Iterator> make_reverse_iterator(Iterator it) {
        return reverse_iterator<Iterator>(it);
    }
}

#endif //TINYSTL_ITERATOR_REVERSE_ITERATOR_H
