#ifndef TINYSTL_ITERATOR_REVERSE_ITERATOR_H
#define TINYSTL_ITERATOR_REVERSE_ITERATOR_H

#include "iterator_base.h"

namespace tinySTL {
    template <class Iterator>
    class reverse_iterator {
    public:
        using iterator_category = typename iterator_traits<Iterator>::iterator_category ;
        using value_type        = typename iterator_traits<Iterator>::value_type;
        using difference_type   = typename iterator_traits<Iterator>::differebce_type;
        using pointer           = typename iterator_traits<Iterator>::pointer;
        using reference         = typename iterator_traits<Iterator>::reference;

        using iterator_type     = Iterator;

        constexpr reverse_iterator() : current_() {}
        constexpr explicit reverse_iterator(Iterator current) : current_(current) {}
        template <class U>
        constexpr explicit reverse_iterator(reverse_iterator<U> &other) : current_(other.base()) {}

        // TODO 学习 constexpr
        constexpr Iterator base() const {
            return current_;
        }

        constexpr reference operator*() const {
            return *(current_ - 1);
        }

        constexpr pointer operator->() const {
            return &(operator*());
        }

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
            --current_;
            return *this;
        }

        reverse_iterator operator--(int) {
            reverse_iterator tmp = *this;
            --current_;
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

        constexpr reference operator[](difference_type n) const {
            return *operator+(n);
        }

        template <typename U>
        reverse_iterator& operator=(const reverse_iterator<U> &other) {
            current_ = other.base();
            return *this;
        }

    private:
        // &*(reverse_iterator(i)) == &*(current - 1)
        Iterator current_;
    };

    template <class LeftIterator, class RightIterator>
    constexpr bool operator==(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() == right.base();
    }

    template <class LeftIterator, class RightIterator>
    constexpr bool operator!=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() != right.base();
    }
    template <class LeftIterator, class RightIterator>
    constexpr bool operator>(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() > right.base();
    }
    template <class LeftIterator, class RightIterator>
    constexpr bool operator>=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() >= right.base();
    }
    template <class LeftIterator, class RightIterator>
    constexpr bool operator<(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() < right.base();
    }
    template <class LeftIterator, class RightIterator>
    constexpr bool operator<=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() <= right.base();
    }

    template <class Iterator>
    constexpr reverse_iterator<Iterator> operator+(
            typename reverse_iterator<Iterator>::difference_type n,
            const reverse_iterator<Iterator> &it) {
        return it + n;
    }

    template <class Iterator>
    constexpr reverse_iterator<Iterator> operator-(
            typename reverse_iterator<Iterator>::difference_type n,
            const reverse_iterator<Iterator> &it) {
        return it - n;
    }

    /**
     * 将普通迭代器转换为反向迭代器
     * @tparam Iterator 普通迭代器类型
     * @param it 迭代器
     * @return 反向迭代器
     */
    template <class Iterator>
    constexpr reverse_iterator<Iterator> make_reverse_iterator(Iterator it) {
        return reverse_iterator<Iterator>(it);
    }
}

#endif //TINYSTL_ITERATOR_REVERSE_ITERATOR_H
