﻿# 中控项目说明
## 一.简介
&emsp;&emsp;中控用来集中管理和控制计算机，基本功能有监控系统资源（包括CPU、
内存占用率，网络流量）和远程控制计算机（包括启动关闭选定的程序、启动、
关闭计算机等）。
## 二.项目组成
&emsp;&emsp;中控项目由客户端、服务端和应用代理三个程序组成。客户端是用户与中
控系统交互的工具，用户通过客户端监视和控制工作站。服务端主要负责资源监视、控制
指令的响应等。应用代理负责启动应用程序和屏幕截图。\
&emsp;&emsp;服务端以服务的形式运行在Windows系统上或者以Daemon的形式运行在
linux系统中。
## 三.编译开发环境
&emsp;&emsp;中控建立在ICE框架之上。客户端由Qt开发，支持Windows 7及以上操作
系统；服务端和应用代理由C#开发，支持Windows XP及以上操作系统（.Net Framework
4.0)，在Linux系统上由mono提供运行时支持。
## 四.安装
+ 客户端不需要安装，直接可运行（Windows系统需要安装VC14运行库）。
+ 服务端提供了安装脚本，在Windows下是批处理文件，在Linux下shell脚本。