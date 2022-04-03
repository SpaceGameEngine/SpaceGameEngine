# SpaceGameEngine
## Project Rule
### ProjectDefaultProperty
1. 所有项目都要在属性管理器中添加`ProjectCommonProperty.props`属性文件
2. 属性页 - 配置属性 - 常规 - Windows SDK版本改为`$([Microsoft.Build.Utilities.ToolLocationHelper]::GetLatestSDKTargetPlatformVersion('Windows', '10.0'))`
3. 属性页 - 配置属性 - 常规 - 平台工具集改为`$(DefaultPlatformToolset)`

### ThirdParty
&emsp;&emsp;所有的第三方库都要放到解决方案文件夹下的`ThirdParty`文件夹中。具体细节如下:
* 静态库:  
1. 要在解决方案中的`ThirdParty`文件夹中添加现有项目。
2. 根据[ProjectDefaultProperty](#ProjectDefaultProperty)添加项目的属性。
3. 添加空白的`licenseheader`文件于库所在的文件夹中。
* 头文件库:  
1. 要在解决方案中的`ThirdParty`文件夹中添加同名文件夹，并添加现有文件。
2. 添加空白的`licenseheader`文件于库所在的文件夹中。