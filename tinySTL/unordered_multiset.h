#ifndef TINYSTL_UNORDERED_MULTISET_H
#define TINYSTL_UNORDERED_MULTISET_H

#include <initializer_list>
#include <algorithm>

#include "functional.h"
#include "functional_hash.h"
#include "hashtable.h"
#include "alloc.h"

namespace tinySTL {

    template <class Key, class Hash = tinySTL::hash<Key>, class KeyEqual = tinySTL::equal_to<Key>,
            class Allocator = tinySTL::allocator<Key>>
    class unordered_multiset {

    };
}


#endif //TINYSTL_UNORDERED_MULTISET_H
