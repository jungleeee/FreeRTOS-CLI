# FreeRTOS-CLI

---
        author      : Jungle
        Creat Time  : 2018/3/26
        Mail        : yjxxx168@qq.com
        Last Change : 2018/3/26

---
### 前言

```
        学习、总结、交流、进步！
```

测试环境为Keil 5.20 && STM32F405ZET6 && FreeRtos 10.0.0；晶振频率为12Mhz。<br />
串口工具使用的SecureCRT，波特率为115200。<br />
请根据实际情况修改后进行测试。<br />

原创，转载请注明出处。<br />

---
### 介绍

在系统调试过此中，总是会有某些参数的临时修改，或者有时候调试需要临时打印一些信息，如果都需要重新修改代码、编译、下载这样一个过程就会显得十分麻烦。<br />
而通过CLI接口，就可以通过调用程序中的接口去修改一些参数，或者临时打开一些预置的调试打印，避免反复修改程序的麻烦和偶尔粗心大意造成的修改不全面。<br />
当然还可以通过一些控制接口，直接实现一些控制，方便调试。总之而言，CLI很大程度方便了程序调试过程。<br />
FreeRTOS系统后续不做介绍，作者只是选择了一个系统平台来实现和展示CLI工具。只需要简单移植，就可以在其他系统中使用。<br />

<!-- more -->

---
### CLI

FreeRTOS官网提供CLI的例程，作者自写CLI之前很认真的阅读过源码，并且有实际使用。但在使用过程中发现一点小小的不方便，于是便自己尝试修改，并于分享。<br />
作者在使用FreeRTOS提供的CLI过程中，发现当系统中要使用的调试命令太多时，输入'help'后出现一大串命令介绍，你需要在这一大串命令中去找要使用的命令的使用格式，很麻烦。<br />
所以在修改后实现了二级命令菜单，可以通过某些属性，将命令简单分类，原因之一。<br />
另一原因是，大多数人在打印调试信息时，都会使用重新映射的'printf'，而它的执行过程是一直阻塞直到全部发送完成，当调式信息过多时，就会影响系统的实时性。<br />
因此作者想要通过串口的DMA来完成调试信息的打印，而不使用前一种方式，其次为系统提供统一可控制的打印接口，可以随意开启/关闭某一分类的调试打印输出。<br />

实现的源码在目录_Code/_APP下，源码很简单，就不做流程介绍了。主要说明几点注意事项(Notice)：<br />

1、usart.c: line: 127-129:<br />
    DMA_Cmd(..., DISABLE);//must disable
    这里需要默认关闭，不然会产生一次中断，产生信号量，导致异常。

2、common.h: line 28-37, bit-field(位域)，baidu or google。<br />
    if you need to add one bit, please reduce one bit on 'OTHER'<br />
    C语言位域，定义。

3、about submenu, please refer to the template in the file cli_register.c<br />
    CLI的次级菜单，参考源码中的定义方式。有举例

4、The critical area problem is not considered in the program, and it is not recommended to use in interrupts, and high frequency is not recommended.
    打印接口，建议不要再中断中使用以及频率太高的地方使用，因为队列长度有限，消息太多会丢包。

5、Provide the program under the ucos system.
    我有移植到UcOS III 系统中使用，两个系统队列的内存分配方式不同，可以仔细看看。

6、Use caution if you don't do a lot of testing.
    如果在项目中使用的话，请小心使用，多测试其稳定信和可靠性。

[源码链接](https://github.com/jungleeee/FreeRTOS-CLI)

---
### 参考

1、[FreeRTOS+CLI](https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/Download_FreeRTOS_Plus_CLI.shtml)
