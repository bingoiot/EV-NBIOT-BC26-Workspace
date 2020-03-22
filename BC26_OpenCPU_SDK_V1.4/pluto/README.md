# EV-ESP8266-Workspace 开发程序模板  

![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/ev-board.png)  

## 程序模板包含以下软件  

* cygwin window 开发工具包，内含xtensa芯片windows平台编译工具  

* flash_download_tools_v3.6.5 esp8266 windows平台程序烧录工具  

* ESP8266_NONOS_SDK-3.0 最新版本SDK。

	本程序模板也可搭建在linux平台上开发，需要自行下载linux平台编译工具。  

## 程序模块使用方法:  

* 下载并安装最新JDK。  
* 下载并安装最新eclipse。  
* 打开eclipse开发环境并导入现有工程(Existing Projects into Workspace):  
![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/1.png)  
![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/2.png)  
* 右键点击工程选择属性配置选项,配置SDK_PATH环境变量,这里特别注意所有目录分隔符使用双反斜杠"\\"：
![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/3.png)  
![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/4.png)  
![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/5.png)  
* 接下来就可以点击编译按钮进行编译了。  
* 如果工程中的pluto目录无法在eclipse打开，可以右键点击目录属性重新编辑正确的路径，也可以删除原目录重新添加:  
![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/6.png) 
![image](https://github.com/bingoiot/EV-ESP8266-Workspace/blob/master/img/7.png) 