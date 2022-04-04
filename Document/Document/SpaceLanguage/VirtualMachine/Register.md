# SpaceGameEngine/SpaceLanguage 
## Register
`SpaceLanguage`的寄存器有32个，除了下面指定的具有特殊用途的寄存器外，其余皆为通用寄存器。每个寄存器都可被看作是`UInt64`变量。

### Special Register
|编号|名称|用途|
|:-|:-|:-|
|0|Arg0|外部调用参数0|
|1|Arg1|外部调用参数1|
|2|Arg2|外部调用参数2|
|3|PC|程序计数寄存器|
|4|BP|基址寄存器|
|5|SP|栈指针寄存器|