# SpaceGameEngine

## 基础线程支持

### 线程

`Concurrent/Thread.h` 中提供了线程的相关API。

`SpaceGameEngine::Thread` 类为线程的抽象，内部使用`std::thread`作为实现。请参考`std::thread`的相关文档来获得详细使用方法。

#### 线程构造与赋值

`SGE::Thread`和`std::thread`具有相同的参数的构造函数和赋值运算符。即：

1. 支持以任何**可调用**对象及其可接受参数构造
2. 为**可移动构造**、**可移动赋值**，不为**可拷贝构造**、**可拷贝赋值**

`Thread`对象的构造即开启新的系统线程并在新的线程上运行参数所指定的任务。

#### 线程加入与分离

`SGE::Thread`提供了成员函数以用于加入或分离所对应的线程。

`SGE::Thread::IsJoinable()`函数的返回值表示当前线程是否可被加入。在不可被加入的`SGE::Thread`对象上调用`Join()`方法会抛出异常。若满足以下条件之一则线程对象不可加入：

1. 线程已被分离
2. 线程已被加入
3. 线程对象被移动

`SGE::Thread::Detach()`函数将**尝试**分离线程。若无法分离则不进行任何操作。该函数调用后，调用者线程与`Thread`对象所表示之线程的运行将没有关联。

`SGE::Thread`的析构函数将会**尝试**分离线程。

#### 线程ID

若持有`SGE::Thread`类的对象，可以通过`SGE::Thread`的成员函数`GetThreadID()`获取该线程ID。

无法获取当前线程的对象，但是可以通过`SGE::Thread`的静态类函数`GetCurrentThreadID()`直接获取当前线程ID。

线程ID的具体表示依赖于操作系统平台。

#### 线程让步与休眠

通过调用`SGE::Thread`的静态类函数`YieldCurrentThread()`或`Sleep(duration)`来使当前线程主动让步执行机会或使当前线程休眠指定的时间。

仅可以影响当前线程的执行情况。

### 锁

`Concurrent/Lock.h` 中提供了与**锁**有关的API。

#### 互斥器（Mutex）

`SGE::Mutex`类表示互斥器。只能通过SpaceGameEngine中的**锁**来锁定互斥器以提供安全的共享内存访问。`SGE::Mutex`内**没有**锁定的相关函数。

`SGE::Mutex`只能通过无参数构造器获取。与`std::mutex`相同，`SGE::Mutex`**不可**拷贝或移动。

#### 可重入锁（RecursiveLock）

`SGE::RecursiveLock`类表示可重入锁。

##### 构造

需要提供一个互斥器（`SGE::Mutex`）来构造`SGE::RecursiveLock`。

`SGE::RecursiveLock`可移动，不可拷贝。

##### 锁定

`Lock()`成员函数将会锁定关联互斥器（构造时传入的互斥器）。当关联互斥器不可用时阻塞当前线程，直到关联互斥器可用并被当前线程锁定。

`TryLock()`成员函数会**尝试**锁定关联互斥器。若关联互斥器当前不可用则**立即**返回`false`，否则锁定关联互斥器并返回`true`。

`TryLock(duration)`成员函数会在一定时间期限内**尝试**错定关联互斥器。若在指定的时间内成功锁定则返回`true`，否则返回`false`。建议使用该函数锁定线程以避免活性失败，并提过检查内部逻辑错误的机会。

同一个线程均可以多次调用上述函数重复锁定同一个互斥器。需要调用相同数量的`Unlock()`函数以完全释放互斥器。`SGE::RecursiveLock`的析构函数保证完全释放互斥器。

#### RAII风格的锁包装器（ScopedLock）

通过`SGE::ScopedLock`的构造函数来锁定给定的任意数量的互斥器。构造函数锁定多个互斥器时保证不发生死锁。其析构函数保证释放所有关联的的互斥器。

`SGE::ScopedLock`在互斥器当前不可用时阻塞线程。`SGE::ScopedLock`不支持`TryLock()`或`TryLock(duration)`的语义。

可以锁定当前线程已经锁定的互斥器。

#### 条件变量（Condition）

`SGE::Condition`类提供了条件变量的支持。使用条件变量可以实现多个读取者线程在某个共享变量不满足条件时阻塞，而另一个写入者线程锁定并更改该同步变量并通知读取者检查条件的功能。

任何有意读取某共享变量并当其不满足条件时阻塞的线程应该：

1. 使用`SGE::RecursiveLock`来锁定保护该共享变量
2. 使用`SGE::Condition`的相关等待函数解除锁定并挂起等待
3. 当等待结束（如被其他线程唤醒，等待超时或发生了虚假唤醒）后，自动锁定互斥并检查条件。若不满足条件则继续重复等待

任何有意更改共享变量的线程应该：

1. 使用`SGE::RecursiveLock`来锁定保护该共享变量
2. 更改共享变量的值
3. 使用`SGE::Condition`的相关函数通知在该条件下挂起的线程

在该过程中，互斥器（`SGE::Mutex`）对象与条件变量（`SGE::Condition`）都是在各个线程中共享的。

`SGE::Condition`不为**可移动构造**，**可移动赋值**，**可拷贝构造**，**可拷贝赋值**

##### 等待

`Wait(lock)`,`Wait(lock, predicate)`, `WaitFor(lock, duration)`, `WaitFor(lock, duration, predicate)`均可使当前线程在该条件对象上等待。

它们都接受一个`SGE::RecursiveLock`锁对象为第一个参数，而且该锁必须为当前线程锁定。等待方法会立即解锁该锁后挂起等待。

等待结束后条件对象会立刻锁定锁，然后无`predicate`参数或`predicate() == true`后函数返回，否则继续解锁挂起等待。

有`duration`作为参数的函数将会在等待超时后锁定锁并返回`false`，否则返回`true`。

##### 唤醒

`NodifyOne()`与`NodifyAll()`函数均可以通知等待线程，不同的是第一个函数会通知再次条件变量上阻塞的一个随机线程，而第二个会通知所有线程。
