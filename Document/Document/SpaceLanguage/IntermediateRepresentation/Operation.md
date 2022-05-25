# SpaceGameEngine/SpaceLanguage/IntermediateRepresentation
## Operation
`Operation`是`SpaceLanguage`中中间表示代码的基本形式，是一种三地址码。
## OperationType
|名称|参数|功能|
|:-|:-|:-|
|Set|Local/Global Const(i) Const(value)|为变量的第i个类型部分赋值value|
|NewLocal|Local|创建一个临时变量（应与DeleteLocal成对使用）|
|DeleteLocal|Local|删除一个临时变量（应与NewLocal成对使用）|
|Push|Local/Global|将变量的拷贝压入栈中|
|Pop|Local/Global|将栈顶变量弹出并复制到指定变量中|
|Copy|Local/Global(Dst) Local/Global(Src)|把Src变量的内容复制到Dst变量中|