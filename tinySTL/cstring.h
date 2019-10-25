#ifndef TINYSTL_CSTRING_H
#define TINYSTL_CSTRING_H

#include <cstddef>
#include <cstring>

namespace tinySTL {
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
        if (dest == const_cast<void*>(src)) {
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
