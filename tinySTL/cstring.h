#ifndef TINYSTL_CSTRING_H
#define TINYSTL_CSTRING_H

#include <cstddef>
#include <cstring>

namespace tinySTL {
    /**
     * strcpy
     */
    // 复制 src 所指向的字符串，包含空终止符（'\0'），到首元素为 dest 所指向的字符数组。
    // 返回原 dest 指针。
    // 若 dest 数组不够大则行为未定义。
    // 若字符串重叠则行为未定义。
    char* strcpy(char* dest, const char* src) {
        if (dest == nullptr || src == nullptr || dest == const_cast<char*>(src)) {
            return dest;
        }

        char* return_str = dest;
        while ((*dest++ = *src++) != '\0');

        return return_str;
    }

    /**
     * strncpy
     */
    // 复制 src 所指向的字节字符串的 count 个字符（包含终止空字符）到 dest 所指向的字符数组。
    // 如果已经复制了 count 个字符，但未达到 src 的尾部，即未遍历到 src 的空终止符（'\0'），则不会为 dest 添加 '\0'。
    // 如果复制到了 src 的空终止符（'\0'），但未达到 count 个字符，则写空字符到 dest，直到总共写入 count 个字符位置。
    // 若字符串重叠，则行为未定义。
    // 参考 https://zhuanlan.zhihu.com/p/37383750
    char* strncpy(char* dest, const char* src, std::size_t count) {
        if (dest == nullptr || src == nullptr || dest == const_cast<char*>(src)) {
            return dest;
        }

        char* return_str = dest;
        while (count-- > 0) {
            // 如果 strlen(src) < count，则后面会写入 count - strlen(src) 个 '\0'，
            // 但不会进入循环，以防止访问 src 时导致缓冲区溢出。
            while ((*dest++ = *src) != '\0') {
                ++src;
            }
        }

        return return_str;
    }

    /**
     * strcat
     */
    // 将 src 连接到 dest 后面。
    // dest 所指的内存空间要能容纳得下 strlen(src) + strlen(dest) + 1 个字符。
    // 若字符串重叠，则行为未定义。
    char* strcat(char* dest, const char* src) {
        char* return_str = dest;
        dest += strlen(dest);
        while ((*dest = *src) != '\0');

        return return_str;
    }

    /**
     * strncat
     */
    // 将 src 前 count 个字符连接到 dest 后面。
    // 如果 count > strlen(src)，则只拷贝 strlen(src) 个字符。
    // 若字符串重叠，则行为未定义。
    char* strncat(char* dest, const char* src, std::size_t count) {
        char* return_str = dest;
        while (count-- != 0 && *src != '\0') {
            *dest++ = *src++;
        }
        *dest = '\0';

        return return_str;
    }

    /**
     * strxfrm
     */
    // 从 src 拷贝前 count 个字符到 dest 中，并返回 src 的长度。
    // 如果 count > strlen(src)，则只拷贝 strlen(src) 个字符。
    // 若 dest 数组不够大则行为未定义。
    // 若字符串重叠，则行为未定义。
    // 若 count 为 ​0​ ，则允许 dest 为空指针。此时，strxfrm 函数的作用相当于 strlen 函数。
    std::size_t strxfrm(char* dest, const char* src, std::size_t count) {
        strncpy(dest, src, count);

        return strlen(src);
    }

    /**
     * strlen
     */
    // 返回给定字节字符串的长度，即首元素为 str 所指向的字符数组直到而不包含首个空字符的字符数。
    // 若 str 所指向的字符数组中无空字符，则行为未定义。
    std::size_t strlen(const char* str) {
        std::size_t length = 0;
        while (*str++ != '\0') {
            ++length;
        }

        return length;
    }

    /**
     * strcmp
     */
    // 按字典序比较两个字符串。
    int strcmp(const char* lhs, const char* rhs) {
        if (lhs == rhs) {
            return 0;
        }

        while (true) {
            if (*lhs != *rhs) {
                return *lhs < *rhs ? -1 :1;
            }

            if (*lhs == '\0') {
                return 0;
            }

            ++lhs;
            ++rhs;
        }
    }

    /**
     * strncmp
     */
    // 按字典序比较两个字符串的前 count 个字符。
    int strncmp(const char* lhs, const char* rhs, std::size_t count) {
        if (lhs == rhs) {
            return 0;
        }

        while (true) {
            if (count-- == 0) {
                return 0;
            }

            if (*lhs != *rhs) {
                return *lhs < *rhs ? -1 : 1;
            }

            ++lhs;
            --rhs;
        }
    }

    /**
     * strchr
     */
    // 在 str 字符串中寻找字符 static_cast<char>(ch) 首次出现的位置。
    // 如果没有找到或者 static_cast<char>(ch) == '\0'，则返回 NULL。
    const char* strchr(const char* str, int ch) {
        auto c = static_cast<char>(ch);
        while (*str != '\0' && *str != c) {
            ++str;
        }

        return *str == c ? str : nullptr;
    }

    char* strchr(char* str, int ch) {
        auto c = static_cast<char>(ch);
        while (*str != '\0' && *str != c) {
            ++str;
        }

        return *str == c ? str : nullptr;
    }

    /**
     * memcpy
     */
    // 从 src 所指向的对象复制 count 个字符到 dest 所指向的对象。
    // 两个对象都被转换成 unsigned char 的数组。
    // src 和 dest 所指内存区域不能重叠，否则行为未定义。
    // src 或 dest 为空指针则行为未定义，即使 count 为零。
    // 与 strcpy 相比，memcpy 遇到 ‘\0’ 并不会结束，而是一定会拷贝完 count 个字节。
    // std::memcpy 理论上是最快的内存到内存复制子程序。通常它比必须扫描其所复制数据的 strcpy ，或必须预防并处理重叠输入的 std::memmove 更高效。
    // 许多 C++ 编译器将适当的内存复制循环变换为 std::memcpy 调用。
    void* memcpy(void* dest, const void* src, std::size_t count) {
        if (dest == nullptr || src == nullptr || dest == const_cast<void*>(src)) {
            return dest;
        }

        auto newDest = static_cast<unsigned char*>(dest);
        auto newSrc = static_cast<const unsigned char*>(src);
        while (count-- != 0) {
            *newDest++ = *newSrc++;
        }

        return dest;
    }
}

#endif //TINYSTL_CSTRING_H
