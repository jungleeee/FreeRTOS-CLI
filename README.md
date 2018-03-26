# FreeRTOS-CLI

---
        author      : Jungle
        Creat Time  : 2018/3/26
        Mail        : yjxxx168@qq.com
        Last Change : 2018/3/26

---
###前言
```
        学习、总计、交流、进步！
```

测试环境为Keil 5.20 && STM32F405ZET6 && FreeRtos 10.0.0；晶振频率为12Mhz。
串口工具使用的SecureCRT，波特率为115200。
请根据实际情况修改后进行测试。

原创，转载请注明出处。

---
###介绍
在系统调试过此中，总是会有某些参数的临时修改，或者有时候调试需要临时打印一些信息，如果都需要重新修改代码、编译、下载这样一个过程就会显得十分麻烦。
而通过CLI接口，就可以通过调用程序中的接口去修改一些参数，或者临时打开一些预置的调试打印，避免反复修改程序的麻烦和偶尔粗心大意造成的修改不全面。
当然还可以通过一些控制接口，直接实现一些控制，方便调试。总之而言，CLI很大程度方便了程序调试过程。
FreeRTOS系统后续不做介绍，作者只是选择了一个系统平台来实现和展示CLI工具。只需要简单移植，就可以在其他系统中使用。

<!-- more -->

###CLI
FreeRTOS官网提供CLI的例程，作者自写CLI之前很认真的阅读过源码，并且有实际使用。但在使用过程中发现一点小小的不方便，于是便自己尝试修改，并于分享。
作者在使用FreeRTOS提供的CLI过程中，发现当系统中要使用的调试命令太多时，输入'help'后出现一大串命令介绍，你需要在这一大串命令中去找要使用的命令的使用格式，很麻烦。
所以在修改后实现了二级命令菜单，可以通过某些属性，将命令简单分类，原因之一。
另一原因是，大多数人在打印调试信息时，都会使用重新映射的'printf'，而它的执行过程是一直阻塞直到全部发送完成，当调式信息过多时，就会影响系统的实时性。
因此作者想要通过串口的DMA来完成调试信息的打印，而不使用前一种方式，其次为系统提供统一可控制的打印接口，可以随意开启/关闭某一分类的调试打印输出。

实现的源码在目录_Code/_APP下，源码很简单，就不做流程介绍了。主要说明几点注意事项(Notice)：

1、cli_task.c: function: cli_task_send(): line: 157-158:
    xSemaphoreTake(xSeriaSendCompleteBinary, portMAX_DELAY)//portMAX_DELAY, According to actual modificaton
    vTaskDelay(cli_MAX_TX_QUEUE_BUG_WAIT)//why delay? please view the header 'common.h' line 46-48

2、common.h: line 28-37, bit-field(位域)，baidu or google。
    if you need to add one bit, please reduce one bit on 'OTHER'

3、about submenu, please refer to the template in the file cli_register.c
