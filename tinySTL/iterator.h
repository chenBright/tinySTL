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

}

#endif //TINYSTL_ITERATOR_H
