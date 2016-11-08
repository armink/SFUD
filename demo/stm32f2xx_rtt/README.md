# stm32f2xx RT-Thread Demo

---

## 1、简介

该 Demo 使用 RT-Thread 开发团队的 [ART-WiFi](http://www.rt-thread.org/product/5.html) 开发板，其板载 `STM32F205RG` 控制器及华邦的 `W25Q64` SPI Flash，并引出一路 SPI 总线，所以还可以外接一路 SPI Flash。如图所示
![RTT_ART_WiFi](http://git.oschina.net/Armink/SFUD/raw/master/docs/zh/images/RTT_ART_WiFi.jpg)

> 注意：如果没有 ART_WiFi 开发板，只要控制器相似，同样可以使用此 Demo 。

### 1.1、使用方法

- Flash 设备： RT-Thread SPI 设备 `spi10` 连接的为板载的 `W25Q64` ，`spi30` SPI 设备连接的是我自己外接的 `W25Q128`。
- SFUD 初始化：在 `app/src/app_task.c` 的 `sys_init_thread` 线程中完成对串行 Flash 的初始化。
- 连接终端：初始化完成后可以将电脑中的终端与开发板连接（串口1），在终端中输入如下命令，完成测试过程。

#### 1.1.1 Flash 操作命令

```
msh >sf
Usage:
sf probe [spi_device]           - probe and init SPI flash by given 'spi_device'
sf read addr size               - read 'size' bytes starting at 'addr'
sf write addr data1 ... dataN   - write some bytes 'data' to flash starting at 'addr'
sf erase addr size              - erase 'size' bytes starting at 'addr'
sf status [<volatile> <status>] - read or write '1:volatile|0:non-volatile' 'status'
sf bench                        - full chip benchmark. DANGER: It will erase full chip!
```

例如：

- 1、探测并选择 SPI 设备名称为 `spi10` 所连接 Flash 设备进行操作，输入下面的命令。选择后，接下来的操作都将针对此 Flash 设备：

```
msh >sf probe spi10
[SFUD]Find a Winbond flash chip. Size is 8388608 bytes.
[SFUD]sf_cmd flash device is initialize success.
8 MB sf_cmd is current selected device.
```

- 2、读取从地址 0 开始，共计 64 字节数据，输入下面的命令：

```
msh >sf read 0 64
Read the W25Q64 flash data success. Start from 0x00000000, size is 64. The data is:
Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
[00000000] 54 00 00 00 90 8F E3 A7 69 61 70 5F 6E 65 65 64 
[00000010] 5F 63 6F 70 79 5F 61 70 70 3D 30 00 69 61 70 5F 
[00000020] 63 6F 70 79 5F 61 70 70 5F 73 69 7A 65 3D 30 00 
[00000030] 73 74 6F 70 5F 69 6E 5F 62 6F 6F 74 6C 6F 61 64 
```

- 3、写数据从地址 10 开始，共计 5 字节数据，输入下面的命令：

```
msh >sf write 10 1 2 3 4 5
Write the W25Q64 flash data success. Start from 0x0000000A, size is 5.
Write data: 1 2 3 4 5 .
```

- 4、擦除从地址 0 开始，共计 8192 字节数据，输入下面的命令：

```
msh >sf erase 0 8192
Erase the W25Q64 flash data success. Start from 0x00000000, size is 8192
```

- 5、读取/修改 Flash 状态寄存器

读取 Flash 状态寄存器当前状态

```
msh >sf status
The W25Q64 flash status register current value is 0x00.
```

修改 Flash 当前状态为 `0x1C` ,特性为 `易闪失（掉电丢失）` 的
```
msh >sf status 1 28
Write the W25Q64 flash status register to 0x1C success.
```

- 6、测试 Flash 全片的性能，命令及结果如下：

```
msh >sf bench yes
Erasing the W25Q64 8388608 bytes data, waiting...
Erase benchmark success, total time: 20.591S.
Writing the W25Q64 8388608 bytes data, waiting...
Write benchmark success, total time: 32.768S.
Reading the W25Q64 8388608 bytes data, waiting...
Read benchmark success, total time: 16.129S.
```

更多性能测试结果见： [`/docs/zh/benchmark.txt`](https://github.com/armink/SFUD/blob/master/docs/zh/benchmark.txt)

## 2、文件（夹）说明

`RVMDK` 下为Keil工程文件（后期加入）

`EWARM` 下为IAR工程文件

## 3、配置方法

在 `rtconfig.h` 可增加如下配置信息，每个配置的功能详见 [`rt-thread/components/drivers/spi/sfud/inc/sfud_cfg.h`](https://github.com/RT-Thread/rt-thread/blob/master/components/drivers/spi/sfud/inc/sfud_cfg.h)

``` C
#define RT_USING_SFUD                      //必选
#define RT_DEBUG_SFUD                  1   //可选
#define RT_SFUD_USING_SFDP                 //可选
#define RT_SFUD_USING_FLASH_INFO_TABLE     //可选
```