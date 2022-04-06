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
|5|If|Reg Data|条件指令跳转|
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

#### Judgement
对寄存器的值进行的判断

|指令编号|指令名称|参数|说明|
|:-|:-|:-|:-|
|17|Equal|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 == Arg1)?1:0|
|18|NotEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 != Arg1)?1:0|
|19|Less|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 < Arg1)?1:0|
|20|LessEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 <= Arg1)?1:0|
|21|Greater|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 > Arg1)?1:0|
|22|GreaterEqual|Reg(Dst) Reg(Arg0) Reg(Arg1)|Dst = (Arg0 >= Arg1)?1:0|