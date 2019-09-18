#ifndef TINYSTL_QUEUE_H
#define TINYSTL_QUEUE_H

#include <algorithm>
#include <utility>

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

        explicit queue(const Container &cont) : c(cont) {}

        explicit queue(Container &&cont) : c(std::move(cont)) {}

        queue(const queue &other) : c(other.c) {}

        queue(queue &&other) noexcept : c(std::move(other.c)) {}

        queue& operator=(const queue &other) {
            if (this != &other) {
                c = other.c;
            }

            return *this;
        }

        queue& operator=(queue &&other) noexcept {
            if (this != &other) {
                c = std::move(other.c);
            }

            return *this;
        }

        reference front() {
            return c.front();
        }

        const_reference front() const  {
            return c.front();
        }

        reference back() {
            return c.back();
        }

        const_reference back() const {
            return c.back();
        }

        bool empty() const {
            return c.empty();
        }

        size_type size() const {
            return c.size();
        }

        void push(const value_type &value) {
            c.push(value);
        }

        void push(value_type &&value) {
            c.push(std::move(value));
        }

        template <class... Args>
        void emplace(Args... args) {
            c.emplace_back(std::forward<Args>(args)...);
        }

        void pop() {
            c.pop();
        }

        void swap(queue &other) noexcept {
            c.swap(other);
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
    bool operator<=(const queue<T, Container> &left, const queue<T, Container> &right) {
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

    template <typename T, typename Container>
    void swap(const queue<T, Container> &left, const queue<T, Container> &right) {
        left.swap(right);
    }
} // namespace tinySTL

#endif //TINYSTL_QUEUE_H
