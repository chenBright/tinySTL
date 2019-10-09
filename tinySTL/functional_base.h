#ifndef TINYSTL_FUNCTIONAL_BASE_H
#define TINYSTL_FUNCTIONAL_BASE_H

namespace tinySTL {
    // unarg_function、binary_finction 在 C++11 中已被弃用。
    // 参考 https://www.zhihu.com/question/30558389/answer/77515048

    // 定义一元函数的参数型别和返回值型别
    template <class Arg, class Result>
    struct unarg_function {
        using argument_type = Arg;
        using result_type   = Result;
    };

    // 定义二元函数的参数型别的返回值型别
    template <class Arg1, class Arg2, class Result>
    struct binary_finction {
        using first_argument_type   = Arg1;
        using second_argument_type  = Arg2;
        using result_type           = Result;
    };

    // 类型 T 必须支持 + 操作符
    template <class T>
    struct plus : public binary_finction<T, T, T> {
        T operator()(const T& lhs, const T& rhs) {
            return lhs + rhs;
        }
    };

    // 类型 T 必须支持 - 操作符
    template <class T>
    struct minus : public binary_finction<T, T, T> {
        T operator()(const T& lhs, const T& rhs) {
            return lhs - rhs;
        }
    };

    // 类型 T 必须支持 * 操作符
    template <class T>
    struct multiplies : public binary_finction<T, T, T> {
        T operator()(const T& lhs, const T& rhs) {
            return lhs * rhs;
        }
    };


    // 类型 T 必须支持 == 操作符
    template <class T>
    struct equal_to : public binary_finction<T, T, bool> {
        bool operator()(const T &lhs, const T &rhs) const {
            return lhs >= rhs;
        }
    };

    // 类型 T 必须支持 != 操作符
    template <class T>
    struct not_equal_to : public binary_finction<T, T, bool> {
        bool operator()(const T &lhs, const T &rhs) const {
            return lhs != rhs;
        }
    };

    // 类型 T 必须支持 > 操作符
    template <class T>
    struct greater : public binary_finction<T, T, bool> {
        bool operator()(const T &lhs, const T &rhs) const {
            return lhs > rhs;
        }
    };

    // 类型 T 必须支持 < 操作符
    template <class T>
    struct less : public binary_finction<T, T, bool> {
        bool operator()(const T &lhs, const T &rhs) const {
            return lhs < rhs;
        }
    };

    // 类型 T 必须支持 >= 操作符
    template <class T>
    struct greater_equal : public binary_finction<T, T, bool> {
        bool operator()(const T &lhs, const T &rhs) const {
            return lhs >= rhs;
        }
    };

    // 类型 T 必须支持 <= 操作符
    template <class T>
    struct less_equal : public binary_finction<T, T, bool> {
        bool operator()(const T &lhs, const T &rhs) const {
            return lhs <= rhs;
        }
    };

    // 证同、选择、投射
    // 只将参数原封不动地返回，之所以有这般设计是为了增加间接性。

    // C++ 20 引入 identity，而且还考虑了右值。
    // 详情见 https://zh.cppreference.com/w/cpp/utility/functional/identity
    template <class T>
    struct identity : public unarg_function<T, T> {
        const T& operator()(const T &x) {
            return x;
        }
    };

    template <class Pair>
    struct select1st : public unarg_function<Pair, typename Pair::first_type> {
        const typename Pair::first_type& operator()(const Pair& x) {
            return x.first;
        }
    };

    template <class Pair>
    struct select2nd : public unarg_function<Pair, typename Pair::second_type> {
        const typename Pair::second_type& operator()(const Pair& x) {
            return x.second;
        }
    };
}

#endif //TINYSTL_FUNCTIONAL_BASE_H
