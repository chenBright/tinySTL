#ifndef TINYSTL_MEMORY_PTR_H
#define TINYSTL_MEMORY_PTR_H

#include <utility>
#include <memory>
#include <functional>
//#include <stdatomic.h> gcc 原子操作头文件
#include <atomic>

#include "utility_move.h"
#include "functional_base.h"
#include "algorithm_base.h"

namespace tinySTL {

    // 定制化删除器
    template <class T>
    class PtrDeleter {
    public:
        using DeleteFunction = std::function<void(T*)>;

    private:
        DeleteFunction func_;

    public:
        PtrDeleter() : func_(std::bind(&PtrDeleter::defaultFunction, this, std::placeholders::_1)) {}

        explicit PtrDeleter(const DeleteFunction& df) : func_(df) {}

        void operator()(T* ptr) {
            func_(ptr);
        }

    private:
        void defaultFunction(T* ptr) {
            ptr->~T();
        }
    };

    // 定制化删除器
    template <class T>
    class PtrDeleter<T[]> {
    public:
        using DeleteFunction = std::function<void(T*)>;

    private:
        DeleteFunction func_;

    public:
        PtrDeleter() : func_(std::bind(&PtrDeleter::defaultFunction, this, std::placeholders::_1)) {}

        explicit PtrDeleter(const DeleteFunction& df) : func_(df) {}

        void operator()(T* ptr) {
            func_(ptr);
        }

    private:
        void defaultFunction(T* ptr) {
            delete [] ptr;
        }
    };


    template <class T, class Deleter> class shared_ptr;
    template <class T, class Deleter> class weak_ptr;

    // 接口功能见：https://zh.cppreference.com/w/cpp/memory/shared_ptr
    // 注意：只实现了部分功能。
    // 参考：
    // http://blog.leanote.com/post/shijiaxin.cn@gmail.com/C-shared_ptr
    // https://www.tamarous.com/shared_ptr/
    // http://www.calvinneo.com/2018/03/18/C++-smart-pointer/
    // https://www.zhihu.com/question/24816143
    // https://www.cnblogs.com/joeylee97/p/8671136.html （有删除器的实现）
    // TODO shared_ptr、weak_ptr 中有一些泛型（即泛型类型为可转换为 T 的类型）构造函数和成员函数未实现。
    template <class T, class Deleter = PtrDeleter<T>>
    class shared_ptr {
        friend class weak_ptr<T, Deleter>;
    public:
        using element_type = T;

    private:
        std::atomic_long* count_;       // 引用计数
        std::atomic_long* weak_count_;  // 弱引用计数
        T* ptr_;                        // 被管理的资源的指针
        Deleter deleter_;               // 删除器

    public:
        shared_ptr() noexcept : count_(nullptr), weak_count_(nullptr), ptr_(nullptr) {}

        explicit shared_ptr(T* ptr) noexcept
            : count_(new std::atomic_long(1)),
              weak_count_(new std::atomic_long(0)),
              ptr_(ptr) {

        }

        // nullptr 的重载函数
        explicit shared_ptr(std::nullptr_t) : shared_ptr() {}

        shared_ptr(const shared_ptr& other)
            : count_(other.count_), // 对于内建类型，不需要调用 move 函数
              weak_count_(other.weak_count_),
              ptr_(other.ptr_),
              deleter_(other.deleter_) {

            if (!other.is_empty()) {
                ++*count_;
            }
        }

        // 在 C++11 中，nullptr 是右值，所以 move(nullptr) 的值是 nullptr。
        shared_ptr(shared_ptr&& other) noexcept
            : count_(other.count_),
              weak_count_(other.weak_count_),
              ptr_(other.ptr_),
              deleter_(tinySTL::move(other.deleter_)) {

            other.count_ = nullptr;
            other.weak_count_ = nullptr;
            other.ptr_ = nullptr;
        }

        explicit shared_ptr(const weak_ptr<T, Deleter>& wptr)
            : count_(wptr.count_),
              weak_count_(wptr.weak_count_),
              ptr_(wptr.ptr_),
              deleter_(wptr.deleter_) {

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

                count_ = other.count_;
                weak_count_ = other.weak_count_;
                ptr_ = other.ptr_;
                deleter_ = other.deleter_;

                ++*count_;

                return *this;
            }

