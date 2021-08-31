# Space Game Engine
## GlobalVariable
`GlobalVariable`是`Space Game Engine`所用来管理全局变量的一套工具模板类，其配合`local static`技法可以解决C++中的全局变量的构造顺序和析构顺序的问题。整个`GlobalVariable`系统是利用`Stack`来实现的，是**线程安全**的。整个系统的构造顺序是利用`local static`来保证的，在构造的同时依照构造的顺序将正在构造的`GlobalVariable`对象所包含的实际的全局变量（实际上是一个指针，在初始化时`New`）以`GlobalVariableCore`的形式（为了能够以多态的方法去`Delete`实际的全局变量指针）放入到`GlobalVariableManager`单例的`Stack`成员变量中（这个过程是有锁的）。然后在`GlobalVariableManager`单例析构时，按照其内`Stack`所存储的顺序去逐个调用每个全局变量的释放函数（即`Delete`实际的全局变量指针），依次出栈，完成整个顺序析构过程。

**注意事项**：使用`GlobalVariable`的全局变量不能在其析构时才引入新的使用`GlobalVariable`的全局变量（不能有这种可能性）。