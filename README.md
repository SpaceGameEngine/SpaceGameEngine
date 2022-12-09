# SpaceGameEngine

## Cpp Version
Use Cpp20

## CI Matrix

We use [Azure Pipeline](https://azure.microsoft.com/en-us/services/devops/pipelines/) as our CI service.

<table>
  <tr>
    <th>OS</th>
    <th>Toolchain</th>
    <th>Config</th>
    <th>Arch</th>
    <th>Status</th>
  </tr>
  <tr>
    <td rowspan="8">Windows 10</td>
    <td rowspan="4">Visual Studio 2019</td>
    <td rowspan="2">Debug</td>
    <td>x64</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2019_x64_Debug"></td>
  </tr>
  <tr>
    <td>x86</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2019_x86_Debug"></td>
  </tr>
  <tr>
    <td rowspan="2">Release</td>
    <td>x64</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2019_x64_Release"></td>
  </tr>
  <tr>
    <td>x86</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2019_x86_Release"></td>
  </tr>
  <tr>
    <td rowspan="4">Visual Studio 2022</td>
    <td rowspan="2">Debug</td>
    <td>x64</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2022_x64_Debug"></td>
  </tr>
  <tr>
    <td>x86</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2022_x86_Debug"></td>
  </tr>
  <tr>
    <td rowspan="2">Release</td>
    <td>x64</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2022_x64_Release"></td>
  </tr>
  <tr>
    <td>x86</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Windows?branchName=master&jobName=VS2022_x86_Release"></td>
  </tr>
  <tr>
    <td rowspan="4">Ubuntu 16.04</td>
    <td rowspan="4">G++ 11</td>
    <td colspan="2">Debug</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Ubuntu?branchName=master&jobName=Ubuntu_Debug"></td>
  </tr>
  <tr>
    <td colspan="2">Release</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Ubuntu?branchName=master&jobName=Ubuntu_Release"></td>
  </tr>
  <tr>
    <td colspan="2">RelWithDebInfo</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Ubuntu?branchName=master&jobName=Ubuntu_RelWithDebInfo"></td>
  </tr>
  <tr>
    <td colspan="2">MinSizeRel</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.Ubuntu?branchName=master&jobName=Ubuntu_MinSizeRel"></td>
  </tr>
  <tr>
    <td rowspan="4">MacOS 10.14</td>
    <td rowspan="4">XCode 10</td>
    <td colspan="2">Debug</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.MacOS?branchName=master&jobName=MacOS_Debug"></td>
  </tr>
  <tr>
    <td colspan="2">Release</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.MacOS?branchName=master&jobName=MacOS_Release"></td>
  </tr>
  <tr>
    <td colspan="2">RelWithDebInfo</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.MacOS?branchName=master&jobName=MacOS_RelWithDebInfo"></td>
  </tr>
  <tr>
    <td colspan="2">MinSizeRel</td>
    <td><img src="https://dev.azure.com/xsun2001/SpaceGameEngine/_apis/build/status/SpaceGameEngine.MacOS?branchName=master&jobName=MacOS_MinSizeRel"></td>
  </tr>
</table>

## User Guide

### Installation
```bash
git clone git@github.com:SpaceGameEngine/SpaceGameEngine.git
cd SpaceGameEngine
# windows
powershell ./setup.ps1
# unix
sh ./setup.sh
```

### Test

run test projects like ./Test/TestXXX...

must set the working directory to this location(`$(SolutionDir)` in Visual Studio)
before running the test program
