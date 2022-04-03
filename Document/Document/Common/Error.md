# SpaceGameEngine
## 错误处理
### 概念
&emsp;&emsp;错误的检查分为两种:`Assert`和`Check`。两者的区别在于`Assert`只在`Debug`模式下才进行检查，而`Check`是不论`Debug`还是`Release`模式都进行检查的。  

&emsp;&emsp;因此`Assert`和`Check`两者有着完全不同的用处。
* `Assert`是用来检查条件的。在一个函数中`Assert`通常放在函数的开头，用来检查函数运行的条件（如参数的合法性、环境的配置是否正确等）。
* `Check`是用来检测无法预测的运行时错误的。在一个函数中其通常被放置在我们无法预测结果的函数调用（比如第三方函数调用）的后面用来检查函数调用返回的结果是否符合我们的需要。

### 概念的完备性分析
&emsp;&emsp;据我目前所知，以上的这种处理错误的方法是可以做到处理大部分程序所产生的错误的。且在`Release`模式生成时，没必要的检测(`Assert`)都被删去，每个错误检测都是不会重复的。具体论证如下：

&emsp;&emsp;我们可以把整个程序运行的过程看做是一系列函数调用的过程。这些函数都有参数（包括函数运行的环境），这些参数分为两种：**编译期确定的**和**运行时确定**的。编译期确定的参数都可以在`Debug`模式下被`Assert`检查，如果这些固定不变的参数在`Debug`模式下没有问题的话，那么在`Release`模式生成时，尽管`Assert`已经没有了，但这些固定不变的参数已经确定是正确的了，所以其在`Release`模式下也必然没有问题（前提是这些参数不会因生成模式的不同而发生变化）。所以，编译期就已经确定的参数的检查就完成了。再来看，运行时的错误。我们可以明确一点：运行时，我们自己的代码在大多数情况下是可控可预测的，所以基本不会出错（除了涉及到多线程的时候），大多数的运行时错误都出在第三方函数调用上。所以我们只需要在调用第三方函数后去用`Check`检查结果是否有错即可。当然，值得注意的是，在`Debug`模式下，`Assert`和`Check`的功能有些重复，因为`Assert`也会检查运行时传来的函数参数，但是没有关系，在`Release`模式下生成时，这些重复的检查都会被删去。

### 从概念到实践
&emsp;&emsp;如果完全按照概念来做错误处理，固然能够将大部分的错误都妥善地处理好。但是，随之而来的，是严峻的性能问题。尽管`Assert`在`Release`模式下没有作用，不带来性能负担。但是如果每调用一次第三方函数就用`Check`检查一次，所带来的负担还是很大的。所以，在实践过程中我们可以遵循以下规则来进行错误处理：  
1. 对每个函数的参数及所需的环境条件进行`Assert`检查：这不仅仅可以检查编译期的参数，同时在运行时（`Debug`模式下），其也可以检查函数调用的运行时的参数，但函数过程中的问题就无能为力了。
2. 对于那些我们可以预测结果的第三方函数调用就不用`Check`进行检查了，如数学计算函数等。对于那些有着还算好用的错误/异常处理的第三方函数调用，如`STL`等，我们可以信任他们的错误处理机制，不进行`Check`。但对于那些检测错误及其麻烦的，如系统调用等，我们要用`Check`进行检查。

### 使用`SpaceGameEngine`中的错误机制
#### `Error(concept)`
&emsp;&emsp;`Error`不是某个具体类，而是一个概念，代表着一系列类。一个`Error`类应有以下内容:
```c++
public:
	static const TChar sm_pContent[] = SGE_TSTR("错误消息内容");//错误消息内容

	static bool Judge();//通过检验其参数来判断是否出错，参数随意，返回true表示有错误发生，返回false表示无错误发生
```
&emsp;&emsp;这样，当你需要加入一种新的错误(`Error`)类型时，你只需要自己定义一个类或是结构体，为其加入这些内容即可。

#### `SGE_ASSERT`&`SGE_CHECK`
&emsp;&emsp;`SGE_ASSERT`、`SGE_CHECK`这两个宏分别对应着前文概念中的`Assert`和`Check`。两者的参数均为`(error_type,...)`，这个`...`代表着`Judge`的实参。

#### 自定义错误处理方式
&emsp;&emsp;一般情况下，出错会直接导致程序强制退出，这是`SpaceGameEngine`默认的错误处理方式。然而，当你需要自定义错误处理方式时，你可以直接在`SGE_ASSERT`、`SGE_CHECK`这两个宏后调用其`Handle`方法，传入一个可调用对象和若干用于调用该可调用对象的实际参数以实现自定义该`SGE_ASSERT`或是`SGE_CHECK`的错误处理方式。

#### 样例
```c++
struct NullPointerError
{
	inline static const TChar sm_pContent[] = SGE_TSTR("Pointer can not be null");
	static bool Judge(const void* ptr);
};

struct InvalidSizeError
{
	inline static const TChar sm_pContent[] = SGE_TSTR("The size is invalid");
	static bool Judge(SizeType size, SizeType min_size, SizeType max_size);
};
```
```c++
SGE_ASSERT(NullPointerError,(void*)1);
SGE_ASSERT(InvalidSizeError, 5, 1, 10);
```
```c++
int test = 0;
SGE_ASSERT(TestError).Handle([&]() {test = 1; });
```