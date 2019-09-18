#ifndef TINYSTL_QUEUE_H
#define TINYSTL_QUEUE_H

#include "deque.h"

namespace tinySTL {
    template <class T, class Container = deque<T>>
    class queue {
        friend bool operator==<>(const queue &left, const queue &right);
        friend bool operator!=<>(const queue &left, const queue &right);
        friend bool operator< <>(const queue &left, const queue &right);
        friend bool operator<=<>(const queue &left, const queue &right);
        friend bool operator> <>(const queue &left, const queue &right);
        friend bool operator>=<>(const queue &left, const queue &right);

    public:
        using container_type    = Container;
        using value_type        = typename Container::value_type;
        using size_type         = typename Container::size_type;
        using reference         = typename Container::reference;
        using const_reference   = typename Container::const_reference;

    private:
        Container c;

    public:
        queue() = default;

        bool empty() const {
            return c.empty();
        }

        size_type size() const {
            return c.size();
        }

        reference front() {
            return c.front();
        }

        const_reference front() const {
            return c.front();
        }

        reference back() {
            return c.back();
        }

        const_reference back() const {
            return c.back();
        }

        void push(const value_type &value) {
            return c.push(value);
        }

        void pop() {
            c.pop();
        }
    }; // class queue

    template <typename T, typename Container>
    bool operator==(const queue<T, Container> &left, const queue<T, Container> &right) {
        return left.c == right.c;
    }

    template <typename T, typename Container>
    bool operator!=(const queue<T, Container> &left, const queue<T, Container> &right) {
        return left.c != right.c;
    }

    template <typename T, typename Container>
    bool operator<(const queue<T, Container> &left, const queue<T, Container> &right) {
        return left.c < right.c;
    }

    template <typename T, typename Container>
    bool operato<=(const queue<T, Container> &left, const queue<T, Container> &right) {
        return left.c <= right.c;
    }

    template <typename T, typename Container>
    bool operator>(const queue<T, Container> &left, const queue<T, Container> &right) {
        return left.c > right.c;
    }

    template <typename T, typename Container>
    bool operator>=(const queue<T, Container> &left, const queue<T, Container> &right) {
        return left.c >= right.c;
    }
} // namespace tinySTL

#endif //TINYSTL_QUEUE_H
