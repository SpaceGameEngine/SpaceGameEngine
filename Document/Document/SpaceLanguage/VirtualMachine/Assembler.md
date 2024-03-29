# SpaceGameEngine/SpaceLanguage/VirtualMachine
## Assembler
### 语法
#### 指令格式
每条指令独占一行，具体格式为：
```
InstructionName Arguments...
```
`InstructionName`对应[`Instruction.md`](./Instruction.md)中表格内的指令名称，其后的参数应符合其对应的指令的要求的数量，一般用无符号整数表示，用空格分隔开不同的参数。

例子：
```
Copy 11 12
Add 13 14 15
```
#### 寄存器别名
可以使用特定的寄存器别名来表示某个寄存器，具体别名如下：
* `pc` ：程序计数寄存器
* `bp` ：基址寄存器
* `sp` ：栈指针寄存器
* `gp` ：全局变量指针寄存器
* `rv` ：返回值寄存器
* `cx` ：通用寄存器`x`
* `ax` ：参数寄存器`x`

例子：
```
Copy c0 a1
Copy sp c0
```

#### 注释
注释方式同`C`语言一致，即用`//`和`/* */`来进行注释。

#### 外部调用
外部调用`ExternalCall`的第二个参数（即调用目标）可以不必是一个无符号整数。其可以用以下形式来表示：
```
ModuleName:FunctionName
```
但是这里的`ModuleName`以及`FunctionName`必须得在编译前于`Assembler`中进行注册。

例子：
```
ExternalCall 11 CoreModule:Allocate
```

#### 标签
在`Assembler`中，可以用如下形式定义一个标签：
```
:LabelName
```
标签名`LabelName`不能重复。每个标签都对应着一个在定义这个标签时的实际代码位置，即当时生成的指令的长度。只要标签名`LabelName`有实际的定义位置，那么其就可以被用在任何需要`Data`参数（`UInt64`参数）的位置，无论是在定义前还是定义后。

例子：
```
:ProgramStart
Goto ProgramStart
```