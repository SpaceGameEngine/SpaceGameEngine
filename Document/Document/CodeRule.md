﻿# Space Game Engine
## Code Rule
### 函数命名的含义
&emsp;&emsp;**每个函数都应该符合与其函数名所对应的相应含义**。只有在整个项目中时时刻刻地遵守这个规定，我们才能保证函数的语义的一致性。以下是目前`SpaceGameEngine`中所规定的几种函数名称形式与其所对应的语义：
1. `Get...`:获取某个对象，如果找不到所要获取的对象，则报错。如果函数返回的结果是明确的，也使用`Get`来命名。因此，我们可以认为：所有的`Get...`系列函数的返回的结果都是**可预期**的，不需要用`Check`进行检查。
2. `Query...`:查询某个对象，如果找到则正常返回所要获取的对象，否则返回一个空的对象（有可能是`nullptr`或是临时构造的对象）。因此，我们可以认为：所有的`Query...`系列函数的返回的结果都是**不可预期**的，有可能需要使用`Check`进行检查（视具体使用方式而定）。
### 宏的命名
&emsp;&emsp;所有的`SpaceGameEngine`的宏都以`SGE_`开头，全部使用大写，单词间用`_`分割。