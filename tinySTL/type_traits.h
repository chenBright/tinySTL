#ifndef TINYSTL_TYPE_TRAITS_H
#define TINYSTL_TYPE_TRAITS_H

namespace tinySTL {

    struct __true_type {};
    struct __false_type {};

    //
    // has_trivial_default_constructor  -> 不重要的构造函数
    // has_trivial_copy_constructor     -> 不重要的拷贝构造函数
    // has_trivial_assignment_operator  -> 不重要的赋值函数
    // has_trivial_destructor           -> 不重要的析构函数
    // is_POD_type                      -> Plain Old Data，标量类型或者传统的 C struct 类型。
    //                                     对 POD 类型，课采用最有效的初值填写方法。
    //
    // 决定以上成员为 __true_type 还是 __false_type 的标准：
    // 如果 class 内含指针成员，并且对它进行内存动态配置，
    // 则该 class 就需要实现自己的 non-trivial-xxx。所以以上成员为 __false_type。

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
        using this_dummy_member_must_be_first = __true_type;

        using has_trivial_default_constructor = __false_type;
        using has_trivial_copy_constructor    = __false_type;
        using has_trivial_assignment_operator = __false_type;
        using has_trivial_destructor          = __false_type;
        using is_POD_type                     = __false_type;
    };

    template <>
    struct __type_traits<bool> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<char> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<signed char> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<unsigned char> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<short> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<unsigned short> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<int> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<unsigned int> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<long> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<unsigned long> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<long long> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<unsigned long long> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<float> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<double> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    template <>
    struct __type_traits<long double> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };

    // 原生指针偏特化版本的 type traits
    template <class T>
    struct __type_traits<T*> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };


    template <class T>
    struct __type_traits<const T*> {
        using has_trivial_default_constructor = __true_type;
        using has_trivial_copy_constructor    = __true_type;
        using has_trivial_assignment_operator = __true_type;
        using has_trivial_destructor          = __true_type;
        using is_POD_type                     = __true_type;
    };
}

#endif //TINYSTL_TYPE_TRAITS_H