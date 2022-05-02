# SpaceGameEngine/SpaceLanguage 
## Register
`SpaceLanguage`的寄存器有64个，除了下面指定的具有特殊用途的寄存器外，编号在`32`之前的寄存器为通用寄存器，其余的为外部调用参数寄存器。每个寄存器都可被看作是`UInt64`变量。

### Special Register
|编号|名称|用途|
|:-|:-|:-|
|0|ProgramCounter|程序计数寄存器|
|1|BasePointer|基址寄存器|
|2|StackPointer|栈指针寄存器|