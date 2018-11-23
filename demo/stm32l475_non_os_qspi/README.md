# stm32L475 QSPI 裸机 Demo

---

## 1、简介

Demo 提供的工程是基于 STM32L475 的，通过 QSPI 外设连接 W25Q128B FLASH，Demo 里的 QSPI 读取数据使用的是四线快读指令 - Fast Read Quad I/O（0xEB）。

通过 `Src\main.c` 的 `void sfud_demo(uint32_t addr, size_t size, uint8_t *data)` 方法来演示并测试第一个 Flash 设备的擦除、写入及读取功能，最后会将写入后数据读取出来，与写入前的数据进行对比，如果一致，则测试通过。

### 1.1、使用方法

- 1、打开电脑的终端与Demo的串口1进行连接，串口配置 115200 8 1 N
- 2、下载并运行 Demo ，此时可查看到类似如下的打印日志信息

```
[SFUD](..\..\..\sfud\src\sfud.c:116) Start initialize Serial Flash Universal Driver(SFUD) V1.1.0.
[SFUD](..\..\..\sfud\src\sfud.c:117) You can get the latest version on https://github.com/armink/SFUD .
[SFUD](..\..\..\sfud\src\sfud.c:858) The flash device manufacturer ID is 0xEF, memory type ID is 0x40, capacity ID is 0x17.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:131) Check SFDP header is OK. The reversion is V1.5, NPN is 0.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:173) Check JEDEC basic flash parameter header is OK. The table id is 0, reversion is V1.5, length is 16, parameter table pointer is 0x000080.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:203) JEDEC basic flash parameter table info:
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:204) MSB-LSB  3    2    1    0
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0001] 0xFF 0xF9 0x20 0xE5
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0002] 0x03 0xFF 0xFF 0xFF
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0003] 0x6B 0x08 0xEB 0x44
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0004] 0xBB 0x42 0x3B 0x08
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0005] 0xFF 0xFF 0xFF 0xFE
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0006] 0x00 0x00 0xFF 0xFF
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0007] 0xEB 0x40 0xFF 0xFF
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0008] 0x52 0x0F 0x20 0x0C
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:206) [0009] 0x00 0x00 0xD8 0x10
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:215) 4 KB Erase is supported throughout the device. Command is 0x20.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:234) Write granularity is 64 bytes or larger.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:245) Target flash status register is non-volatile.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:271) 3-Byte only addressing.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:305) Capacity is 8388608 Bytes.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:311) Flash device supports 4KB block erase. Command is 0x20.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:311) Flash device supports 32KB block erase. Command is 0x52.
[SFUD](..\..\..\sfud\src\sfud_sfdp.c:311) Flash device supports 64KB block erase. Command is 0xD8.
[SFUD]Find a Winbond flash chip. Size is 8388608 bytes.
[SFUD](..\..\..\sfud\src\sfud.c:837) Flash device reset success.
[SFUD]W25Q128B flash device is initialize success.
Erase the W25Q128B flash data finish. Start from 0x00000000, size is 1024.
Write the W25Q128B flash data finish. Start from 0x00000000, size is 1024.
Read the W25Q128B flash data success. Start from 0x00000000, size is 1024. The data is:
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

The W25Q128B flash test is success.

```

## 2、文件（夹）说明

`components\sfud\port\sfud_port.c` 移植参考文件

`MDK-ARM` 下为Keil工程文件

`EWARM` 下为IAR工程文件