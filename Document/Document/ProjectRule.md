# Space Game Engine
## ProjectRule
### ProjectDefaultProperty
&emsp;&emsp;所有项目都要在属性管理器中添加`ProjectCommonProperty.props`属性文件

### ThirdParty
&emsp;&emsp;所有的第三方库都要放到解决方案文件夹下的`ThirdParty`文件夹中。具体细节如下:
* 静态库:  
要在解决方案中的`ThirdParty`文件夹中添加现有项目。  
根据[ProjectDefaultProperty](#ProjectDefaultProperty)添加项目的属性。  
添加空白的`licenseheader`文件于库所在的文件夹中。
* 头文件库:  
要在解决方案中的`ThirdParty`文件夹中添加同名文件夹，并添加现有文件。  
添加空白的`licenseheader`文件于库所在的文件夹中。