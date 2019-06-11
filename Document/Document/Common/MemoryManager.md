# Space Game Engine
## Memory Manager
### `Allocator(concept)`
&emsp;&emsp;`Allocator`是`SpaceGameEngine`中用来提供内存分配接口的一**系列**类。一个`Allocator`类应该有四个静态成员函数:
```c++
static void* RawNew(SizeType size);

static void RawDelete(void* ptr);

template<typename T,typename... Args>
static T* New(Args&&... arg);

template<typename T>
static void Delete(T* ptr);
```
&emsp;&emsp;目前的`Allocator`类有`StdAllocator`和`MemoryManagerAllocator`两种，`DefaultAllocator`则代表着目前引擎默认使用的`Allocator`。

### `MemoryManager`
&emsp;&emsp;`MemoryManager`是`SpaceGameEngine`中的内存管理器，它根据**不同的分配要求**来调用其所管理的内存分配器(如`MemoryManager::FixedSizeAllocator`等)以完成内存管理工作。

### `MemoryManager::FixedSizeAllocator`
&emsp;&emsp;`MemoryManager::FixedSizeAllocator`是一种只负责分配一种固定规格(size)的内存块的分配器。

### `MemoryManager::MemoryBlockHeader`
&emsp;&emsp;`MemoryManager::MemoryBlockHeader`是`MemoryManager::FixedSizeAllocator`所分配的内存块的头部，其包含有指向下一个内存块的指针。值得注意的是，内存分配遵循这样的一个规则：
```c++
实际分配的内存空间大小=max(我们所要的内存空间大小,sizeof(MemoryManager::MemoryBlockHeader));
```
这是因为内存块头部的信息只在`MemoryManager::FixedSizeAllocator`对它进行管理时才有用，当整个内存块被分配出去后，其头部的信息就没有存在的必要了。

### `MemoryManager::MemoryPageHeader`
&emsp;&emsp;`MemoryManager::MemoryPageHeader`与`MemoryManager::MemoryBlockHeader`类似，实际上都是包含了一定的信息的内存区域的头部。然而，与内存块头部不同的是，内存页头部的信息是不会被抹去的，因为内存页头部只在`MemoryManager::FixedSizeAllocator`中被使用。而且内存页才是实际的内存持有者，即`MemoryManager::FixedSizeAllocator`先向系统申请一整个内存页所需的内存空间(头部所占的空间+所要使用的空间，这个空间大小在一个`MemoryManager::FixedSizeAllocator`中是固定不变的)，再在内存页除头部外的内存空间上划分出内存块，以供外部使用，最后，再销毁内存页，释放其所使用的内存空间。当然，一个`MemoryManager::FixedSizeAllocator`中所用到的内存页在大多数情况下都不止一个。