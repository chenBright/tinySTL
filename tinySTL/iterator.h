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
        return &array;
    }

    template< class C >
    constexpr auto cbegin(const C& c) noexcept
    -> decltype(tinySTL::begin(c)) {
        return tinySTL::begin(c);
    }

    template <class C>
    auto end(C& c) -> decltype(c.end()) {
        return c.end();
    }

    template <class C>
    auto end(const C& c) -> decltype(c.end()) {
        return c.end();
    }

    template< class T, std::size_t N >
    T* end(T (&array)[N]) {
        return array + N;
    }

    template< class C >
    constexpr auto cend(const C& c) noexcept
    -> decltype(tinySTL::end(c)) {
        return tinySTL::end(c);
    }

    template <class C>
    auto rbegin(C& c) -> decltype(c.rbegin()) {
        return c.rbegin();
    }

    template <class C>
    auto rbegin(const C& c) -> decltype(c.rbegin()) {
        return c.rbegin();
    }

    template< class T, std::size_t N >
    reverse_iterator<T*> rbegin(T (&array)[N]) {
        return reverse_iterator<T*>(&array);
    }

    template< class C >
    auto crbegin(const C& c) -> decltype(rbegin(c)) {
        return rbegin(c);
    }

    template <class C>
    auto rend(C& c) -> decltype(c.rend()) {
        return c.rend();
    }

    template <class C>
    auto rend(const C& c) -> decltype(c.rend()) {
        return c.rend();
    }

    template< class T, std::size_t N >
    reverse_iterator<T*> rend(T (&array)[N]) {
        return reverse_iterator<T*>(array + N);
    }

    template< class C >
    auto crend(const C& c) -> decltype(rend(c)) {
        return rend(c);
    }

}

#endif //TINYSTL_ITERATOR_H
