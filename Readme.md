# 自己开发rg35xx应用程序模版工程

本项目为rg35xx应用程序模版工程，用于开发rg35xx简单的应用程序。

![img](./doc/img.png)

## 如何编译

1. 使用docker获取编译环境

```shell
docker pull miyoocfw/toolchain-shared-uclibc:latest
```

2. 编译

```shell
./make.sh
```

## 如何添加自己的源文件

1. 在src目录下添加自己的源文件，例如test.c

2. 修改src/Makefile，添加自己的源文件，主要修改以下三个变量

```makefile
TARGET = main
C_SOURCES =  \
	${wildcard $(SRCS_DIR)/*.c}
CPP_SOURCES =  \
	${wildcard $(SRCS_DIR)/*.cpp}
```
