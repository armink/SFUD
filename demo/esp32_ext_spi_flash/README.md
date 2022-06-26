# ESP32系列单片机 外部SPI FLASH demo

## 1、简介

基于ESP-IDF库通过SPI外设连接到Nor Flash。本demo使用合宙ESP32C3核心板和W25Q128测试，使用ESP-IDFv4.4分支。Flash接在合宙标明的SPI外设脚上，WP与HD拉高，其他引脚见`components/SFUD/sfud_port.c`标明。

![esp32c3-core](https://cdn.openluat-luatcommunity.openluat.com/images/20220215114919363_QQ截图20220215114724.png)

本demo包含测试和跑分：通过 `main/main.c` 的 `void sfud_demo(uint32_t addr, size_t size, uint8_t *data)` 方法来演示并测试第一个 Flash 设备的擦除、写入及读取功能，最后会将写入后数据读取出来，与写入前的数据进行对比，如果一致，则测试通过。通过`main/main.c`的`void sfud_benchmark()`函数跑分。跑分仿照rtt部分demo编写。

## 1.1、使用方法

在shell中加载idf的环境变量后执行`idf.py build flash moitor`后即可。跑分中途会有阻塞擦写导致未喂狗警告，实际大数据量擦写时需要注意及时喂狗。

```shell
--- idf_monitor on /dev/cu.wchusbserial54580021441 115200 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
ESP-ROM:esp32c3-api1-20210207
Build:Feb  7 2021
rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:2
load:0x3fcd6100,len:0x16c8
load:0x403ce000,len:0x930
load:0x403d0000,len:0x2d50
entry 0x403ce000
I (31) boot: ESP-IDF v4.4-263-g000d3823bb-dirty 2nd stage bootloader
I (31) boot: compile time 14:01:18
I (31) boot: chip revision: 3
I (35) boot.esp32c3: SPI Speed      : 40MHz
I (39) boot.esp32c3: SPI Mode       : DIO
I (44) boot.esp32c3: SPI Flash Size : 2MB
I (49) boot: Enabling RNG early entropy source...
I (54) boot: Partition Table:
I (58) boot: ## Label            Usage          Type ST Offset   Length
I (65) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (73) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (80) boot:  2 factory          factory app      00 00 00010000 00100000
I (87) boot: End of partition table
I (92) esp_image: segment 0: paddr=00010020 vaddr=3c020020 size=09a58h ( 39512) map
I (109) esp_image: segment 1: paddr=00019a80 vaddr=3fc8bc00 size=014e4h (  5348) load
I (110) esp_image: segment 2: paddr=0001af6c vaddr=40380000 size=050ach ( 20652) load
I (122) esp_image: segment 3: paddr=00020020 vaddr=42000020 size=1a17ch (106876) map
I (148) esp_image: segment 4: paddr=0003a1a4 vaddr=403850ac size=0699ch ( 27036) load
I (155) esp_image: segment 5: paddr=00040b48 vaddr=50000010 size=00010h (    16) load
I (159) boot: Loaded app from partition at offset 0x10000
I (160) boot: Disabling RNG early entropy source...
I (177) cpu_start: Pro cpu up.
I (186) cpu_start: Pro cpu start user code
I (186) cpu_start: cpu freq: 160000000
I (186) cpu_start: Application information:
I (189) cpu_start: Project name:     main
I (194) cpu_start: App version:      0991166-dirty
I (199) cpu_start: Compile time:     Jun 26 2022 14:01:12
I (205) cpu_start: ELF file SHA256:  be25db972f3ec439...
I (211) cpu_start: ESP-IDF:          v4.4-263-g000d3823bb-dirty
I (218) heap_init: Initializing. RAM available for dynamic allocation:
I (225) heap_init: At 3FC8E580 len 00031A80 (198 KiB): DRAM
I (231) heap_init: At 3FCC0000 len 0001F060 (124 KiB): STACK/DRAM
I (238) heap_init: At 50000020 len 00001FE0 (7 KiB): RTCRAM
I (245) spi_flash: detected chip: mxic
I (249) spi_flash: flash io: dio
W (253) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (266) cpu_start: Starting scheduler.
I (271) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud.c:116)Start initialize Serial Flash Universal Driver(SFUD) V1.1.0.
I (281) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud.c:117)You can get the latest version on https://github.com/armink/SFUD .
I (291) SFUD: Initializing SPI bus...
I (301) gpio: GPIO[7]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (301) SFUD: Creating Semaphore for SFUD...
I (311) SFUD: Registering flash in SFUD...
I (311) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud.c:866)The flash device manufacturer ID is 0xEF, memory type ID is 0x40, capacity ID is 0x18.
I (331) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:132)Check SFDP header is OK. The reversion is V1.5, NPN is 0.
I (341) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:175)Check JEDEC basic flash parameter header is OK. The table id is 0, reversion is V1.5, length is 16, parameter table pointer is 0x000080.
I (361) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:203)JEDEC basic flash parameter table info:
I (371) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:204)MSB-LSB  3    2    1    0
I (381) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0001] 0xFF 0xF9 0x20 0xE5
I (391) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0002] 0x07 0xFF 0xFF 0xFF
I (401) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0003] 0x6B 0x08 0xEB 0x44
I (411) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0004] 0xBB 0x42 0x3B 0x08
I (421) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0005] 0xFF 0xFF 0xFF 0xFE
I (431) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0006] 0x00 0x00 0xFF 0xFF
I (441) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0007] 0xEB 0x40 0xFF 0xFF
I (451) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0008] 0x52 0x0F 0x20 0x0C
I (461) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:207)[0009] 0x00 0x00 0xD8 0x10
I (471) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:215)4 KB Erase is supported throughout the device. Command is 0x20.
I (481) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:234)Write granularity is 64 bytes or larger.
I (491) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:245)Target flash status register is non-volatile.
I (511) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:271)3-Byte only addressing.
I (521) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:305)Capacity is 16777216 Bytes.
I (531) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:312)Flash device supports 4KB block erase. Command is 0x20.
I (541) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:312)Flash device supports 32KB block erase. Command is 0x52.
I (551) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud_sfdp.c:312)Flash device supports 64KB block erase. Command is 0xD8.
I (561) SFUD: Find a Winbond flash chip. Size is 16777216 bytes.
I (571) SFUD: (/Users/<username>/Documents/GitHub/SFUD/sfud/src/sfud.c:844)Flash device reset success.
I (581) SFUD: W25Q128 flash device is initialize success.
I (611) SFUD_DEMO: Erase the W25Q128 flash data finish. Start from 0x00000000, size is 1024.
I (621) SFUD_DEMO: Write the W25Q128 flash data finish. Start from 0x00000000, size is 1024.
I (621) SFUD_DEMO: Read the W25Q128 flash data success. Start from 0x00000000, size is 1024. The data is:
Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
[00000000] 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 
[00000010] 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 
[00000020] 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 
[00000030] 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 
[00000040] 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 
[00000050] 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 
[00000060] 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 
[00000070] 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 
[00000080] 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 
[00000090] 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F 
[000000A0] A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF 
[000000B0] B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF 
[000000C0] C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF 
[000000D0] D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF 
[000000E0] E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF 
[000000F0] F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF 
[00000100] 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 
[00000110] 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 
[00000120] 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 
[00000130] 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 
[00000140] 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 
[00000150] 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 
[00000160] 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 
[00000170] 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 
[00000180] 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 
[00000190] 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F 
[000001A0] A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF 
[000001B0] B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF 
[000001C0] C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF 
[000001D0] D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF 
[000001E0] E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF 
[000001F0] F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF 
[00000200] 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 
[00000210] 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 
[00000220] 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 
[00000230] 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 
[00000240] 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 
[00000250] 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 
[00000260] 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 
[00000270] 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 
[00000280] 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 
[00000290] 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F 
[000002A0] A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF 
[000002B0] B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF 
[000002C0] C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF 
[000002D0] D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF 
[000002E0] E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF 
[000002F0] F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF 
[00000300] 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 
[00000310] 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 
[00000320] 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 
[00000330] 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 
[00000340] 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 
[00000350] 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 
[00000360] 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 
[00000370] 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 
[00000380] 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 
[00000390] 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F 
[000003A0] A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF 
[000003B0] B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF 
[000003C0] C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF 
[000003D0] D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF 
[000003E0] E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF 
[000003F0] F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF 

I (981) SFUD_DEMO: The W25Q128 flash test is success.
I (991) SFUD_BENCHMARK: Erasing the W25Q128 16777216 bytes data, waiting...
E (5271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (5271) task_wdt:  - IDLE (CPU 0)
E (5271) task_wdt: Tasks currently running:
E (5271) task_wdt: CPU 0: main
E (10271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (10271) task_wdt:  - IDLE (CPU 0)
E (10271) task_wdt: Tasks currently running:
E (10271) task_wdt: CPU 0: main
E (15271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (15271) task_wdt:  - IDLE (CPU 0)
E (15271) task_wdt: Tasks currently running:
E (15271) task_wdt: CPU 0: main
E (20271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (20271) task_wdt:  - IDLE (CPU 0)
E (20271) task_wdt: Tasks currently running:
E (20271) task_wdt: CPU 0: main
E (25271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (25271) task_wdt:  - IDLE (CPU 0)
E (25271) task_wdt: Tasks currently running:
E (25271) task_wdt: CPU 0: main
I (28691) SFUD_BENCHMARK: Erase benchmark success, total time: 27.697S.
I (28691) SFUD_BENCHMARK: Writing the W25Q128 16777216 bytes data, waiting...
E (30271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (30271) task_wdt:  - IDLE (CPU 0)
E (30271) task_wdt: Tasks currently running:
E (30271) task_wdt: CPU 0: main
E (35271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (35271) task_wdt:  - IDLE (CPU 0)
E (35271) task_wdt: Tasks currently running:
E (35271) task_wdt: CPU 0: main
E (40271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (40271) task_wdt:  - IDLE (CPU 0)
E (40271) task_wdt: Tasks currently running:
E (40271) task_wdt: CPU 0: main
E (45271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (45271) task_wdt:  - IDLE (CPU 0)
E (45271) task_wdt: Tasks currently running:
E (45271) task_wdt: CPU 0: main
E (50271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (50271) task_wdt:  - IDLE (CPU 0)
E (50271) task_wdt: Tasks currently running:
E (50271) task_wdt: CPU 0: main
E (55271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (55271) task_wdt:  - IDLE (CPU 0)
E (55271) task_wdt: Tasks currently running:
E (55271) task_wdt: CPU 0: main
E (60271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (60271) task_wdt:  - IDLE (CPU 0)
E (60271) task_wdt: Tasks currently running:
E (60271) task_wdt: CPU 0: main
E (65271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (65271) task_wdt:  - IDLE (CPU 0)
E (65271) task_wdt: Tasks currently running:
E (65271) task_wdt: CPU 0: main
I (68431) SFUD_BENCHMARK: Write benchmark success, total time: 39.731S.
I (68431) SFUD_BENCHMARK: Reading the W25Q128 16777216 bytes data, waiting...
E (70271) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (70271) task_wdt:  - IDLE (CPU 0)
E (70271) task_wdt: Tasks currently running:
E (70271) task_wdt: CPU 0: main
I (72061) SFUD_BENCHMARK: Read benchmark success, total time: 3.632S.
```

## 2、文件夹说明

`components/SFUD`下的文件为移植参考。在`components`文件夹下且包含`CMakeList.txt`文件的组件idf会自动加入编译。
