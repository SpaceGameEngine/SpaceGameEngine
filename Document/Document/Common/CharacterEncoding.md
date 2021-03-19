# Space Game Engine
## Character Encoding
### 概念
&emsp;&emsp;我懒得写了。看[字符编码笔记：ASCII，Unicode 和UTF-8 - 阮一峰的网络日志](http://www.ruanyifeng.com/blog/2007/10/ascii_unicode_and_utf-8.html)

### `SpaceGameEngine`中关于字符编码问题的相关规定和说明
1. 所有的`Cpp`源代码都必须使用`UTF-8 with BOM`的格式保存。
2. 考虑到不同的平台所主要使用的字符编码是不同的，为了统一字符编码，方便跨平台的使用，我们决定将所有运行时字符串默认的编码设为`UCS2`。
3. 同时对于某些只需要临时使用的字符串，我们采用所处操作系统支持度最好的字符编码来表示(Windows为ucs2，unix为utf8)，这样的编码不确定的字符称为`TChar`，字符串为`TString`。
4. 所有跨平台的内容(如文件、网络数据等)在使用时都必须指明一种明确的字符编码。