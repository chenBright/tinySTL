#ifndef TINYSTL_ALLOCATE_H
#define TINYSTL_ALLOCATE_H

#include <cstddef> // size_t
#include <cstdlib> // malloc、realloc、free
#include <cstring> // memcpy

#include <algorithm>
#include <functional>
#include <new>

namespace tinySTL {
    // SGI STL Allocator Design
    // http://decsai.ugr.es/mgsilvente/stl/alloc.html

    // 第一级内存空间配置器
    // 直接调用 malloc()、free()、realloc() 对内存空间进行分配、释放、重新分配操作
    class mallocAlloc {
    public:
        // 分配内存失败时的自定义处理函数的类型
        typedef std::function<void()> OutOfMemoryHandler;

        /**
         * 分配 bytes 个字节的内存空间
         * @param bytes 字节数
         * @return 分配到的内存空间的指针
         */
        static void* allocate(size_t bytes) {
            void *result = ::malloc(bytes);
            if (result == nullptr) {
                // malloc() 分配内存失败，则调用内存不足处理程序
                result = outOfMemoryInMalloc(bytes);
            }

            return result;
        }

        /**
         * 释放 ptr 所指向的内存空间
         * @param ptr 指针
         */
        static void deallocate(void *ptr) {
            ::free(ptr);
        }

        /**
         * 重新分配内存空间
         * @param ptr 指向原内存空间的指针
         * @param bytes 重新分配后的内存空间的字节数
         * @return 指向新内存空间的指针
         */
        static void* reallocate(void *ptr, size_t bytes) {
            void *result = ::realloc(ptr, bytes);
            if (result == nullptr) {
                // realloc() 分配内存失败，则调用内存不足处理程序
                result = outOfMemoryInRealloc(ptr, bytes);
            }

            return result;
        }

    private:
        // 分配内存失败时的自定义处理函数
        static OutOfMemoryHandler handler;

        // 以上两个函数分别为 malloc()、realloc()调用失败时，提供处理功能。
        // 这两个函数的逻辑都是：
        // 无限循环，直到分配到需要的内存位置。
        // 同时，如果用户设置了自定义的处理函数（OutOfMemoryHandler），则还要调用 OutOfMemoryHandler()。
        // 如果用户没有设置自定义处理函数，则直接抛出异常（throw bad_alloc）。
        // TODO 找出第三版中的 item，且设计"内存不足"的处理函数是用户的职责，该 item 有提到该函数的特定模式
        // 该实现类似《Effective C++》第二版 item7 中提到的 C++ new-handler 机制。
        // C++ new-handler 机制：
        // 要求系统在无法满足内存分配要求时，调用一个自定义的处理函数。

        /**
         * 调用 malloc() 失败后，使用该函数处理
         * @param bytes 字节数
         * @return 分配到的内存空间的指针
         */
        static void* outOfMemoryInMalloc(size_t bytes) {
            while (true) {
                if (handler == nullptr) {
                    throw std::bad_alloc(); // bad_alloc() 在 new 头文件中定义了
                }

                handler();
                void* result = ::malloc(bytes);
                if (result != nullptr) {
                    return result;
                }
            }
        }

        /**
         * 调用 malloc() 失败后，使用该函数处理
         * @param ptr 旧内存空间的指针
         * @param bytes 重新分配的字节数
         * @return 新分配的内存空间的指针
         */
        static void* outOfMemoryInRealloc(void *ptr, size_t bytes) {
            while (true) {
                if (handler == nullptr) {
                    throw std::bad_alloc();
                }

                handler();
                void* result = ::realloc(ptr, bytes);
                if (result != nullptr) {
                    return result;
                }
            }
        }
    };

