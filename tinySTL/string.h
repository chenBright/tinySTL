#ifndef TINYSTL_STRING_H
#define TINYSTL_STRING_H

#include <iostream>
#include <stdexcept>

#include "utility_pair.h"
#include "algorithm_base.h"
#include "cstring.h"


namespace tinySTL {

    // 引用计数基类
    class RCObject {
        friend class String;

    private:
        int refCount_;
        bool shareable_;

    public:
        // refCount初始化为 0，其值完全有 RCPtr 控制。
        RCObject() : refCount_(0), shareable_(true) {}

        RCObject(const RCObject&) : RCObject() {}

        // 使 RCObject 成为抽象基类，但该纯虚函数需要提供定义，
        // 不然会使被继承的类无法在栈上创建（原因可
        // 查阅如何仅在堆上或栈上分配内存）
        virtual ~RCObject() = 0;

        RCObject& operator=(const RCObject&) = delete;

    public:
        void addReference() {
            ++refCount_;
        }

        void removeReference() {
            if (--refCount_ == 0) {
                delete this;
            }
        }

        size_t getRefCount() const {
            return refCount_;
        }

        void markUnshareable() {
            shareable_ = false;
        }

        bool isShareable() const {
            return shareable_;
        }

        bool isShared() const {
            return refCount_ > 1;
        }
    };

    // 引用计数智能指针
    // T 必须支持RCObject接口，因此 T 通常继承于RCObject。
    template<class T>
    class RCPtr {
    private:
        T* ptr_;

    public:
        explicit RCPtr(T* realPtr = nullptr) : ptr_(realPtr) {
            init();
        }

        RCPtr(const RCPtr& other) : ptr_(other.ptr_) {}

        RCPtr& operator=(const RCPtr& other) {
            if (ptr_ != other.ptr_) {
                if (ptr_ != nullptr) {
                    ptr_->removeReference();
                }
                ptr_ = other.ptr_;
                init();
            }

            return *this;
        }

        ~RCPtr() {
            if (ptr_ != nullptr) {
                ptr_->removeReference();
            }
        }

    public:
        T* operator->() const {
            return ptr_;
        }

        T& operator*() const {
            return *ptr_;
        }

    private:
        void init() {
            if (ptr_ == nullptr) {
                return;
            }

            if (!ptr_->isSharabled()) {
                // 如果其值不可共享，那么就赋值一份。
                // 注意，这里将新对象的赋值行为，
                // 由之前的 string 转移到RCPtr，
                // 此时 T（在原有的String实现中是StringValue）
                // 若要完成深拷贝，T（StringValue）必须自定义拷贝构造函数。
                ptr_ = new T(*ptr_);
            }
            ptr_->addReference();
        }
    };

    class string {
        friend std::ostream& operator<<(std::ostream& os, const string& str);

        friend std::istream& operator>>(std::istream& is, string&);

    private:
        // 内部数据类
        struct string_value : public RCObject {
            char* ptr_{};
            size_t size_;

            explicit string_value(const char* str = "") : size_(strlen(str)) {
                init(str);
            }

            string_value(const string_value& other) : string_value(other.ptr_) {}

            ~string_value() override {
                // Clang-Tidy: 'if' statement is unnecessary; deleting null pointer has no effect
                delete[] ptr_;
            }

        private:
            void init(const char* str) {
                ptr_ = new char[size_ + 1];
                strcpy(ptr_, str);
            }
        };

        RCPtr<string_value> value_;

    public:
        explicit string(const char* str)
            : value_(new string_value(str)) {
        }

        string(const string& str) {
            if (str.value_->isShareable()) {
                value_ = str.value_;
                value_->addReference();
            } else {
                value_ = new string_value(str.value_->ptr_);
            }
        }

        ~string() {
            // Clang-Tidy: 'if' statement is unnecessary; deleting null pointer has no effect
            value_->removeReference();
        }

        string& operator=(const string& str) {
            if (str.value_ != value_) {
                value_->removeReference();

                if (str.value_->isShareable()) {
                    value_ = str.value_;
                    value_->addReference();
                } else {
                    value_ = new string_value(str.value_->ptr_);
                }
            }

            return *this;
        }

        string& operator=(const char* str) {
            string tmp(str);
            swap(tmp);

            return *this;
        }

        size_t getRefCount() const {
            return value_->getRefCount();
        }

        inline size_t size() const {
            return value_->size_;
        }

        inline const char* c_str() const {
            return value_->ptr_;
        }

        inline void swap(string& str) noexcept {
            using tinySTL::swap;
            swap(value_->ptr_, str.value_->ptr_);
            swap(value_->size_, str.value_->size_);

        }

        string& operator += (const string& str) {
            return operator=(str.c_str());
        }

        string& operator += (const char* str) {
            if (str != nullptr && *str != '\0') {
                // 异常安全
                size_t len = value_->size_ + ::strlen(str) + 1;
                char* tmp = new char[len];
                strcpy(tmp, value_->ptr_);
                strcpy(tmp + value_->size_, str);

                using tinySTL::swap;
                swap(value_->ptr_, tmp);
                value_->size_ = len;
                delete[] tmp;
            }

            return *this;
        }

        const char& operator[](size_t index) const {
            if (index >= value_->size_) {
                throw std::out_of_range("String : out of index!");
            }

            return value_->ptr_[index];
        }

        char& operator[](size_t index) {
            // 调用const版本的operator[]，降低代码重复。

            if (index >= value_->size_) {
                throw std::out_of_range("String : out of index!");
            }

            if (value_->isShared()) {
                value_->removeReference();
                value_ = new string_value(value_->ptr_);
            }

            return value_->ptr_[index];
        }
    };

    std::ostream& operator<<(std::ostream& os, const string& str) {
        os << str.value_->ptr_;
        return os;
    }

    std::istream& operator>>(std::istream& is, string& str) {

    }

    bool operator<(const string& left, const string& right) {
        return lexicographical_compare(left.c_str(),
                                 left.c_str() + left.size(),
                                       right.c_str(),
                                       right.c_str() + right.size());
    }

    bool operator<=(const string& left, const string& right) {
        return !(right < left);
    }

    bool operator>(const string& left, const string& right) {
        return right < left;
    }



    bool operator>=(const string& left, const string& right) {
        return !(left < right);
    }

    bool operator==(const string& left, const string& right) {
        if (left.size() != right.size()) {
            return false;
        }

        for (int i = 0; i < left.size(); ++i) {
            if (left[i] != right[i]) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const string& left, const string& right) {
        return !(left == right);
    }

    string operator+(const string& left, const string& right) {
        string tmp(left);
        tmp += right;

        return tmp;
    }

    void swap(string& left, string& right) {
        left.swap(right);
    }

}

#endif //TINYSTL_STRING_H
