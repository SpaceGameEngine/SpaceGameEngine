# Space Game Engine
## ProjectRule
### BuildTarget
&emsp;&emsp;所有项目的输出目录和中间目录都要修改。具体如下:
* x86 :
  * 输出目录 :`$(SolutionDir)Binary\$(ProjectName)\x86\$(Configuration)\`
  * 中间目录 :`$(SolutionDir)Binary\$(ProjectName)\x86\$(Configuration)\Cache\`
* x64 :
  * 输出目录 :`$(SolutionDir)Binary\$(ProjectName)\$(Platform)\$(Configuration)\`
  * 中间目录 :`$(SolutionDir)Binary\$(ProjectName)\$(Platform)\$(Configuration)\Cache\`

### ThirdParty
&emsp;&emsp;所有的第三方库都要放到解决方案文件夹下的`ThirdParty`文件夹中。具体细节如下:
* 静态库:  
要在解决方案中的`ThirdParty`文件夹中添加现有项目。  
根据[BuildTarget](#BuildTarget)来设置项目的输出目录和中间目录。  
添加空白的`licenseheader`文件于库所在的文件夹中。
* 头文件库:  
要在解决方案中的`ThirdParty`文件夹中添加同名文件夹，并添加现有文件。  
添加空白的`licenseheader`文件于库所在的文件夹中。