# SpaceGameEngine
## Module
### 说明
`Module`模块是`Space Game Engine`所使用的一种代码组织形式。在某些情况下，模块与模块之间会存在着一些依赖和一些初始化和释放的相关工作需要在各个模块的初始化和释放时进行，同时考虑到在某些平台下，需要有对各个模块所对应的动态库进行按需延迟加载的需求，所以有了一整套模块`Module`和模块管理`ModuleManager`系统。

`Module`类是每个实际模块类都应当继承的基类，其目前有如下虚函数可供子类重写:
* `void OnLoad()`
* `void OnUnload()`

`ModuleManager`类是一个用来管理模块的管理器，其是一个单例，其目前所提供的功能如下：
* `LoadModule(name)` : 加载模块(不会重复加载)
### 如何注册模块
在每个模块的`Private`文件夹下建一个`模块名Module.cpp`文件，里面定义`class 模块名Module : public Module`(不必导出)，然后加上`SGE_MODULE(模块名);`即可。