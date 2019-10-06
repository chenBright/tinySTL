#ifndef TINYSTL_PRIORITY_QUEUE_H
#define TINYSTL_PRIORITY_QUEUE_H

#include <functional>
#include <utility>

#include "vector.h"
#include "algorithm_heap.h"
#include "functional.h"
#include "utility_move.h"

namespace tinySTL {
    template <class T, class Container = vector<T>, class Compare = tinySTL::less<typename Container::value_type>>
    class priority_queue {
    public:
        using container_type    = Container;
        using value_type        = typename Container::value_type;
        using size_type         = typename Container::size_type;
        using reference         = typename Container::reference;
        using const_reference   = typename Container::const_reference;

    private:
        Compare comp_;
        Container c_;

    public:
        priority_queue() = default;

        explicit priority_queue(const Compare &compare) : comp_(compare) {}

        priority_queue(const Compare &compare, const Container &cont) : comp_(compare), c_(cont) {
            make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(const Compare &compare, const Container &&cont) : comp_(compare), c_(tinySTL::move(cont)) {
            make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(const priority_queue &other) : comp_(other.comp_), c_(other.c_) {}

        // comp_ 是比较函数对象，可以 move
        priority_queue(priority_queue &&other) noexcept : comp_(tinySTL::move(other.comp_)), c_(tinySTL::move(other.c_))  {}

        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last) : c_(first, last) {
            make_heap(c_.begin(), c_.end(), comp_);
        }

        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last, const Compare &compare) : c_(first, last), comp_(compare) {
            make_heap(c_.begib(), c_.end(), comp_);
        }

        ~priority_queue() = default;

        priority_queue& operator=(const priority_queue &other) {
            if (this != &other) {
                comp_ = other.comp_;
                c_ = other.c_;
            }

            return *this;
        }

        priority_queue& operator=(priority_queue &&other) noexcept {
            if (this != &other) {
                comp_ = tinySTL::move(other.comp_);
                c_ = tinySTL::move(other.c_);
            }

            return *this;
        }

        const_reference top() const {
            return c_.front();
        }

        bool empty() const {
            return c_.empty();
        }

        size_type size() const {
            return c_.size();
        }

        void push(const value_type &value) {
            c_.push_back(value);
            push_heap(c_.begin(), c_.end(), comp_);
        }

        void push(value_type &&value) {
            c_.push_back(tinySTL::move(value));
            push_heap(c_.begin(), c_.end(), comp_);
        }

        template <class... Args>
        void emplace(Args... args) {
            c_.push_back(std::forward<Args>(args)...);
            push_heap(c_.begin(), c_.end(), comp_);
        }

        void pop() {
            pop_heap(c_.begin(), c_.end(), comp_);
            c_.pop_back();
        }

        void swap(priority_queue &other) {
            using std::swap;
            swap(comp_, other.comp_);
            swap(c_, other.c_); // 实际调用 c_ 的 swap 函数，原因见《C++ Promer》P706
        }
    };

    template <class T, class Container, class Compare>
    void swap(priority_queue<T, Container, Compare> &left, priority_queue<T, Container, Compare> &right) {
        left.swap(right);
    }

}

#endif //TINYSTL_PRIORITY_QUEUE_H
