#ifndef TINYSTL_TYPE_TRAITS_H
#define TINYSTL_TYPE_TRAITS_H

#include <cstddef> // nullptr_t

namespace tinySTL {
    // integral_constant 是包装特定类型的静态常量。它是 C++ 类型特性的基类。
    template <class T, T v>
    struct integral_constant {
        using value_type    = T;
        using type          = integral_constant;

        static constexpr T value = v;

        // C++ 14
        constexpr explicit operator value_type() const noexcept {
            return value;
        }

        constexpr value_type operator()() const noexcept {
            return value;
        }
    };

    using true_type    = integral_constant<bool, true>;
    using false_type   = integral_constant<bool, false>;

    // is_same
    // 如果 T 与 U 指名同一类型（考虑 const/volatile 限定），则成员常量 value 为 true。
    // 否则 value 为 false 。
    template <class T, class U>
    struct is_same : public false_type {};

    template <class T>
    struct is_same<T, T> : public true_type {};

    template <class T, class U> constexpr bool is_same_v = is_same<T, U>::value; // C++ 17

    // remove_cv
    // remove_const
    // remove_volatil

    // 移除最顶层 const
    template <class T>
    struct remove_const {
        using type = T;
    };

    template <class T>
    struct remove_const<const T> {
        using type = T;
    };

    template <class T, class U> using remove_const_t = typename remove_const<T>::type; // C++ 14


    // 移除最顶层 volatile
    template <class T>
    struct remove_volatile {
        using type = T;
    };

    template <class T>
    struct remove_volatile<volatile T> {
        using type = T;
    };

    template <class T, class U> using remove_volatile_t = typename remove_volatile<T>::type; // C++ 14


    // 移除最顶层 const 、最顶层 volatile 或两者
    template <class T>
    struct remove_cv {
        using type = typename remove_volatile<typename remove_const<T>::type>::type;
    };

    template <class T, class U> using remove_cv_t = typename remove_cv<T>::type; // C++ 14

    /**
     * is_void
     * is_void_t
     */
    //
    template <class T>
    struct is_void : public is_same<void, typename remove_cv<T>::type> {};

    template <class T> bool is_void_v = is_void<T>::value; // C++ 17

    /**
     * is_null_pointer
     * is_null_pointer_v
     */
    // 判断 T 是否为 std::nullptr_t 类型。
    template <class T>
    struct is_null_pointer : public is_same<std::nullptr_t, typename remove_cv<T>::type> {};

    template <class T> bool is_null_pointer_v = is_null_pointer<T>::value;

    // is_integral 是否为整数
    // 作用：用于区分重载函数，例如 Vector<int> 有这样两个构造函数:
    // 1 vector(size_type n, const_reference value)，其中 const_reference = const int&
    // 2 template <class InputIterator> vector(InputIterator first, InputIterator last)
    // 如果 InputIterator 为整数类型，则此构造函数拥有的效果同
    // vector(static_cast<size_type>(first), static_cast<value_type>(last))。
    // 如果 InputIterator 为迭代器类型，才调用迭代器版本的重载函数。
    // 所以引入 is_integral，借助模板函数的类型推到功能，用来区分整数类型和迭代器类型。
    // 详细使用见 vector 实现。
    template <class T>
    struct is_integral                      : public false_type {};

    // 实例化类模板，只需要写 <>。
    // 参考《C++ Primer》第五版 P584
    template <>
    struct is_integral<bool>                : public true_type {};

    template <>
    struct is_integral<char>                : public true_type {};

    template <>
    struct is_integral<unsigned char >      : public true_type {};

    template <>
    struct is_integral<signed char>         : public true_type {};

    template <>
    struct is_integral<short>               : public true_type {};

    template <>
    struct is_integral<unsigned short>      : public true_type {};

    template <>
    struct is_integral<int>                 : public true_type {};

    template <>
    struct is_integral<unsigned int>        : public true_type {};

    template <>
    struct is_integral<long>                : public true_type {};

    template <>
    struct is_integral<unsigned long>       : public true_type {};

    template <>
    struct is_integral<long long>           : public true_type {};

    template <>
    struct is_integral<unsigned long long > : public true_type {};

    // is_floating_point
    // is_floating_point_v
    // 判断 T 是否为浮点类型。
    template <class T>
    struct is_floating_point : public integral_constant<bool,
                               is_same<float, typename remove_cv<T>::type>::value ||
                               is_same<double, typename remove_cv<T>::type>::value ||
                               is_same<long double , typename remove_cv<T>::type>::value> {};

    template <class T> bool is_floating_pointer_v = is_floating_point<T>::value; // C++ 17

    // is_array
    // is_array_v
    // 判断 T 是否数组类型。
    template <class T>
    struct is_array : public false_type {};

    template <class T>
    struct is_array<T[]> : public true_type {};

    template <class T, std::size_t N>
    struct is_array<T[N]> : public true_type {};

    template <class T> bool is_array_v = is_array<T>::value; // C++ 17

    // is_pointer
    // 判断 T 是否为指向对象指针或指向函数指针。
    template <class T>
    struct is_pointer_helper : public false_type {};

    template <class T>
    struct is_pointer_helper<T*> : public true_type {};

