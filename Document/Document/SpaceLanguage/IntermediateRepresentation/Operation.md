# SpaceGameEngine/SpaceLanguage/IntermediateRepresentation
## Operation
`Operation`是`SpaceLanguage`中中间表示代码的基本形式，是一种三地址码。
## OperationType
> Variable = Global/Local/Reference

|名称|参数|功能|
|:-|:-|:-|
|Set|Variable Const(i) Const(value)|为变量的第i个类型部分赋值value|
|NewLocal|Local|创建一个临时变量（应与DeleteLocal成对使用）|
|DeleteLocal|Local|删除一个临时变量（应与NewLocal成对使用）|
|Push|Variable|将变量的拷贝压入栈中|
|Pop|Variable|将栈顶变量弹出并复制到指定变量中|
|Copy|Variable(dst) Variable(src)|把src变量的内容复制到dst变量中|
|Label|Const(id)|在函数体内创建一个标签(不同函数间的标签相互独立)|
|Goto|Const(label)|在当前函数体内跳转到某个标签|
|If|Variable(condition) Const(label)|在当前函数体内如果条件成立(值大于0)，则跳转到某个标签|
|Call|Const(function)|调用函数，函数参数需要用Push提前推入栈中|
|CallFunctionPointer|Variable(function pointer)|用函数指针调用函数，函数参数需要用Push提前推入栈中|
|Return|Variable(value)|函数返回返回值|
|ExternalCallArgument|Const(i) Variable(value)|设置外部调用的第i个参数|
|ExternalCall|Const(module) Const(function)|进行外部调用|
|GetReturnValue|Variable(dst)|获取函数(包括外部调用)返回值|
|MakeReference|Reference(dst) Variable(src)|创建一个引用与给定变量绑定|
|GetAddress|Variable(dst) Variable(src)|获取一个变量的地址|
|GetReference|Reference(dst) Variable(src)|创建一个引用与给定变量所指向的变量绑定|
|ReleaseReference|Reference|释放一个引用|