    // 第二级内存空间配置器包含两部分：
    // 1. 第一级内存空间配置器；
    // 2. 内存池。
    //
    // SGI 中内存操作的设计哲学：
    // 1. 向 system heap 请求内存空间；
    // 2. 考虑多线程状态；
    // 3. 考虑内存不足时的应变措施；
    // 4. 考虑过多"小型内存块"可能造成的内存碎片问题。
    //
    // SGI 的策略是：
    // 1. 如果处理的内存块足够大，超过了 128 bytes 时，则使用第一级内存空间配置器处理；
    // 2. 如果处理的内存块小于 128 bytes 时，由内存池管理。
    //
    // 内存池分配和管理的方法，又称为 sub-allocation：
    // 每次分配一块大的内存空间内存池，并维护一个内存块链表数组。
    //
    // 申请内存空间时，则从对应的内存块链表中取出相应大小的内存块。
    // 释放内存空间时，则将内存块插入到对应的内存块链表头部。
    //
    // 内存块链表数组维护了16个内存块链表，它们分别管理的内存块的大小（单位为 bytes）分别为
    // 8，16，24，32，40，48，56，64，72，80，88，96，104，112，120，128。
    // 内存池会将所有的内存需求量上调到 8 的倍数。
    // 例如，申请 30 bytes 内存，实际得到的内存空间大小为 32 bytes，从第4个链表中分配得到。
    class alloc {
    public:
        /**
         * 申请 bytes 字节内存空间
         * @param bytes 字节数
         * @return 申请到的内存空间的指针
         */
        static void* allocate(size_t bytes) {
            if (bytes > static_cast<size_t>(MAX_BYTES)) {
                // 需求量大于 128 bytes，则使用第一级内存空间配置器
                return mallocAlloc::allocate(bytes);
            }

            // 需求量小于 128 bytes，使用内存池
            size_t index = freeListIndex(bytes); // 找到对应的链表
            obj *blockList = freeLists[index];
            if (blockList != nullptr) {
                // 链表不为空，则将链表头结点更新为原第二个结点，
                // 原头结点作为结果返回，即分配给用户。
                freeLists[index] = blockList->nextBlock;
                return blockList;
            } else {
                // 链表为空，
                return refill(roundUp(bytes));
            }
        }


        /**
         * "释放" ptr 指定的大小为 bytes 字节的内存空间
         * @param ptr 内存空间的指针
         * @param bytes 字节数
         */
        static void deallocate(void *ptr, size_t bytes) {
            if (bytes > static_cast<size_t>(MAX_BYTES)) {
                mallocAlloc::deallocate(ptr);
            }

            // 使用头插法将"释放"的内存空间插入到对应的链表头部
            size_t index = freeListIndex(bytes);
            obj *blockList = freeLists[index];
            obj *newBlockHead = static_cast<obj*>(ptr); // TODO 《深入探索 C++ 对象模型》中不能对 void* 操作的原因
            newBlockHead->nextBlock = blockList;
            freeLists[index] = newBlockHead;
        }

        /**
         * 调整 ptr 指向的内存空间的大小
         * SGI STL Allocator Design 中 The Original SGI STL allocator interface 一节介绍了该接口的具体信息。
         * http://decsai.ugr.es/mgsilvente/stl/alloc.html
         * 具体实现参考了 https://github.com/steveLauwh/SGI-STL/blob/be34aace923769ee114366e0e7decf8521d422b9/SGI-STL%20V3.3/stl_alloc.h#L528
         *
         * @param ptr 原内存空间的指针
         * @param oldSize 原内存空间的大小
         * @param newSize 调整后内存空间的大小
         * @return 新内存空间的地址
         */
        static void* reallocate(void *ptr, size_t oldSize, size_t newSize) {
            auto maxBytes = static_cast<size_t>(MAX_BYTES);
            if (oldSize > maxBytes && newSize > maxBytes) {
                // 新旧内存空间的大小都大于 128 bytes，则第一级内存空间配置器
                return mallocAlloc::reallocate(ptr, newSize);
            }

            // 如果新旧值上调到 8 的倍数后，是同一个数，则不需要调整，直接返回原内存空间的指针即可。
            if (roundUp(oldSize) == roundUp(newSize)) {
                return ptr;
            }

            void *result = allocate(newSize); // 从相应的内存块链表中取出内存块
            // 防止 oldSize > newSize 而造成 memcpy() 拷贝时出现问题
            size_t copySize = std::min(oldSize, newSize);
            // 注意：对内存空间的拷贝是按位执行的。如果内存空间上的对象有自定义的构造函数或者析构函数，
            //      则不建议使用 alloc::reallocate() 来调整内存空间。
            //      完全通用的容器实现通常不会使用 alloc::reallocate()。
            ::memcpy(result, ptr, copySize);
            deallocate(ptr, oldSize); // "释放"旧的内存空间

            return result;
        }

