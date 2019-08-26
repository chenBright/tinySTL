#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H

#include <cstddef>
#include <form.h>
#include <tclDecls.h>

namespace tinySTL {
    // 迭代器类型
    // Input Iterator  ---
    //                    |---> Forward Iterator ---> Bidirectional Iterator ---> Random Access Iterator
    // Output Iterator ---
    // 迭代器之间的 concept（概念）与 refinement（强化）关系
    //
    // 所指对象为只读
    struct input_iterator_tag {};
    // 所指对象为只写
    struct output_iterator_tag {};
    // 允许"写入型"算法对迭代器形成的区间上进行读写操作（只能向前移动）
    struct forward_iterator_tag : public input_iterator_tag {};
    // 同上（能双向移动）
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    // 允许随机访问
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    /**
     * 迭代器基类
     * 每个新设计的迭代器都继承自它，就能保证符合 STL 的规范，以免漏写属性。
     *
     * 设计适当的对应类型，是迭代器的职责。
     * 设计适当的迭代器，是容器的职责。因为只用容器才知道设计怎么的迭代器来遍历自己。
     *
     * @tparam Category 迭代器类型
     * @tparam T 迭代器所指对象的类型
     * @tparam Distance 迭代器之间的距离的类型
     * @tparam Pointer 迭代器所指对象的指针
     * @tparam Reference 迭代器所指对象的引用
     */
    template <class Category,
              class T,
              class Distance = std::ptrdiff_t,
              class Pointer = T*,
              class Reference = T&>
    struct iterator_base {
        using iterator_category = Category;
        using value_type        = T;
        using differebce_type   = Distance;
        using pointer           = Pointer;
        using reference         = Reference;
    };


    // iterator_traits 和基本的 iterator_traits 函数

    /**
     * iterator traits
     * @tparam Iterator 迭代器
     */
    template <class Iterator>
    struct iterator_traits {
        using iterator_category = typename Iterator::iterator_category; // 迭代器类型
        using value_type        = typename Iterator::value_type;        // 迭代器所指的对象的类型
        using difference_type   = typename Iterator::differebce_type;   // 两个迭代器之间的距离
        using pointer           = typename Iterator::pointer;           // 迭代器所指的对象的指针
        using reference         = typename Iterator::reference;         // 迭代器所指的对象的引用
    };

    // 原生指针不是类，不能为他们定义内嵌类型（如 Iterator::value_type）。
    // 所以，针对 temlplate 参数是指针的情况，设计了特化版本的 iterator traits。

    // pointer 偏特化版本 iterator traits
    template <class T>
    struct iterator_traits<T*> {
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t ;
        using pointer           = T*;
        using reference         = T&;
    };

    // const pointer 偏特化版本 iterator traits
    template <class T>
    struct iterator_traits<const T*> {
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t ;
        using pointer           = T*;
        using reference         = T&;
    };

    /**
     * 获取迭代器类型
     * @tparam Itertor 迭代器
     * @return 迭代器类型对象
     */
    template <class Itertor>
    inline typename iterator_traits<Itertor>::iterator_category iterator_category(const Itertor&) {
        using category = typename iterator_traits<Itertor>::iterator_category;
        return category(); // 迭代器类型是一个类，此处返回一个对象
    }

    /**
     * 获取迭代器的 difference type
     * @tparam Iterator 迭代器
     * @return difference type 指针
     */
    template <class Iterator>
    inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
        using differenceType = typename iterator_traits<Iterator>::difference_type;
        return static_cast<differenceType*>(0);
    }

    /**
     * 获取迭代器的对象类型
     * @tparam Iterator 迭代器
     * @return 对象指针
     */
    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type value_type(const Iterator&) {
        using valueType = typename iterator_traits<Iterator>::value_type;
        return static_cast<valueType*>(0);
    }


    // advance 函数及其各种偏特化版本的函数

    namespace detail {
        // advance 的偏特化版本

        template <class InputIterator, class Distance>
        inline void __advance(InputIterator &it, Distance n, input_iterator_tag) {
            while (n-- > 0) {
                ++it;
            }
        }

        template <class BidirectionalIterator, class Distance>
        inline void __advance(BidirectionalIterator &it, Distance n, bidirectional_iterator_tag) {
            if (n > 0) {
                while (n-- > 0) {
                    ++it;
                }
            } else{
                while (n++ < 0) {
                    --it;
                }
            }
        }

        template <class RandomAccessIterator, class Distance>
        inline void __advance(RandomAccessIterator &it, Distance n, random_access_iterator_tag) {
            it += n;
        }
    } // namespace detail

    /**
     * 移动迭代器
     * @tparam InputIterator 迭代器类型。Input Iterator 和它的"强化"迭代器都是合法的参数。
     *                       STL 算法的一个命名规则：迭代器类型的参数的命名为算法所能接受的最低阶迭代器类型。
     * @tparam Distance difference type
     * @param it 迭代器
     * @param n 距离
     */
    template <class InputIterator, class Distance>
    inline void advance(InputIterator &it, Distance n) {
        detail::__advance(it, n, iterator_category(it));
    }


    // distance 函数及其各种偏特化版本的函数

    namespace detail {
        // distance 的偏特化版本

        template <class InputIterator>
        typename iterator_traits<InputIterator>::difference_type
        __distance(InputIterator first, InputIterator last, input_iterator_tag) {
            typename iterator_traits<InputIterator>::difference_type n = 0;
            while (first != last) {
                ++n;
                ++first;
            }

            return n;
        }

        template <class RandomAccessIterator>
        typename iterator_traits<RandomAccessIterator>::difference_type
        __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
            return last - first;
        }
    } // namespace detail

    /**
     * 两个迭代器的距离
     * @tparam InputIterator 迭代器类型。Input Iterator 和它的"强化"迭代器都是合法的参数。
     * @param first 迭代器
     * @param last 迭代器
     * @return 距离
     */
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        return detail::__distance(first, last, iterator_category(first));
    }

    /**
     * 当前迭代器向前移动 n 步后的迭代器
     * @tparam InputIterator 迭代器类型。Input Iterator 和它的"强化"迭代器都是合法的参数。
     * @param it 迭代器
     * @param n 距离
     * @return 移动后的迭代器
     */
    template <class InputIterator>
    inline InputIterator next(InputIterator it,
            typename iterator_traits<InputIterator>::difference_type n = 1) {
        advance(it, 1);

        return it;
    }

    /**
     * 当前迭代器向后移动 n 步后的迭代器
     * @tparam BidirectionalIterator 迭代器类型。Bidirectional Iterator 和它的"强化"迭代器都是合法的参数。
     * @param it 迭代器
     * @param n 距离（非负数）
     * @return 移动后的迭代器
     */
    template <class BidirectionalIterator>
    inline BidirectionalIterator prev(BidirectionalIterator it,
            typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
        advance(it, -n);

        return it;
    }
}

#endif //TINYSTL_ITERATOR_H