            return *this;
        }

        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this != &other) {
                clear();

                count_ = other.count_;
                weak_count_ = other.weak_count_;
                ptr_ = other.ptr_;
                deleter_ = tinySTL::move(other.deleter_);

                other.count_ = nullptr;
                other.weak_count_ = nullptr;
                other.ptr_ = nullptr;
                deleter_ = nullptr;
            }

            return *this;
        }

        void reset() noexcept {
            clear();
        }

        void swap(shared_ptr& other) noexcept {
            using tinySTL::swap;

            swap(count_, other.count_);
            swap(weak_count_, other.weak_count_);
            swap(ptr_, other.ptr_);
            swap(deleter_, other.deleter_);
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
                deleter_(ptr_);
            }

            // 只有当两个引用计数都为 0 时，才能 delete 引用计数。
            // 当 *count_ == 0 而 *weak_ptr_ != 0 时，
            // 表示虽然资源已经释放了，但还有 weak_ptr 存在，
            // 可能还需要用到 count_ 和 weak_count_。
            if (*count_ == 0 && *weak_count_ == 0) {
                delete count_;
                delete weak_count_;
            }

            count_ = nullptr;
            weak_count_ = nullptr;
            ptr_ = nullptr;
        }
    }; // class shared_ptr

    template <class T1, class D1, class T2, class D2>
    bool operator==(const shared_ptr<T1, D1>& left, const shared_ptr<T2, D2>& right) noexcept {
        return left.get() == right.get();
    }

    template <class T1, class D1, class T2, class D2>
    bool operator!=(const shared_ptr<T1, D1>& left, const shared_ptr<T2, D2>& right) noexcept {
        return !(left == right);
    }

    template <class T1, class D1, class T2, class D2>
    bool operator<(const shared_ptr<T1, D1>& left, const shared_ptr<T2, D2>& right) noexcept {
        return left.get() < right.get();
    }

    template <class T1, class D1, class T2, class D2>
    bool operator<=(const shared_ptr<T1, D1>& left, const shared_ptr<T2, D2>& right) noexcept {
        return !(right < left);
    }

    template <class T1, class D1, class T2, class D2>
    bool operator>(const shared_ptr<T1, D1>& left, const shared_ptr<T2, D2>& right) noexcept {
        return right < left;
    }

    template <class T1, class D1, class T2, class D2>
    bool operator>=(const shared_ptr<T1, D1>& left, const shared_ptr<T2, D2>& right) noexcept {
        return !(right > left);
    }

    // 与 nullptr_t 比较

    template <class T, class D>
    bool operator==(const shared_ptr<T, D>& left, std::nullptr_t) noexcept {
        return !left;
    }

    template <class T, class D>
    bool operator==(std::nullptr_t, const shared_ptr<T, D>& right) noexcept {
        return !right;
    }

    template <class T, class D>
    bool operator!=(const shared_ptr<T, D>& left, std::nullptr_t) noexcept {
        return static_cast<bool>(left);
    }

    template <class T, class D>
    bool operator!=(std::nullptr_t, const shared_ptr<T, D>& right) noexcept {
        return static_cast<bool>(right);
    }

    template <class T, class D>
    bool operator<(const shared_ptr<T, D>& left, std::nullptr_t) noexcept {
        return tinySTL::less<T*>(left, nullptr);
    }

    template <class T, class D>
    bool operator<(std::nullptr_t, const shared_ptr<T, D>& right) noexcept {
        return tinySTL::less<T*>(nullptr, right);
    }

    template <class T, class D>
    bool operator<=(const shared_ptr<T, D>& left, std::nullptr_t) noexcept {
        return !(nullptr < left);
    }

    template <class T, class D>
    bool operator<=(std::nullptr_t, const shared_ptr<T, D>& right) noexcept {
        return !(right < nullptr);
    }

    template <class T, class D>
    bool operator>(const shared_ptr<T, D>& left, std::nullptr_t) noexcept {
        return nullptr < left;
    }

    template <class T, class D>
    bool operator>(std::nullptr_t, const shared_ptr<T, D>& right) noexcept {
        return right < nullptr;
    }

    template <class T, class D>
    bool operator>=(const shared_ptr<T, D>& left, std::nullptr_t) noexcept {
        return !(nullptr > left);
    }

    template <class T, class D>
    bool operator>=(std::nullptr_t, const shared_ptr<T, D>& right) noexcept {
        return !(right > nullptr);
    }

    template <class T, class D>
    void swap(shared_ptr<T, D>& left, shared_ptr<T, D>& right) noexcept {
        left.swap(right);
    }

    template <class T, class Deleter = PtrDeleter<T>>
    class weak_ptr {
        friend class shared_ptr<T, Deleter>;
    public:
        using element_type = T;

    private:
        std::atomic_int* count_;        // 引用计数
        std::atomic_int* weak_count_;   // 弱引用计数
        T* ptr_;                        // 被管理的资源的指针
        Deleter deleter_;               // 删除器

    public:
        weak_ptr() noexcept
            : count_(nullptr),
              weak_count_(nullptr),
              ptr_(nullptr) {

        }

        weak_ptr(const weak_ptr& other) noexcept
            : count_(other.count_),
              weak_count_(other.weak_count_),
              ptr_(other.ptr_) {

            if (!is_empty()) {
                ++*weak_count_;
            }
        }

        explicit weak_ptr(const shared_ptr<T>& sptr) noexcept
            : count_(sptr.count_),
              weak_count_(sptr.weak_count_),
              ptr_(sptr.ptr_) {

            if (!is_empty()) {
                ++weak_count_;
            }
        }

        // other 移动后，other == nullptr 且 other.use_count == 0。
        weak_ptr(weak_ptr&& other) noexcept
            : count_(other.count_),
              weak_count_(other.weak_count_),
              ptr_(other.ptr_),
              deleter_(tinySTL::move(other.deleter_)) {

            other.count_ = nullptr;
            other.weak_count_ = nullptr;
            other.ptr_ = nullptr;
            deleter_ = nullptr;
        }

        ~weak_ptr() = delete;

        weak_ptr& operator=(const weak_ptr& other) {
            if (this != &other) {
                clear();

                count_ = other.count_;
                weak_count_ = other.weak_count_;
                ptr_ = other.ptr_;
                deleter_ = other.deleter_;

                if (!is_empty()) {
                    ++*weak_count_;
                }
            }

            return *this;
        }

        weak_ptr& operator=(const shared_ptr<T>& sptr) {
            clear();

            count_ = sptr.count_;
            weak_count_ = sptr.weak_count_;
            ptr_ = sptr.ptr_;
            deleter_ = sptr.deleter_;

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
            swap(deleter_, other.deleter_);
        }

        long use_count() const noexcept {
            if (ptr_ == nullptr) {
                return 0;
            }

            return *count_;
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
                // 如果 *count_ == 0，则资源已经被 shared_ptr 释放了，不能重复释放。
//                deleter_(ptr_);
            }

            count_ = nullptr;
            weak_count_ = nullptr;
            ptr_ = nullptr;
        }
    }; // class weak_ptr

    template <class T, class D>
    void swap(weak_ptr<T, D>& left, weak_ptr<T, D>& right) {
        left.swap(right);
    }

    // 参考
    // https://blog.csdn.net/weixin_41442027/article/details/105609088
    // https://my.oschina.net/u/4396841/blog/3429379/print
    // https://www.cnblogs.com/codingmengmeng/p/9123874.html
    // https://www.cnblogs.com/zhanggaofeng/p/10294507.html
    // http://hahaya.github.io/use-enable-shared-from-this
    template <class T>
    class enable_shared_from_this {
    public:
        template <class T1, class T2>
        friend void do_enable(shared_ptr<T1> ptr, enable_shared_from_this<T2>* esftPtr);
    private:
        // 在第一次创建 shared_ptr 时，weakPtr_ 才指向某个资源。
        mutable weak_ptr<T> weakPtr_;

    public:
        enable_shared_from_this() noexcept = default;

        enable_shared_from_this(const enable_shared_from_this&) noexcept = default;

        ~enable_shared_from_this() = default;

        enable_shared_from_this& operator=(const enable_shared_from_this&) noexcept = default;

        shared_ptr<T> shared_fromt_this() {
            return shared_ptr<T>(weakPtr_);
        }

        shared_ptr<T const> shared_fromt_this() const {
            return shared_ptr<const T>(weakPtr_);
        }
    };

    // 使用了 SFINAE 技术，即匹配失败不是错误，在编译时期来确定某个 type 是否具有我们需要的性质。
    // 如果 esftPtr 的类型是一个继承自 enable_shared_from_this 的类型，则会匹配第一个版本的函数。
    // 否则，匹配第二个版本的函数，什么都不用做。
    template <class T1, class T2>
    void do_enable(shared_ptr<T1> sPtr, enable_shared_from_this<T2>* esftPtr) {
        // 如果 weakPtr_ 为指向资源，即第一次创建 shared_ptr，则令其指向 sPtr 的资源。
        if (esftPtr->weakPtr_.expired()) {
            esftPtr->weakPtr_ = sPtr;
        }
    }

    template <class T1, class T2>
    void do_enable(...) {}


    template <class T, class Deleter = PtrDeleter<T>>
    class unique_ptr {
    public:
        using pointer = T*;
        using element_type = T;

    private:
        T* ptr_;            // 被管理的资源的指针
        Deleter deleter_;   // 删除器

    public:
        unique_ptr() noexcept : ptr_(nullptr) {}

        explicit unique_ptr(std::nullptr_t) noexcept : ptr_(nullptr) {}

        explicit unique_ptr(pointer ptr) : ptr_(ptr) {}

        unique_ptr(unique_ptr&& other) noexcept
            : ptr_(other.ptr_),
              deleter_(tinySTL::move(other.deleter_)) {
            other.ptr_ = nullptr;
        }

        ~unique_ptr() {
            if (!is_empty()) {
                deleter_(ptr_);
            }
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept {
            reset(other.release());
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }

        unique_ptr& operator=(std::nullptr_t) noexcept {
            ptr_ = nullptr;
        }

        pointer release() noexcept {
            auto tmp = ptr_;
            ptr_ = nullptr;

            return tmp;
        }

        void reset(pointer ptr = pointer()) noexcept {
            if (!is_empty()) {
                deleter_(ptr_);
            }

            ptr_ = ptr;
        }

        void swap(unique_ptr& other) {
            using tinySTL::swap;

            swap(ptr_, other.ptr_);
            swap(deleter_, other.deleter_);
        }

        pointer get() const noexcept {
            return ptr_;
        }

        explicit operator bool() const noexcept {
            return !is_empty();
        }

        T& operator*() const {
            return *ptr_;
        }

        pointer operator->() const noexcept {
            return ptr_;
        }

    private:
        // 禁止拷贝
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        bool is_empty() const {
            return ptr_ == nullptr;
        }
    }; // class unique_ptr

    template <class T1, class D1, class T2, class D2>
    bool operator==(const unique_ptr<T1, D2>& left, const unique_ptr<T2, D2>& right) noexcept {
        return left.get() == right.get();
    }

    template <class T1, class D1, class T2, class D2>
    bool operator!=(const unique_ptr<T1, D2>& left, const unique_ptr<T2, D2>& right) noexcept {
        return !(left == right);
    }

    template <class T1, class D1, class T2, class D2>
    bool operator<(const unique_ptr<T1, D2>& left, const unique_ptr<T2, D2>& right) noexcept {
        return left.get() < right.get();
    }

    template <class T1, class D1, class T2, class D2>
    bool operator<=(const unique_ptr<T1, D2>& left, const unique_ptr<T2, D2>& right) noexcept {
        return !(right < left);
    }

    template <class T1, class D1, class T2, class D2>
    bool operator>(const unique_ptr<T1, D2>& left, const unique_ptr<T2, D2>& right) noexcept {
        return right < left;
    }

    template <class T1, class D1, class T2, class D2>
    bool operator>=(const unique_ptr<T1, D2>& left, const unique_ptr<T2, D2>& right) noexcept {
        return !(right > left);
    }

    // 与 nullptr_t 比较

    template <class T, class D>
    bool operator==(const unique_ptr<T, D>& left, std::nullptr_t) noexcept {
        return !left;
    }

    template <class T, class D>
    bool operator==(std::nullptr_t, const unique_ptr<T, D>& right) noexcept {
        return !right;
    }

    template <class T, class D>
    bool operator!=(const unique_ptr<T, D>& left, std::nullptr_t) noexcept {
        return static_cast<bool>(left);
    }

    template <class T, class D>
    bool operator!=(std::nullptr_t, const unique_ptr<T, D>& right) noexcept {
        return static_cast<bool>(right);
    }

    template <class T, class D>
    bool operator<(const unique_ptr<T, D>& left, std::nullptr_t) noexcept {
        return tinySTL::less<T*>(left, nullptr);
    }

    template <class T, class D>
    bool operator<(std::nullptr_t, const unique_ptr<T, D>& right) noexcept {
        return tinySTL::less<T*>(nullptr, right);
    }

    template <class T, class D>
    bool operator<=(const unique_ptr<T, D>& left, std::nullptr_t) noexcept {
        return !(nullptr < left);
    }

    template <class T, class D>
    bool operator<=(std::nullptr_t, const unique_ptr<T, D>& right) noexcept {
        return !(right < nullptr);
    }

    template <class T, class D>
    bool operator>(const unique_ptr<T, D>& left, std::nullptr_t) noexcept {
        return nullptr < left;
    }

    template <class T, class D>
    bool operator>(std::nullptr_t, const unique_ptr<T, D>& right) noexcept {
        return right < nullptr;
    }

    template <class T, class D>
    bool operator>=(const unique_ptr<T, D>& left, std::nullptr_t) noexcept {
        return !(nullptr > left);
    }

    template <class T, class D>
    bool operator>=(std::nullptr_t, const unique_ptr<T, D>& right) noexcept {
        return !(right > nullptr);
    }

    template <class T, class D>
    void swap(const unique_ptr<T, D>& left, const unique_ptr<T, D>& right) {
        left.swap(right);
    }

    // T 数组的特化版本
    template <class T>
    class unique_ptr<T[]> {
    public:
        using pointer = T*;
        using element_type = T;

    private:
        T* ptr_;

    public:
        unique_ptr() noexcept : ptr_(nullptr) {}

        explicit unique_ptr(std::nullptr_t) noexcept : ptr_(nullptr) {}

        explicit unique_ptr(pointer ptr) : ptr_(ptr) {}

        unique_ptr(unique_ptr&& other) noexcept : ptr_(other.ptr_) {}

        ~unique_ptr() {
            if (!is_empty()) {
                delete [] ptr_;
            }
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept {
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }

        unique_ptr& operator=(std::nullptr_t) noexcept {
            ptr_ = nullptr;
        }

        void reset(pointer ptr = pointer()) noexcept {
            if (!is_empty()) {
                delete ptr_;
            }

            ptr_ = ptr;
        }

        pointer release() noexcept {
            auto tmp = ptr_;
            ptr_ = nullptr;

            return tmp;
        }

        void reset(std::nullptr_t) noexcept {
            if (!is_empty()) {
                delete ptr_;
                ptr_ = nullptr;
            }
        }

        void swap(unique_ptr& other) {
            using tinySTL::swap;
            swap(ptr_, other.ptr_);
        }

        pointer get() const noexcept {
            return ptr_;
        }

        explicit operator bool() const noexcept {
            return !is_empty();
        }

        T& operator*() const {
            return *ptr_;
        }

        pointer operator->() const noexcept {
            return ptr_;
        }

        T& operator[](size_t i) const {
            return ptr_[i];
        }


    private:
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        bool is_empty() const {
            return ptr_ == nullptr;
        }
    }; // class unique_ptr<T[]>

    template <class T>
    void swap(unique_ptr<T[]>& left, unique_ptr<T[]>& right) {
        left.swap(right);
    }


} // namespace tinySTL

#endif //TINYSTL_MEMORY_PTR_H
