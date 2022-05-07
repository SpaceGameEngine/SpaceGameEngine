# SpaceGameEngine/SpaceLanguage/ExternalCallerModule
## CoreModule
`CoreModule`的`Id`为0，有以下函数：
|Id|函数名|参数|返回值|功能|
|:-|:-|:-|:-|:-|
|0|Allocate|Size Alignment|内存地址|申请内存|
|1|Free|MemoryAddress|0|释放内存|
|2|Int8Load|MemoryAddress|Int8|读取Int8|
|3|Int8Store|MemoryAddress Value|0|将值存储到Int8中|
|4|Int8Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制Int8|
|5|Int8Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|6|Int8Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|7|Int8Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|8|Int8Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|9|Int8Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|10|Int8And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|11|Int8Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|12|Int8Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|13|Int8Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|14|Int8ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|15|Int8ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|16|Int8Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断Int8相等|
|17|Int8NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断Int8不相等|
|18|Int8Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断Int8小于|
|19|Int8LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断Int8小于等于|
|20|Int8Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断Int8大于|
|21|Int8GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断Int8大于等于|