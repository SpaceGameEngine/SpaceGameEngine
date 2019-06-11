# Space Game Engine
## Project Struction
> 注：这里的`$(ProjectName)`是VS的宏，在`SpaceGameEngine`中实际上是指**模块**名，而不是**项目**名。
* **Binary** 存放项目编译出的二进制文件
	* `$(ProjectName)\$(PlatformTarget)\$(Configuration)\` 编译的目标文件
	* `$(ProjectName)\$(PlatformTarget)\$(Configuration)\Cache\` 编译的中间文件
* **Document** 文档
	* **Document** 手写的文档
	* **Reference** `Doxygen`生成的`API Reference`
* **Source** 源代码
	* `$(ProjectName)` 各种模块
		* `Public\` 模块中公开的代码
		* `Private\` 模块中私有的代码
* **Test** 测试代码
	* `$(ProjectName)` 各种测试模块
* **ThirdParty** 第三方代码
	* `$(ProjectName)` 各种第三方代码包装而成的模块