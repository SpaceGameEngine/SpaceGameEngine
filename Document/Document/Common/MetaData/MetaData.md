﻿# Space Game Engine
## MetaData
### 概念
&emsp;&emsp;`MetaData`是一个用于在运行时储存必要的类型信息的结构体类型，其所储存的类型信息将用于实现反射及相关功能。从某种角度来说，它是`type_info`的升级版，因此也能用于`RTTI`。
### `MetaData`的获取
#### 目前
&emsp;&emsp;目前的获取方法还是使用`type_traits`来判断类型的某些性质以包装其必要的一些函数(构造、赋值和比较)并使用`typeid`及其他关键字来获取其名称、大小、对齐方式等信息。然而这些都是依托于编译器来实现的，有很大的局限性(如必须明确指定类型，还并不是真正的运行时获取)。
#### 将来
&emsp;&emsp;将来所要使用的元信息获取方法则要更为的易于使用。首先，项目管理器会在项目编译时对项目代码进行解析，读取其需要获取元信息的类型的类型信息，然后再用这些信息生成代码以填充元信息到二进制文件中。而目前的依靠编译器的编译期获取方式将作为一种缺省的元信息获取方式用于获取那些事先无法得知类型名的类型(如临时的lambda的类型)的元信息。