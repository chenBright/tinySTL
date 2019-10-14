#ifndef TINYSTL_MEMORY_H
#define TINYSTL_MEMORY_H

#include <utility>
#include <memory>

#include "utility_move.h"
#include "functional_base.h"


namespace tinySTL {
    template <class T> class shared_ptr;
    template <class T> class weak_ptr;

    // 接口功能见：https://zh.cppreference.com/w/cpp/memory/shared_ptr
    // 注意：只实现了部分功能。
    // 参考：
    // http://blog.leanote.com/post/shijiaxin.cn@gmail.com/C-shared_ptr
    // https://www.tamarous.com/shared_ptr/
    // http://www.calvinneo.com/2018/03/18/C++-smart-pointer/
    // https://www.zhihu.com/question/24816143
    // TODO shared_ptr、weak_ptr 中有一些泛型（即泛型类型为可转换为 T 的类型）构造函数和成员函数未实现。
    template <class T>
    class shared_ptr {
        friend class weak_ptr<T>;
    public:
        using element_type = T;

    private:
        size_t* count_;         // 引用计数
        size_t* weak_count_;    // 弱引用计数
        T* ptr_;                // 被管理的资源的指针

    public:
        shared_ptr() noexcept : count_(nullptr), weak_count_(nullptr), ptr_(nullptr) {}

        explicit shared_ptr(T* ptr) noexcept : count_(new size_t(1)), weak_count_(new size_t(0)), ptr_(ptr) {}

        shared_ptr(const shared_ptr& other) : count_(other.count_), weak_count_(new size_t(0)), ptr_(other.ptr_) {
            if (!other.is_empty()) {
                ++*count_;
            }
        }

        // 在 C++11 中，nullptr 是右值，所以 move(nullptr) 的值是 nullptr。
        shared_ptr(shared_ptr&& other) noexcept
            : count_(tinySTL::move(other.count_)),
              weak_count_(tinySTL::move(other.weak_count_)),
              ptr_(tinySTL::move(other.ptr_)) {

            other.count_ = nullptr;
            other.ptr_ = nullptr;
        }

        explicit shared_ptr(const weak_ptr<T>& wptr) : count_(wptr.count_), weak_count_(wptr.weak_count_), ptr_(wptr.ptr_) {
            if (!wptr.is_empty()) {
                ++*count_;
            }
        }

        ~shared_ptr() {
            clear();
        }

        shared_ptr& operator=(const shared_ptr& other) {
            if (this != &other) {
                clear();
                if (other.is_empty()) {
                    return *this;
                }

                count_ = other.count_;
                ptr_ = other.ptr_;

                ++*count_;

                return *this;
            }

            return *this;
        }

        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this != &other) {
                clear();
                count_ = tinySTL::move(other.count_);
                ptr_ = tinySTL::move(other.ptr_);
                other.count_ = nullptr;
                other.ptr_ = nullptr;
            }

