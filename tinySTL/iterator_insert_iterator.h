#ifndef TINYSTL_ITERATOR_INSERT_ITERATOR_H
#define TINYSTL_ITERATOR_INSERT_ITERATOR_H

#include "iterator_base.h"
#include "utility_move.h"
#include <iterator>
#include <vector>

namespace tinySTL {

    // 接口功能见：https://zh.cppreference.com/w/cpp/iterator/back_insert_iterator
    template <class Container>
    class back_insert_iterator : public iterator_base<output_iterator_tag, void, void, void, void> {
    public:
        // 继承获得成员类型 iterator_category 、 value_type 、 difference_type 、 pointer 及 reference 。

        using container_type    = Container;

    private:
        container_type* container_;

    public:
        back_insert_iterator() noexcept = default; // C++ 20

        explicit back_insert_iterator(Container& c) : container_(&c) {}

        back_insert_iterator& operator=(const typename Container::value_type& value) {
            container_->push_back(value);
        }

        // 将 value 插入到迭代器。
        back_insert_iterator& operator=(typename Container::value_type&& value) {
            container_->push_back(tinySTL::move(value));
        }

        // 空操作，只需返回迭代器自身即可。
        // 为了保证迭代器接口的完备。

        back_insert_iterator& operator*() {
            return *this;
        }

        back_insert_iterator& operator++() {
            return *this;
        }

        back_insert_iterator& operator++(int) {
            return *this;
        }
    };

    // 接口功能见：https://zh.cppreference.com/w/cpp/iterator/front_insert_iterator
    template <class Container>
    class front_insert_iterator : public iterator_base<output_iterator_tag, void, void, void, void> {
    public:
        // 继承获得成员类型 iterator_category 、 value_type 、 difference_type 、 pointer 及 reference 。

        using container_type    = Container;

    private:
        container_type* container_;

    public:
        front_insert_iterator() noexcept = default; // C++ 20

        explicit front_insert_iterator(Container& c) : container_(&c) {}

        front_insert_iterator& operator=(const typename Container::value_type& value) {
            container_->push_front(value);
        }

        // 将 value 插入到迭代器。
        front_insert_iterator& operator=(typename Container::value_type&& value) {
            container_->push_front(tinySTL::move(value));
        }

        // 空操作，只需返回迭代器自身即可。
        // 为了保证迭代器接口的完备。

        front_insert_iterator& operator*() {
            return *this;
        }

        front_insert_iterator& operator++() {
            return *this;
        }

        front_insert_iterator& operator++(int) {
            return *this;
        }
    };
}

#endif //TINYSTL_ITERATOR_INSERT_ITERATOR_H
