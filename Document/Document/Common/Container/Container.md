# Space Game Engine
## Container(concept)
### 概念
&emsp;&emsp;`Container`即容器，是一类可以容纳其他任意数量对象并提供相关的增删查改功能的对象。
### 规定
&emsp;&emsp;一个标准的容器应满足以下要求：
* 有以下公有(public)函数
	* 默认构造函数
	* 拷贝/移动构造和赋值运算符，非模板版本和用以支持使用不同分配器的同种容器间的赋值的模板版本，共8个函数
	* 接受`std::initializer_list`的构造函数
	* `SizeType GetSize() const;`
	* `void Clear();`
	* `Iterator GetBegin();`
	* `Iterator GetEnd();`
	* `ConstIterator GetConstBegin() const;`
	* `ConstIterator GetConstEnd() const;`