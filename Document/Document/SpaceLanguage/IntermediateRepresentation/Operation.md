# SpaceGameEngine/SpaceLanguage/IntermediateRepresentation
## Operation
`Operation`是`SpaceLanguage`中中间表示代码的基本形式。
## OperationType
> Variable = Global/Local/Reference  
> Value = Constant/Variable

|名称|参数|功能|变量寄存器个数|额外寄存器个数|
|:-|:-|:-|:-|:-|
|Set|Variable Constant(i) Constant(value)|为变量的第i个类型部分赋值value|1|2|
|NewLocal|Local|创建一个临时变量（应与DeleteLocal成对使用）|1|0|
|DeleteLocal|Local|删除一个临时变量（应与NewLocal成对使用）|1|0|
|Push|Value|将变量的拷贝压入栈中|1|1|
|Pop|Variable|将栈顶变量弹出并复制到指定变量中|1|1|
|Copy|Variable(dst) Variable(src)|把src变量的内容复制到dst变量中|2|0|
|Goto|Constant(basic block)|在当前函数体内跳转到某个基本块|0|0|
|If|Variable(condition) Constant(then basic block) Constant(else basic block)|在当前函数体内如果条件成立(值大于0)，则跳转到某个基本块，否则跳转到另一个基本块|1|0|
|Call|Constant(function)|调用函数，函数参数需要用Push提前推入栈中|0|0|
|CallFunctionPointer|Variable(function pointer)|用函数指针调用函数，函数参数需要用Push提前推入栈中|1|0|
|Return|Value(value)|函数返回返回值|1|0|
|ExternalCallArgument|Constant(i) Value(value)|设置外部调用的第i个参数|1|0|
|ExternalCall|Constant(module) Constant(function)|进行外部调用|0|0|
|GetReturnValue|Variable(dst)|获取函数(包括外部调用)返回值|1|0|
|MakeReference|Reference(dst) Variable(src)|创建一个引用与给定变量绑定|2|0|
|GetAddress|Variable(dst) Variable(src)|获取一个变量的地址|2|0|
|GetReference|Reference(dst) Variable(src)|创建一个引用与给定变量所指向的变量绑定|2|0|
|ReleaseReference|Reference|释放一个引用|1|0|