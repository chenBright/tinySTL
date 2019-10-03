#ifndef TINYSTL_HASHTABLE_H
#define TINYSTL_HASHTABLE_H

#include "alloc.h"

namespace tinySTL {
    template <class T>
    struct hashtable_node {
        T data_;
        hashtable_node* next;
    };

    template <class Value, class Key, class HashFunction, class ExtractKey, class EqualKey,
              class Allocator = tinySTL::allocator<Value>>
    class hashtable {

    };

}

#endif //TINYSTL_HASHTABLE_H
