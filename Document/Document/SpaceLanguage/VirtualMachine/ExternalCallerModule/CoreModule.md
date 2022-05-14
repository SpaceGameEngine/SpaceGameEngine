# SpaceGameEngine/SpaceLanguage/VirtualMachine/ExternalCallerModule
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
|22|UInt8Load|MemoryAddress|UInt8|读取UInt8|
|23|UInt8Store|MemoryAddress Value|0|将值存储到UInt8中|
|24|UInt8Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制UInt8|
|25|UInt8Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|26|UInt8Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|27|UInt8Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|28|UInt8Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|29|UInt8Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|30|UInt8And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|31|UInt8Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|32|UInt8Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|33|UInt8Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|34|UInt8ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|35|UInt8ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|36|UInt8Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断UInt8相等|
|37|UInt8NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断UInt8不相等|
|38|UInt8Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断UInt8小于|
|39|UInt8LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断UInt8小于等于|
|40|UInt8Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断UInt8大于|
|41|UInt8GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断UInt8大于等于|
|42|Int16Load|MemoryAddress|Int16|读取Int16|
|43|Int16Store|MemoryAddress Value|0|将值存储到Int16中|
|44|Int16Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制Int16|
|45|Int16Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|46|Int16Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|47|Int16Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|48|Int16Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|49|Int16Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|50|Int16And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|51|Int16Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|52|Int16Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|53|Int16Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|54|Int16ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|55|Int16ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|56|Int16Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断Int16相等|
|57|Int16NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断Int16不相等|
|58|Int16Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断Int16小于|
|59|Int16LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断Int16小于等于|
|60|Int16Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断Int16大于|
|61|Int16GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断Int16大于等于|
|62|UInt16Load|MemoryAddress|UInt16|读取UInt16|
|63|UInt16Store|MemoryAddress Value|0|将值存储到UInt16中|
|64|UInt16Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制UInt16|
|65|UInt16Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|66|UInt16Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|67|UInt16Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|68|UInt16Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|69|UInt16Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|70|UInt16And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|71|UInt16Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|72|UInt16Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|73|UInt16Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|74|UInt16ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|75|UInt16ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|76|UInt16Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断UInt16相等|
|77|UInt16NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断UInt16不相等|
|78|UInt16Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断UInt16小于|
|79|UInt16LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断UInt16小于等于|
|80|UInt16Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断UInt16大于|
|81|UInt16GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断UInt16大于等于|
|82|Int32Load|MemoryAddress|Int32|读取Int32|
|83|Int32Store|MemoryAddress Value|0|将值存储到Int32中|
|84|Int32Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制Int32|
|85|Int32Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|86|Int32Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|87|Int32Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|88|Int32Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|89|Int32Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|90|Int32And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|91|Int32Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|92|Int32Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|93|Int32Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|94|Int32ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|95|Int32ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|96|Int32Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断Int32相等|
|97|Int32NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断Int32不相等|
|98|Int32Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断Int32小于|
|99|Int32LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断Int32小于等于|
|100|Int32Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断Int32大于|
|101|Int32GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断Int32大于等于|
|102|UInt32Load|MemoryAddress|UInt32|读取UInt32|
|103|UInt32Store|MemoryAddress Value|0|将值存储到UInt32中|
|104|UInt32Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制UInt32|
|105|UInt32Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|106|UInt32Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|107|UInt32Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|108|UInt32Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|109|UInt32Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|110|UInt32And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|111|UInt32Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|112|UInt32Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|113|UInt32Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|114|UInt32ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|115|UInt32ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|116|UInt32Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断UInt32相等|
|117|UInt32NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断UInt32不相等|
|118|UInt32Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断UInt32小于|
|119|UInt32LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断UInt32小于等于|
|120|UInt32Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断UInt32大于|
|121|UInt32GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断UInt32大于等于|
|122|Int64Load|MemoryAddress|Int64|读取Int64|
|123|Int64Store|MemoryAddress Value|0|将值存储到Int64中|
|124|Int64Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制Int64|
|125|Int64Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|126|Int64Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|127|Int64Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|128|Int64Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|129|Int64Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|130|Int64And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|131|Int64Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|132|Int64Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|133|Int64Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|134|Int64ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|135|Int64ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|136|Int64Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断Int64相等|
|137|Int64NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断Int64不相等|
|138|Int64Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断Int64小于|
|139|Int64LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断Int64小于等于|
|140|Int64Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断Int64大于|
|141|Int64GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断Int64大于等于|
|142|UInt64Load|MemoryAddress|UInt64|读取UInt64|
|143|UInt64Store|MemoryAddress Value|0|将值存储到UInt64中|
|144|UInt64Copy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制UInt64|
|145|UInt64Add|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|146|UInt64Subtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|147|UInt64Multiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|148|UInt64Divide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|149|UInt64Mod|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 % Arg1|
|150|UInt64And|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 & Arg1|
|151|UInt64Or|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 \| Arg1|
|152|UInt64Xor|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 ^ Arg1|
|153|UInt64Not|MemoryAddress(Result) MemoryAddress(Arg0)|0|Result = ~Arg0|
|154|UInt64ShiftLeft|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 << Arg1|
|155|UInt64ShiftRight|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 >> Arg1|
|156|UInt64Equal|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断UInt64相等|
|157|UInt64NotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断UInt64不相等|
|158|UInt64Less|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断UInt64小于|
|159|UInt64LessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断UInt64小于等于|
|160|UInt64Greater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断UInt64大于|
|161|UInt64GreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断UInt64大于等于|
|162|FloatLoad|MemoryAddress|Float|读取Float|
|163|FloatStore|MemoryAddress Value|0|将值存储到Float中|
|164|FloatCopy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制Float|
|165|FloatAdd|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|166|FloatSubtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|167|FloatMultiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|168|FloatDivide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|169|FloatEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断Float相等|
|170|FloatNotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断Float不相等|
|171|FloatLess|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断Float小于|
|172|FloatLessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断Float小于等于|
|173|FloatGreater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断Float大于|
|174|FloatGreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断Float大于等于|
|175|DoubleLoad|MemoryAddress|Double|读取Double|
|176|DoubleStore|MemoryAddress Value|0|将值存储到Double中|
|177|DoubleCopy|MemoryAddress(Dst) MemoryAddress(Src)|0|复制Double|
|178|DoubleAdd|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 + Arg1|
|179|DoubleSubtract|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 - Arg1|
|180|DoubleMultiply|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 * Arg1|
|181|DoubleDivide|MemoryAddress(Result) MemoryAddress(Arg0) MemoryAddress(Arg1)|0|Result = Arg0 / Arg1|
|182|DoubleEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 == Arg1|判断Double相等|
|183|DoubleNotEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 != Arg1|判断Double不相等|
|184|DoubleLess|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 < Arg1|判断Double小于|
|185|DoubleLessEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 <= Arg1|判断Double小于等于|
|186|DoubleGreater|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 > Arg1|判断Double大于|
|187|DoubleGreaterEqual|MemoryAddress(Arg0) MemoryAddress(Arg1)|Arg0 >= Arg1|判断Double大于等于|