            return *this;
        }

        void reset() noexcept {
            clear();
        }

        void swap(shared_ptr& other) noexcept {
            using std::move;
            swap(count_, other.count_);
            swap(ptr_, other.ptr_);
        }

        T* get() const noexcept {
            return ptr_;
        }

        T* operator->() {
            return ptr_;
        }

        T& operator*() {
            return *ptr_;
        }

        long use_count() const noexcept {
            return ptr_ == nullptr ? 0 : count_;
        }

        bool unique() const noexcept {
            return use_count() == 1;
        }

        explicit operator bool() const noexcept {
            return get() != nullptr;
        }

    private:
        // 判断管理的资源是否为空
        bool is_empty() const {
            return ptr_ == nullptr;
        }

        // 清理资源
        void clear() {
            if (is_empty()) {
                return;
            }

            if (--*count_ == 0) {
                delete ptr_;
            }

            if (*count_ == 0 && *weak_count_ == 0) {
                delete count_;
                delete weak_count_;
            }

            count_ = nullptr;
            weak_count_ = nullptr;
            ptr_ = nullptr;
        }
    }; // class shared_ptr

    template <class T, class U>
    bool operator==(const shared_ptr<T>& left, const shared_ptr<T>& right) noexcept {
        return left.get() == right.get();
    }

    template <class T, class U>
    bool operator!=(const shared_ptr<T>& left, const shared_ptr<T>& right) noexcept {
        return !(left == right);
    }

    template <class T, class U>
    bool operator<(const shared_ptr<T>& left, const shared_ptr<T>& right) noexcept {
        return left.get() < right.get();
    }

    template <class T, class U>
    bool operator<=(const shared_ptr<T>& left, const shared_ptr<T>& right) noexcept {
        return !(right < left);
    }

    template <class T, class U>
    bool operator>(const shared_ptr<T>& left, const shared_ptr<T>& right) noexcept {
        return right < left;
    }

    template <class T, class U>
    bool operator>=(const shared_ptr<T>& left, const shared_ptr<T>& right) noexcept {
        return !(right > left);
    }

    // 与 nullptr_t 比较

    template <class T>
    bool operator==(const shared_ptr<T>& left, std::nullptr_t) noexcept {
        return !left;
    }

    template <class T>
    bool operator==(std::nullptr_t, const shared_ptr<T>& right) noexcept {
        return !right;
    }

    template <class T>
    bool operator!=(const shared_ptr<T>& left, std::nullptr_t) noexcept {
        return static_cast<bool>(left);
    }

    template <class T>
    bool operator!=(std::nullptr_t, const shared_ptr<T>& right) noexcept {
        return static_cast<bool>(right);
    }

    template <class T>
    bool operator<(const shared_ptr<T>& left, std::nullptr_t) noexcept {
        return tinySTL::less<T*>(left, nullptr);
    }

    template <class T>
    bool operator<(std::nullptr_t, const shared_ptr<T>& right) noexcept {
        return tinySTL::less<T*>(nullptr, right);
    }

    template <class T, class U>
    bool operator<=(const shared_ptr<T>& left, std::nullptr_t) noexcept {
        return !(nullptr < left);
    }

    template <class T, class U>
    bool operator<=(std::nullptr_t, const shared_ptr<T>& right) noexcept {
        return !(right < nullptr);
    }

    template <class T, class U>
    bool operator>(const shared_ptr<T>& left, std::nullptr_t) noexcept {
        return nullptr < left;
    }

    template <class T, class U>
    bool operator>(std::nullptr_t, const shared_ptr<T>& right) noexcept {
        return right < nullptr;
    }

    template <class T, class U>
    bool operator>=(const shared_ptr<T>& left, std::nullptr_t) noexcept {
        return !(nullptr > left);
    }

    template <class T, class U>
    bool operator>=(std::nullptr_t, const shared_ptr<T>& right) noexcept {
        return !(right > nullptr);
    }

    template <class T>
    void swap(shared_ptr<T>& left, shared_ptr<T>& right) noexcept {
        left.swap(right);
    }

    template <class T>
    class weak_ptr {
        friend class shared_ptr<T>;
    public:
        using element_type = T;

    private:
        size_t* count_;         // 引用计数
        size_t* weak_count_;    // 弱引用计数
        T* ptr_;                // 被管理的资源的指针

    public:
        weak_ptr() noexcept : count_(nullptr), weak_count_(nullptr), ptr_(nullptr) {}

        weak_ptr(const weak_ptr& other) noexcept : count_(other.count_), weak_count_(other.weak_count_), ptr_(other.ptr_) {
            if (!is_empty()) {
                ++*weak_count_;
            }
        }

        explicit weak_ptr(const shared_ptr<T>& sptr) noexcept : count_(sptr.count_), weak_count_(sptr.weak_count_), ptr_(sptr.ptr_) {
            if (!is_empty()) {
                ++weak_count_;
            }
        }

        ~weak_ptr() = delete;

        weak_ptr& operator=(const weak_ptr& other) {
            count_ = other.count_;
            weak_count_ = other.weak_count_;
            ptr_ = other.ptr_;

            if (!is_empty()) {
                ++*weak_count_;
            }

            return *this;
        }

        weak_ptr& operator=(const shared_ptr<T>& sptr) {
            count_ = sptr.count_;
            weak_count_ = sptr.weak_count_;
            ptr_ = sptr.ptr_;
            if (!is_empty()) {
                ++weak_count_;
            }

            return *this;
        }

        void reset() {
            clear();
        }

        void swap(weak_ptr& other) noexcept {
            using tinySTL::swap;
            swap(count_, other.count_);
            swap(weak_count_, other.weak_count_);
            swap(ptr_, other.ptr_);
        }

        long use_count() const noexcept {
            return count_;
        }

        bool expired() const noexcept {
            return use_count() == 0;
        }

        shared_ptr<T> lock() const noexcept {
            return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
        }

    private:
        // 判断管理的资源是否为空
        bool is_empty() const {
            return ptr_ == nullptr;
        }

        // 清理资源
        void clear() {
            if (is_empty()) {
                return;
            }

            // 如果两个引用计数都为 0，则在 weak_ptr 中也可以销毁管理的资源。
            if (*count_ == 0 && --*weak_count_ == 0) {
                delete count_;
                delete weak_count_;
                delete ptr_;
            }

            count_ = nullptr;
            weak_count_ = nullptr;
            ptr_ = nullptr;
        }
    };

    template <class T>
    void swap(weak_ptr<T>& left, weak_ptr<T>& right) {
        left.swap(right);
    }
} // namespace tinySTL

#endif //TINYSTL_MEMORY_H
