# SpaceGameEngine
## Bootstrap
### 用途
对于一个使用引擎的可执行程序来说，在某些平台下（如`Windows`）其必须自己在运行时去加载其所要利用到的各个模块，但是因为引擎自带的模块加载功能是在`Common`模块中的，所以其无法利用位于`Common`模块里的模块加载功能来加载`Common`模块自身，所以就有了这个`Bootstrap`模块去利用`Common`模块无关的系统API去先加载`Common`模块。
### 用法
`Bootstrap`模块在任何编译配置下都应该用静态链接库的形式被使用。其只应被实际的可执行程序模块使用。其用法是声明一个`Bootstrap`全局变量，这个全局变量应当有且只有一个。推荐的方法是只在程序的`main.cpp`中定义这个全局变量即可。
### 注意事项
`Bootstrap`模块会假设可执行程序位于项目文件夹的最外层，其余模块位于`Binary`文件夹里（最后发布程序时的文件结构，参考[PackagedStructure](./PackagedStructure.md)）。所以，如果使用`Visual Studio`进行调试时，可以将项目设置->调试目录下的工作目录设置为`$(SolutionDir)`以方便后续的调试工作。