    private:
        // 《Effective c++》 Item 2 P15
        // 尽量使用const,枚举(enums),内联(inlines),不要使用宏定义(define)。
        // the enum hack，理论基础：一个属于枚举类型的数值可当做 int 被使用 。
        // the enum hack 比较像 #define 而不像 const。
        // 取 const 的地址是合法的，但取 enum 和 #define 的地址是不合法的。
        enum { ALIGN = 8 };                         // 小型内存块的大小上调边界
        enum { MAX_BYTES = 128 };                   // 小型内存块的大小上限
        enum { N_FREE_LISTS = MAX_BYTES / ALIGN };  // free-lists 个数：16
        enum { NOBJS = 20 };                        // 默认从内存池获取的内存块数量
        // 在新版本的编译器中，可以使用静态常量代替 the enum hack。
        // 通常 C++ 要求为使用的任何东西提供一个定义式，
        // 但如果它是 class 专属常量又是 static 且为整数类型（如 int、char、bool），
        // 只要不去它的地址，可以声明并使用它而无须提供定义式。
        // static const int ALIGN = 8;
        // static const int MAX_BYTES = 128;
        // static const int N_FREE_LISTS = MAX_BYTES / ALIGN;
        // static const int NOBJS = 20;
        // 如果需要取 class 的专属常量时，则在类外提供定义式。例如
        // const int alloc::ALIGN;
        // 如果旧编译器不支持上述语法，不允许 static 成员在其声明式中获得初值，
        // 则需要在类外声明并赋初值。例如
        // static const int ALIGN; // 类内
        // const int alloc::ALIGN = 8; // 类外
        // 此外，"in-class 赋初值"只有当成员是整数常量（如 int、char、bool）时才合法。

        // 内存块链表节点
        union obj {
            union obj *nextBlock; // 指针，指向下一个区块
            char nextData[1]; // 看作指针，指向数据
        };

        static obj *freeLists[N_FREE_LISTS]; // 内存块链表数组

        static char *startFree; // 内存池起始地址
        static char *endFree;   // 内存池末尾地址
        static size_t heapSize; // 内存池大小

        /**
         * 将 bytes 上调为 8 的倍数
         * @param bytes 字节数
         * @return 调整后的字节数
         */
        static size_t roundUp(size_t bytes) {
            // return (bytes + (ALIGN - 1)) & ~(ALIGN - 1); // 这是 SCI STL 中使用位操作的实现
            return (bytes / ALIGN + 1) * ALIGN;
        }

        /**
         * bytes 字节数对应的内存块链表在数组中的索引值
         * @param bytes 字节数
         * @return 索引值
         */
        static size_t freeListIndex(size_t bytes) {
            return (bytes + (ALIGN - 1)) / (ALIGN - 1);
        }

