# stm32f2xx RT-Thread Demo

---

## 1、简介

该 Demo 使用 RT-Thread 开发团队的 [ART-WiFi](http://www.rt-thread.org/product/5.html) 开发板，其板载 `STM32F205RG` 控制器及华邦的 `W25Q64` SPI Flash，并引出一路 SPI 总线，所以还可以外接一路 SPI Flash。如图所示
![RTT_ART_WiFi](http://git.oschina.net/Armink/SFUD/raw/master/docs/zh/images/RTT_ART_WiFi.jpg)

> 注意：如果没有 ART_WiFi 开发板，只要控制器相似，同样可以使用此 Demo 。

### 1.1、使用方法

- Flash 设备：在 `components/sfud/inc/sfud_cfg.h` 中的 `SFUD_FLASH_DEVICE_TABLE` 宏定义有定义两个 Flash 设备。第一个为板载的 `W25Q64` ，第二个是我自己外接的 `W25Q16`，如果没有外接或外接的是其他 Flash ，可以参考 [首页文档中介绍的 Flash 设备表配置方法](https://github.com/armink/SFUD#234-flash-设备表) 进行修改。
- SFUD 初始化：在 `app/src/app_task.c` 的 `sys_init_thread` 线程中完成对 SFUD 的初始化。
- 连接终端：初始化完成后可以将电脑中的终端与开发板连接（串口1），在终端中输入如下命令，完成测试过程。

#### 1.1.1 Flash 操作命令

```
Flash <read|write|erase|benchmark> <device_index> <addr> <size>
```

例如：

- 1、读取第一个 Flash 设备的从地址 0 开始，共计 64 字节数据，输入下面的命令：

```
msh >flash read 0 0 64
```

- 2、写数据从地址 10 开始，共计 5 字节数据到第二个 Flash 设备，输入下面的命令：

```
msh >flash write 0 10 5 1 2 3 4 5 
```

- 3、擦除从地址 0 开始，共计 8192 字节数据到第一个 Flash 设备，输入下面的命令：

```
msh >flash erase 0 0 8192
```

- 4、测试第一个 Flash 设备全片（共计： 8388608 字节）的性能，命令及结果如下：

```
msh >flash benchmark 0 0 8388608
Erasing the W25Q64 8388608 bytes data, waiting...
Erase benchmark success, total time: 20.591S.
Writing the W25Q64 8388608 bytes data, waiting...
Write benchmark success, total time: 32.768S.
Reading the W25Q64 8388608 bytes data, waiting...
Read benchmark success, total time: 16.129S.
```

更多性能测试结果见： [`/docs/zh/benchmark.txt`](https://github.com/armink/SFUD/blob/master/docs/zh/benchmark.txt)

#### 1.1.2 读取/修改 Flash 状态命令

```
flash_status <read|write> <device_index> [<1:volatile|0:non-volatile> <status>]
```

## 2、文件（夹）说明

- `components\sfud\port\sfud_port.c` 移植参考文件
- `components\sfud\inc\sfud_cfg.c` 库配置文件

`RVMDK` 下为Keil工程文件（后期加入）

`EWARM` 下为IAR工程文件