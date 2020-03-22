# EV-NBIOT-BC26-Workspace 开发程序模板  

![image](https://github.com/bingoiot/EV-NBIOT-BC26-Workspace/blob/master/img/ev-board.png)  

* 如果你对我们的开发板有兴趣可以通过此链接购买:https://shop589439611.taobao.com/ 

## 程序模板包含以下软件  

* cygwin window 开发工具包，内含MT2625芯片windows平台编译工具  

* FlashTool windows平台程序烧录工具  

* BC26_OpenCPU_SDK_V1.4 最新版本SDK。

	本程序模板也可搭建在linux平台上开发，需要自行下载linux平台编译工具。  

## 程序模块使用方法:  

* 下载并安装最新JDK。  
* 下载并安装最新eclipse。  
* 打开eclipse开发环境并导入现有工程(Existing Projects into Workspace):  
* 编辑代码 
* 接下来就可以点击编译按钮进行编译了: 
* 双击打开EV-NBIOT-BC26-Workspace\BC26_OpenCPU_SDK_V1.2\MS-DOS快捷方式
* 在DOS命令窗口执行make clean 清除历史编译, make new 编译 
* 编译生成的文件在EV-NBIOT-BC26-Workspace\BC26_OpenCPU_SDK_V1.2\build\gcc下。 
* EV-NBIOT-BC26-Workspace\BC26NBR01A05_OCPU下是内核文件,新买的模块需要先下载内核文件再烧写用户程序!  
* 更多文档请参考EV-NBIOT-BC26-Workspace\BC26_OpenCPU_SDK_V1.2\docs 
* 在开发过程中请参考阅读Quectel_BC26-OpenCPU_User_Guide_V1.0.pdf修改工程  