    template <class T>
    struct is_pointer : public is_pointer_helper<typename remove_cv<T>::type> {};

    // is_lvalue_reference
    // is_lvalue_reference_v
    // 判断 T 是否为左值引用类型。
    template <class T>
    struct is_lvalue_reference : public false_type {};

    template <class T>
    struct is_lvalue_reference<T&> : public true_type {};

    template <class T> bool is_lvalue_reference_v = is_lvalue_reference<T>::value; // C++ 17

    // is_rvalue_reference
    // is_rvalue_reference_v
    // 判断 T 是否为右值引用类型。
    template <class T>
    struct is_rvalue_reference : public false_type {};

    template <class T>
    struct is_rvalue_reference<T&&> : public true_type {};

    template <class T> bool is_rvalue_reference_v = is_rvalue_reference<T>::value; // C++ 17

    // is_reference
    // is_reference_v
    // 判断 T 是否为引用类型。
    template <class T>
    struct is_reference : public false_type {};

    template <class T>
    struct is_reference<T&> : public true_type {};

    template <class T>
    struct is_reference<T&&> : public true_type {};

    template <class T> bool is_reference_v = is_reference<T>::value; // C++ 17

    // is_const
    // is_const_v
    // 判断 T 为 const 限定类型（即 const 或 const volatile ）。
    template <class T>
    struct is_const : public false_type {};

    template <class T>
    struct is_const<const T> : public true_type {};

    template <class T> bool is_const_v = is_const<T>::value; // C++ 17

    // is_volatile
    // is_volatile_v
    // 判断 T 为 volatile 限定类型（即 volatile 或 const volatile ）。
    template <class T>
    struct is_volatile : public false_type {};

    template <class T>
    struct is_volatile<volatile T> : public true_type {};

    template <class T> bool is_volatile_v = is_volatile<T>::value; // C++ 17

    // remove_reference
    // 去除引用，得到类型 T
    template <class T>
    struct remove_reference {
        using type = T;
    };

    template <class T>
    struct remove_reference<T&> {
        using type = T;
    };

    template <class T>
    struct remove_reference<T&&> {
        using type = T;
    };

    template <class T> using remove_reference_t = typename remove_reference<T>::type; // C++ 14

    /**
     * add_lvalue_reference
     */
    // 获取 T 的左值引用类型。
    template <class T>
    struct add_lvalue_reference {
        using type = T&;
    };

    template <class T>
    struct add_lvalue_reference<T&> {
        using type = T&;
    };

    template <class T>
    struct add_lvalue_reference<T&&> {
        using type = T&;

    };

    /**
     * add_rvalue_reference
     */
    // 获取 T 的右值引用类型。
    template <class T>
    struct add_rvalue_reference {
        using type = T&&;
    };

    template <class T>
    struct add_rvalue_reference<T&> {
        using type = T&&;
    };

    template <class T>
    struct add_rvalue_reference<T&&> {
        using type = T&&;

    };

    template <class T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type; // C++ 14

    // remove_pointer
    // remove_pointer_t
    // 移除 T 的指针属性
    template <class T>
    struct remove_pointer {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T*> {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T* const> {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T* volatile> {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T* const volatile> {
        using type = T;
    };

    template <class T> using remove_pointer_t = typename remove_pointer<T>::type; // C++ 14



    // has_trivial_default_constructor  -> 不重要的构造函数
    // has_trivial_copy_constructor     -> 不重要的拷贝构造函数
    // has_trivial_assignment_operator  -> 不重要的赋值函数
    // has_trivial_destructor           -> 不重要的析构函数
    // is_POD_type                      -> Plain Old Data，标量类型或者传统的 C struct 类型。
    //                                     对 POD 类型，可采用最有效的初值填写方法。
    //
    // 决定以上成员为 true_type 还是 false_type 的标准：
    // 如果 class 内含指针成员，并且对它进行内存动态配置，
    // 则该 class 就需要实现自己的 non-trivial-xxx。所以以上成员为 false_type。
    /**
     * type traits
     * SGI 使用了保守的策略：将所有成员定义为 __false_type。
     *                     而针对 C++ 内置类型（如 int）设计了偏特化版本的 __type_traits。
     * @tparam type 内嵌类型（类）
     */
    template <class type>
    struct __type_traits {
        // 不要移除这个成员。它通知"有能力自动将 __type_traits 特化"的编译器，说现在所看到的这个
        // __type_traits template 是特化的。这是为了保证万一编译器也使用了一个名为 __type_traits template，
        // 但它与此处定义并无关联时，type traits 仍能顺利运作。
        using this_dummy_member_must_be_first = true_type;

        using has_trivial_default_constructor = false_type;
        using has_trivial_copy_constructor    = false_type;
        using has_trivial_assignment_operator = false_type;
        using has_trivial_destructor          = false_type;
        using is_POD_type                     = false_type;
    };

    template <>
    struct __type_traits<bool> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<char> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<signed char> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned char> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<short> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned short> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<int> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned int> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<long long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned long long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<float> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<double> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<long double> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    // 原生指针偏特化版本的 type traits
    template <class T>
    struct __type_traits<T*> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };


    template <class T>
    struct __type_traits<const T*> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };
}

#endif //TINYSTL_TYPE_TRAITS_H
