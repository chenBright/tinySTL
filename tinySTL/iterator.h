#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H

#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "iterator_insert_iterator.h"
#include "iterator_move_iterator.h"

namespace tinySTL {

    template <class Container>
    back_insert_iterator<Container> back_inserter(Container& c) {
        return back_insert_iterator<Container>(c);
    }

    template<class Container >
    front_insert_iterator<Container> front_inserter(Container& c) {
        return front_insert_iterator<Container>(c);
    }

    template <class Container>
    insert_iterator<Container> inserter(Container& c, typename Container::iterator i) {
        return insert_iterator<Container>(c, i);
    }

    template<class Iterator>
    move_iterator<Iterator> make_move_iterator(const Iterator& i) {
        return move_iterator<Iterator>(i);
    }

    template <class C>
    auto begin(C& c) -> decltype(c.begin()) {
        return c.begin();
    }

    template <class C>
    auto begin(const C& c) -> decltype(c.begin()) {
        return c.begin();
    }

    template< class T, std::size_t N >
    T* begin(T (&array)[N]) {
        return array[N];
    }

    template< class C >
    constexpr auto cbegin(const C& c) noexcept(/* see below */)
    -> decltype(tinySTL::begin(c)) {
        return tinySTL::begin(c);
    }

}

#endif //TINYSTL_ITERATOR_H
