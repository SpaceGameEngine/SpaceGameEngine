# SpaceGameEngine/SpaceLanguage 
## Instruction
### Format
`SpaceLanguage`的指令有5种格式，其大小分别为16位、24位、32位、72位和80位，具体如下：  
16位：
```
+-------------+------------+
|instr_type(8)|argument0(8)|
+-------------+------------+
```
24位:
```
+-------------+------------+------------+
|instr_type(8)|argument0(8)|argument1(8)|
+-------------+------------+------------+
```
32位:
```
+-------------+------------+------------+------------+
|instr_type(8)|argument0(8)|argument1(8)|argument2(8)|
+-------------+------------+------------+------------+
```
72位：
```
+-------------+--------+
|instr_type(8)|data(64)|
+-------------+--------+
```
80位：
```
+-------------+------------+--------+
|instr_type(8)|argument0(8)|data(64)|
+-------------+------------+--------+
```

### Argument Type
* `Reg`：寄存器编号
* `Data`：固定的常量数据(64位)

### Instruction Type
#### Core
|指令名称|参数|说明|
|:-|:-|:-|
|ExternalCall|Reg Data|调用外部函数，并将结果放到寄存器中|
|Set|Reg Data|设置寄存器的值|
|Copy|Reg(Dst) Reg(Src)|复制寄存器的值|
|Goto|Data|指令跳转|
|GotoRegister|Reg|指令跳转|
|If|Reg|条件指令跳转|
#### Math
对寄存器的值进行的数学计算
|指令名称|参数|说明|
|:-|:-|:-|
|Add|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 + Arg1|
|Subtract|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 - Arg1|
|Multiply|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 * Arg1|
|Divide|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 / Arg1|
|Mod|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 % Arg1|
|And|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 & Arg1|
|Or|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 \| Arg1|
|Xor|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 ^ Arg1|
|Not|Reg(Dst) Reg(Arg0)|Dst = ~Arg0|
|ShiftLeft|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 << Arg1|
|ShiftRight|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 >> Arg1|
|SelfAdd|Reg(Dst) Reg(Arg0)|Dst = Dst + Arg0|
|SelfSubtract|Reg(Dst) Reg(Arg0)|Dst = Dst - Arg0|
|SelfMultiply|Reg(Dst) Reg(Arg0)|Dst = Dst * Arg0|
|SelfDivide|Reg(Dst) Reg(Arg0)|Dst = Dst / Arg0|
|SelfMod|Reg(Dst) Reg(Arg0)|Dst = Dst % Arg0|
|SelfAnd|Reg(Dst) Reg(Arg0)|Dst = Dst & Arg0|
|SelfOr|Reg(Dst) Reg(Arg0)|Dst = Dst \| Arg0|
|SelfXor|Reg(Dst) Reg(Arg0)|Dst = Dst ^ Arg0|
|SelfNot|Reg(Dst)|Dst = ~Dst|
|SelfShiftLeft|Reg(Dst) Reg(Arg0)|Dst = Dst << Arg0|
|SelfShiftRight|Reg(Dst) Reg(Arg0)|Dst = Dst >> Arg0|
#### Judgement
对寄存器的值进行的判断
|指令名称|参数|说明|
|:-|:-|:-|
|Equal|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 == Arg1)?1:0|
|NotEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 != Arg1)?1:0|
|Less|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 < Arg1)?1:0|
|LessEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 <= Arg1)?1:0|
|Greater|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 > Arg1)?1:0|
|GreaterEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 >= Arg1)?1:0|