        /**
         * 当内存块链表无可用内存块时，调用 alloc::refill()，
         * 从内存池取出新的内存空间，将一个内存块返回给用户，其他则被填充到对应的内存块链表中。
         * 获取到的内存块数量 n 有 3 种情况：
         * 1. n = NOBJS（20）
         * 2. 1 < n < NOBJS（20)
         * 3. n = 1
         * 如果是1、2，则将第一个内存块返回给用户，其他填充到对应的内存块链表中；
         * 如果是3，则直接将内存块返回给用户，没有内存块可以填充到对应的内存块链表中。
         *
         * 因为获取到的内存空间是连续的，所以要将获取到的内存空间分成一个个内存块，并连接成链表。
         *
         * @param bytes 从内存池获取内存空间的大小（字节数）。注意：该函数假设 bytes 已经上调到 8 的倍数。
         * @return 获取到的内存空间的地址
         */
        static void* refill(size_t bytes) {
            // 将 nObjs 的引用传给 alloc::chunkAlloc()，该函数会将 nObjs 修改为实际获取到的内存块的个数。
            int nObjs = NOBJS;
            char *chunk = chunkAlloc(bytes, nObjs);
            // 只获取到 1 个内存块，则直接将该内存块返回用户
            if (nObjs == 1) {
                return static_cast<void*>(chunk);
            }

            size_t index = freeListIndex(bytes);
            auto nextBlok = reinterpret_cast<obj*>(chunk + bytes); // 第二个内存块的地址，第一个要返回给用户
            freeLists[index] = nextBlok; // 头插法，插入链表头部
            // 因为获取到的内存空间是连续的，所以要将获取到的内存空间分成 nObjs - 1 个内存块，并连接成链表。
            for (int i = 1; i < nObjs; ++i) {
                obj *currentBlock = nextBlok;
                // nextBlok = nextBlok + bytes;
                // 不能这样写，因为 bytes 的单位是字节，所以要转成 char*，才能相加。
                nextBlok = reinterpret_cast<obj*>(reinterpret_cast<char*>(nextBlok) + bytes);
                if (i == nObjs - 1) {
                    // 最后一个区块的 nextBlock 要指向 nullptr， 不能指向 nextBlock。
                    // 因为此时 nextBlock 指向申请到的内存最后一个字节后一个字节，不为 nullptr。
                    // 如果访问了该内存地址，可能会访问到非内存池的内存，
                    // 甚至当该地址的内存不是该进程的资源，还会导致内存访问越界。
                    currentBlock->nextBlock = nullptr;
                } else {
                    currentBlock->nextBlock = nextBlok;
                }
            }
        }

        /**
         * 从内存池中取出 nObjs * bytes 大小的内存空间
         * @param bytes 内存块的大小（字节数）。注意：该函数假设 bytes 已经上调到 8 的倍数。
         * @param nObjs 内存块数量。该参数是一个变量的引用，通过该引用，告诉用户实际获取到的内存块数量。
         * @return 获取到内存空间数量
         */
        static char* chunkAlloc(size_t bytes, int &nObjs) {
            char *result; // 指向获取到的内存空间
            size_t totalBytes = bytes * nObjs; // 总内存空间需求量
            size_t bytesLeft = endFree - startFree; // 内存池剩余空间
            if (bytesLeft >= totalBytes) {
                // 内存池剩余内存空间满足需求量
                result = startFree;
                startFree += totalBytes; // 调整内存池的其实地址

                return result;
            } else if (bytesLeft >= bytes) {
                // 内存池剩余内存空间不能完全满足需求量，
                // 但可以提供一个或以上的区块
                nObjs = static_cast<int>(bytesLeft / bytes);
                totalBytes = bytes * nObjs; // 修改实际得到的内存空间大小
                result = startFree;
                startFree += totalBytes; // 调整内存池的其实地址

                return result;
            } else {
                // 内存池剩余内存空间连一个区块大小的空间都无法提供，则要扩充内存池
                size_t neededBytes = 2 * totalBytes + roundUp(heapSize >> 4); // 扩充的大小
                // 如果内存池还有剩余内存空间，则将剩余内存空间填充到对应的内存块链表中
                if (bytesLeft > 0) {
                    size_t index = freeListIndex(bytesLeft);
                    auto newBlock = reinterpret_cast<obj*>(startFree);
                    newBlock->nextBlock = freeLists[index];
                    freeLists[index] = newBlock;
                }

                // 向系统申请内存空间，用来扩充内存池
                startFree = static_cast<char*>(::malloc(neededBytes));
                // 系统内存空间分配失败
                if (startFree == nullptr) {
                    // 尝试在内存块链表数组中寻找可用且空间足够大（>= bytes）的一个内存块
                    for (size_t i = bytes; i < MAX_BYTES; i += ALIGN) {
                        size_t index = freeListIndex(i);
                        if (freeLists[index] == nullptr) {
                            continue;
                        }

                        // 找到一块满足要求的内存块
                        startFree = reinterpret_cast<char*>(freeLists[index]); // 更新内存池地址
                        endFree = startFree + i;

                        freeLists[index] = freeLists[index]->nextBlock; // 取出内存块

                        // 此时内存池有可用内存空间，且能分配一个 bytes 大小的内存块。
                        // 递归调用自己，则能成功返回内存空间。
                        return chunkAlloc(bytes, nObjs);
                    }
                    endFree = nullptr;
                    // 最后的尝试
                    // 如果内存块链表数组中没有也用空间，则最后使用第一级内存空间配置器来获取内存空间。
                    // 第一级内存空间配置器有 out-of-memory 机制，
                    // 要么一直等到系统中可用内存分配过来，
                    // 要么因为用户没有设置自定义的处理函数（OutOfMemoryHandler）而抛出 bad_alloc 异常。
                    startFree = static_cast<char*>(mallocAlloc::allocate(neededBytes));
                }
                // 从系统中申请到内存空间，则更新内存池参数。
                heapSize += neededBytes;
                endFree = startFree + neededBytes;

                // 此时内存池有充足的内存空间，递归调用自己，则能成功返回内存空间。
                return chunkAlloc(bytes, nObjs);
            }
        }
    };
    char *alloc::startFree = nullptr;
    char *alloc::endFree = nullptr;
    size_t alloc::heapSize = 0;
    alloc::obj *alloc::freeLists[alloc::N_FREE_LISTS] = {nullptr};


