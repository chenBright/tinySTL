#ifndef TINYSTL_STACK_H
#define TINYSTL_STACK_H

#include <algorithm>
#include <utility>

#include "deque.h"

namespace tinySTL {
    // 栈
    // 以某种容器作为底部数据结构，是底层容器的包装器。
    // stack 不是 container，是 container adapter。
    // stack 没有迭代器。
    template <class T, class Container = deque<T>>
    class stack {
        friend bool operator==<>(const stack &left, const stack &right);
        friend bool operator!=<>(const stack &left, const stack &right);
        friend bool operator< <>(const stack &left, const stack &right);
        friend bool operator<=<>(const stack &left, const stack &right);
        friend bool operator> <>(const stack &left, const stack &right);
        friend bool operator>=<>(const stack &left, const stack &right);

    public:
        using container_type    = Container;
        using value_type        = typename Container::value_type;
        using size_type         = typename Container::size_type;
        using reference         = typename Container::reference;
        using const_reference   = typename Container::const_reference;

    private:
        Container c;

    public:
        stack() = default;

        explicit stack(const Container &cont) : c(cont) {}

        explicit stack(const Container &&cont) : c(std::move(cont)) {}

        stack(const stack &other) : c(other.c) {}

        stack(stack &&other) noexcept : c(std::move(other.c)) {}

        ~stack() = default;

        stack& operator=(const stack &other) {
            if (this != &other) {
                c = other.c;
            }

            return *this;
        }

        stack& operator=(stack &&other) noexcept {
            if (this != &other)  {
                c = std::move(other.c);
            }

            return *this;
        }

        reference top() {
            return c.back();
        }

        const_reference top() const {
            return c.back();
        }

        bool empty() const {
            return c.empty();
        }

        size_type size() const {
            return c.size();
        }

        void push(const_reference value) {
            c.push_back(value);
        }

        void push(value_type &&value) {
            c.push_back(std::move(value));
        }

        template <class... Args>
        void emplace(Args&&... args) {
            c.emplace_back(std::forward(args)...);
        }

        void pop() {
            c.pop_back();
        }

        void swap(stack &other) {
            c.swap(other.c);
        }

    }; // class stack

    template <class T, class Container>
    bool operator==(const stack<T, Container> &left, const stack<T, Container> &right) {
        return left.c == right.c;
    }

    template <class T, class Container>
    bool operator!=(const stack<T, Container> &left, const stack<T, Container> &right) {
        return left.c != right.c;
    }

    template <class T, class Container>
    bool operator<(const stack<T, Container> &left, const stack<T, Container> &right) {
        return left.c < right.c;
    }

    template <class T, class Container>
    bool operator<=(const stack<T, Container> &left, const stack<T, Container> &right) {
        return left.c <= right.c;
    }

    template <class T, class Container>
    bool operator>(const stack<T, Container> &left, const stack<T, Container> &right) {
        return left.c > right.c;
    }

    template <class T, class Container>
    bool operator>=(const stack<T, Container> &left, const stack<T, Container> &right) {
        return left.c >= right.c;
    }

    template <class T, class Container>
    void swap(const stack<T, Container> &left, const stack<T, Container> &right) {
        left.swap(right);
    }
} // namespace tinySTL


#endif //TINYSTL_STACK_H
