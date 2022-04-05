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
|指令编号|指令名称|参数|说明|
|:-|:-|:-|:-|
|0|ExternalCall|Reg Data|调用外部函数，并将结果放到寄存器中|
|1|Set|Reg Data|设置寄存器的值|
|2|Copy|Reg(Dst) Reg(Src)|复制寄存器的值|
|3|Goto|Data|指令跳转|
|4|GotoRegister|Reg|指令跳转|
|5|If|Reg|条件指令跳转|
#### Math
对寄存器的值进行的数学计算

|指令编号|指令名称|参数|说明|
|:-|:-|:-|:-|
|6|Add|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 + Arg1|
|7|Subtract|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 - Arg1|
|8|Multiply|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 * Arg1|
|9|Divide|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 / Arg1|
|10|Mod|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 % Arg1|
|11|And|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 & Arg1|
|12|Or|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 \| Arg1|
|13|Xor|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 ^ Arg1|
|14|Not|Reg(Dst) Reg(Arg0)|Dst = ~Arg0|
|15|ShiftLeft|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 << Arg1|
|16|ShiftRight|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = Arg0 >> Arg1|
|17|SelfAdd|Reg(Dst) Reg(Arg0)|Dst = Dst + Arg0|
|18|SelfSubtract|Reg(Dst) Reg(Arg0)|Dst = Dst - Arg0|
|19|SelfMultiply|Reg(Dst) Reg(Arg0)|Dst = Dst * Arg0|
|20|SelfDivide|Reg(Dst) Reg(Arg0)|Dst = Dst / Arg0|
|21|SelfMod|Reg(Dst) Reg(Arg0)|Dst = Dst % Arg0|
|22|SelfAnd|Reg(Dst) Reg(Arg0)|Dst = Dst & Arg0|
|23|SelfOr|Reg(Dst) Reg(Arg0)|Dst = Dst \| Arg0|
|24|SelfXor|Reg(Dst) Reg(Arg0)|Dst = Dst ^ Arg0|
|25|SelfNot|Reg(Dst)|Dst = ~Dst|
|26|SelfShiftLeft|Reg(Dst) Reg(Arg0)|Dst = Dst << Arg0|
|27|SelfShiftRight|Reg(Dst) Reg(Arg0)|Dst = Dst >> Arg0|
#### Judgement
对寄存器的值进行的判断

|指令编号|指令名称|参数|说明|
|:-|:-|:-|:-|
|28|Equal|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 == Arg1)?1:0|
|29|NotEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 != Arg1)?1:0|
|30|Less|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 < Arg1)?1:0|
|31|LessEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 <= Arg1)?1:0|
|32|Greater|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 > Arg1)?1:0|
|33|GreaterEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 >= Arg1)?1:0|