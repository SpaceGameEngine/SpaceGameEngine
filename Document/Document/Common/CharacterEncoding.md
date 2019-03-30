# Space Game Engine
## Character Encoding
### 概念
&emsp;&emsp;我懒得写了。看[字符编码笔记：ASCII，Unicode 和UTF-8 - 阮一峰的网络日志](http://www.ruanyifeng.com/blog/2007/10/ascii_unicode_and_utf-8.html)

### `SpaceGameEngine`中关于字符编码问题的相关规定和说明
1. 所有的`Cpp`源代码都必须使用`UTF-8 with BOM`的格式保存。
2. 考虑到不同的平台所主要使用的字符编码是不同的，为了避免过多的字符编码转换影响性能，`SpaceGameEngine`使用`TChar`类型来储存程序运行时所使用到的字符，`TChar`的具体类型和其所使用的字符编码由程序当前所在的平台(编译的目标平台)所决定。
3. 所有跨平台的内容(如文件、网络数据等)在使用时都必须指明一种明确的字符编码。