    // 负责对象的内存操作和内容操作分开：
    // 1. allocate() 和 deallocate() 负责内存的分配和释放；
    // 2. construct() 和 destory() 负责对象构造和析构。
    // SGI STL Allocator Design
    // http://decsai.ugr.es/mgsilvente/stl/alloc.html
    template <class T>
    class allocator {
    public:
        typedef T           ValueType;      // 数据类型
        typedef T*          Pointer;        // 指针
        typedef const T*    ConstPointer;   // 指向常量的指针
        typedef T&          Reference;      // 引用
        typedef const T&    ConstReference; // 常量引用
        typedef size_t      SizeType;       // 数量类型
        typedef ptrdiff_t   DifferentType;  // 用来保存两个指针减法操作的结果

        template <class U>
        class rebind {
            typedef allocator<U> other;
        };

        /**
         * 分配一个足够 T 类型对象的内存空间
         * @return 内存空间起始地址指针
         */
        static Pointer allocate() {
            return allocator::allocate(sizeof(T));
        }

        /**
         * 分配 n 个足够 T 类型对象的内存空间
         * @param n 对象个数
         * @return 内存空间起始地址指针
         */
        static Pointer allocate(SizeType n) {
            if (n == 0) {
                return 0;
            }

            return static_cast<Pointer>(alloc::allocate(sizeof(T) * n));
        }

        /**
         * 销毁 ptr 指向的对象，释放内存
         * @param ptr
         */
        static void deallocate(Pointer ptr) {
            allocator::deallocate(ptr, 1);
        }

        /**
         * 销毁 ptr 指向的 n 个对象，释放内存
         * @param ptr 起始地址
         * @param n 对象个数
         */
        static void deallocate(Pointer ptr, SizeType n) {
            if (n == 0) {
                return;
            }
            alloc::deallocate(static_cast<void*>(ptr), sizeof(T) * n);
        }

        /**
         * 在 ptr 指向的内存空间上构造 T 对象，并传入 value 作为实参
         * @param ptr 用于构造 T 对象的内存空间
         * @param value 构造函数的实参
         */
        static void construct(Pointer ptr, ConstReference value) {
            new(ptr) T(value);
        }

        // T 的构造函数没有形参，其他同上
        static void construct(Pointer ptr) {
            // TODO 学习 new 的高级用法
            // placement new
            // 在 ptr 指向的空间上调用 T 的构造函数
            // 参考 https://zh.wikipedia.org/wiki/New_(C%2B%2B)#%E5%B8%A6%E4%BD%8D%E7%BD%AE%E7%9A%84new%E8%BF%90%E7%AE%97%E7%AC%A6%E8%A1%A8%E8%BE%BE%E5%BC%8F
            new(ptr) T();
        }


        /**
         * 析构 ptr 所指的对象
         * @param ptr 被析构的对象的指针
         */
        static void destory(Pointer ptr) {
            ptr->~T(); // 调用对象的析构函数
        }

        /**
         * 析构 [first, last) 范围内所指的对象
         * @param first 起始迭代器
         * @param last 最后一个要析构的迭代器的后一个迭代器
         */
        static void destory(Pointer first, Pointer last) {
            while (first++ != last) {
                destory(&*first);
            }
        }
    };
}

#endif //TINYSTL_ALLOCATE_H
