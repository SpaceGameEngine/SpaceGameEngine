# SpaceGameEngine
## Container Concept
&emsp;&emsp;`Concept`是`Container`的基石。通过这种对类型的高度抽象，我们可以明确地去规定什么样的容器应该符合怎样的行为，什么样的元素类型能被这样的容器接受。所以，这里的`Concept`主要干两件事：规定容器所接受的元素、规定容器自身的行为。

&emsp;&emsp;然而，值得注意的是，当下Cpp20还没有全面地普及和得到编译器的支持。为了实现`Concept`功能，这里我决定先使用SFINAE来模拟`Concept`。