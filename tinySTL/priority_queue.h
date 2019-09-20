#ifndef TINYSTL_PRIORITY_QUEUE_H
#define TINYSTL_PRIORITY_QUEUE_H

#include <functional>

#include "vector.h"
#include "algorithm_heap.h"

namespace tinySTL {
    template <class T, class Container = vector<T>, class Compare = std::less<typename Container::value_type>>
    class priority_queue {
    public:
        using container_type    = Container;
        using value_type        = typename Container::value_type;
        using size_type         = typename Container::size_type;
        using reference         = typename Container::reference;
        using const_reference   = typename Container::const_reference;

    private:
        Container c_;
        Compare comp_;

    public:
        priority_queue() : c_() {}

        explicit priority_queue(const Compare &compare) : c_(), comp_(compare) {}

        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last) : c_(first, last) {
            make_heap(c_.begin(), c_.end(), comp_);
        }

        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last, const Compare &compare) : c_(first, last), comp_(compare) {
            make_heap(c_.begib(), c_.end(), comp_);
        }

        bool empty() const {
            return c_.empty();
        }

        size_type size() const {
            return c_.size();
        }

        const_reference top() const {
            return c_.front();
        }

        void push(const value_type &value) {
            c_.push_back(value);
            push_heap(c_.begin(), c_.end(), comp_);
        }

        void pop() {
            pop_heap(c_.begin(), c_.end(), comp_);
            c_.pop_back();
        }
    };
}

#endif //TINYSTL_PRIORITY_QUEUE_H
