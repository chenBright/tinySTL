#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H

#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "iterator_insert_iterator.h"

namespace tinySTL {

    template <class Container>
    back_insert_iterator<Container> back_inserter(Container& c) {
        return back_insert_iterator<Container>(c);
    }

}

#endif //TINYSTL_ITERATOR_H
