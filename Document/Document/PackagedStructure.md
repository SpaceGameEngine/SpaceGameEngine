# SpaceGameEngine
## Packaged Structure
> 因为不同操作系统的操作系统的习惯和配置方法不同，所以目前还是主要在`Windows`平台上使用自定义的打包结构，在其他平台上则依据其上的其他项目的一般结构来确定`Space Game Engine`在这些平台上的具体打包结构。
### Windows
* 打包后的根目录
	* **Binary** 存放项目所依赖的所有二进制文件
		* `$(ProjectName)\$(PlatformTarget)\$(Configuration)\` 依赖的二进制文件
	* 项目名.exe

> 有待日后继续完善