﻿# 中控项目说明

## 一.简介

&emsp;&emsp;中控主要用来集中管理和控制计算机，基本功能有监控系统资源（包括CPU、
内存占用率，网络流量）和远程控制计算机（包括启动关闭选定的程序、启动、
关闭计算机等）。是基于ZeroC ICE框架构建的一套远程控管系统。

## 二.项目组成

### 客户端

客户端主要功能是为用户控制、监视工作站提供GUI,用户可以通过图形化界面远程监控和操作工作站。主要用Qt（界面）和标准C++,通信部分基于ICE框架开发的本地程序。支持Windows和Linux（包括麒麟）操作系统。在Windows下用Qt+VS2017开发编译,在Linux（包括麒麟）下用Qt和GCC编译生成Linux系统本地程序。

### 监控服务程序

监控服务程序主要功能是收集所在工作站资源利用情况,包括CPU,内存占用率和网络流量监视,以及响应客户端控制请求,完成对工作站的控制功能。主要的控制功能包括：

1. 启动,关闭程序
2. 传输文件
3. 关闭或重启工作站
4. 截取屏幕画面

监控服务程序基于C#开发,支持运行在Windows和Linux（包括麒麟）操作系统,Windows下需要安装.Net Framework 4.0以上运行库,Linux系统下需要安装Mono运行库。

### 应用代理

应用代理主要功能是响应监控服务程序命令,在用户态（Level 5）启动程序。开发环境与监控服务程序一致。

### 工作站运行日志记录程序

主要功能是记录监控服务程序发来的工作站资源利用情况到MySql数据库中,以便以后分析。主要记录了工作站基本情况（内存大小,网络配置等）,开关机时间,CPU、内存占用率，以及应用程序启停。由C#开发,支持运行在.Net Framework 4.5和Mono环境中。

### 工作站运行日志查看Web

是一套由dotNetCore 2.0 和 Angular 5.X 前端组成的单页面（SAP）Web程序,提供了图形化资源显示功能,主要是方便查看日志记录程序记录的数据。可以部署到IIS7（Windows）或nginx（Linux）服务器上,或直接运行。也可以部署到Docker上。

