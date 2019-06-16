# Space Game Engine
## Memory Manager
### `SpaceGameEngine`中的内存管理原理
#### 内存分配/释放策略
&emsp;&emsp;对于内存分配/释放请求，内存管理器会根据所请求分配/释放的内存大小来决定使用哪种内存分配/释放策略。
* 对于那些要求分配/释放的内存大小大于**1024**字节的，会直接使用`new/delete`进行分配或释放。
* 对于那些要求分配/释放的内存大小小于等于**1024**字节的，则采用内存管理器中的**固定大小分配器**进行分配或释放。所谓固定大小分配器，就是最简化的分配器，其只负责分配/释放一种固定大小的内存块。内存管理器中自带一部分的固定大小分配器，内存管理器会从中选取一个所能分配/释放的内存块大小刚好大于或恰好等于所请求分配/释放的内存大小的固定大小分配器去完成这个分配或是释放内存的任务。

#### 内存对齐策略
&emsp;&emsp;在内存分配时，为了便于内存对齐，我们做了以下约定:对于那些默认对齐方式(`alignment=0`)的分配请求，如果其要求分配的内存大小大于16字节，我们让其关于16字节对齐，否则，我们让其关于一个恰好大于或刚好等于其内存大小且是2的n次方的数对齐(1/2/4/8/16)。当然你也可以自定义对齐方式，但是内存管理器所支持的最大对齐为1024字节。

### `SpaceGameEngine`中的内存管理应用
#### `Allocator(concept)`
&emsp;&emsp;`Allocator`是`SpaceGameEngine`中用来提供内存分配接口的一**系列**类。一个`Allocator`类应该有四个静态成员函数:
```c++
static void* RawNew(SizeType size, SizeType alignment = 0);

static void RawDelete(void* ptr);

template<typename T,typename... Args>
static T* New(Args&&... arg);

template<typename T>
static void Delete(T* ptr);
```
&emsp;&emsp;目前的`Allocator`类有`StdAllocator`和`MemoryManagerAllocator`两种，`DefaultAllocator`则代表着目前引擎默认使用的`Allocator`。用户只需直接使用这些`Allocator`即可完成内存管理的任务。