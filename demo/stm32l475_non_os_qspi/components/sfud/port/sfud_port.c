/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include <stdio.h>
#include <stm32l4xx_hal.h>
#include <stm32l4xx_hal_gpio.h>
#include <string.h>
#include <stdio.h>
 
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif/* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
    extern UART_HandleTypeDef huart1;
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
 
  return ch;
}

struct qspi_message
{
    /* instruction stage */
    struct
    {
        uint8_t content;
        uint8_t qspi_lines;
    } instruction;

    /* address and alternate_bytes stage */
    struct
    {
        uint32_t content;
        uint8_t size;
        uint8_t qspi_lines;
    } address, alternate_bytes;

    /* dummy_cycles stage */
    uint32_t dummy_cycles;

    /* data stage */
    struct
    {
        uint8_t *buf;
        uint32_t size;
        uint8_t qspi_lines;
    } data;
};

void sfud_log_info(const char *format, ...);
void qspi_send_cmd(struct qspi_message *message);
sfud_err qspi_send(const void *send_buf, size_t length);
sfud_err qspi_send_then_recv(const void *send_buf, size_t send_length, void *recv_buf, size_t recv_length);
sfud_flash *sfud_dev = NULL;
extern QSPI_HandleTypeDef hqspi;

typedef struct
{
    QSPI_HandleTypeDef *spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
} spi_user_data, *spi_user_data_t;

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

static void spi_lock(const sfud_spi *spi)
{
    __disable_irq();
}

static void spi_unlock(const sfud_spi *spi)
{
    __enable_irq();
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size)
{
    sfud_err result = SFUD_SUCCESS;

    spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;

    if (write_size)
    {
        SFUD_ASSERT(write_buf);
    }
    if (read_size)
    {
        SFUD_ASSERT(read_buf);
    }

    /* reset cs pin */
    if (spi_dev->cs_gpiox != NULL)
        HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_RESET);

    if (write_size && read_size)
    {
        /* read data */
        qspi_send_then_recv(write_buf, write_size, read_buf, read_size);
    }
    else if (write_size)
    {
        /* send data */
        qspi_send(write_buf, write_size);
    }

    /* set cs pin */
    if (spi_dev->cs_gpiox != NULL)
        HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_SET);

    return result;
}

/**
 * QSPI fast read data
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format, uint8_t *read_buf, size_t read_size)
{
    struct qspi_message message;
    sfud_err result = SFUD_SUCCESS;
    extern QSPI_HandleTypeDef hqspi;

    /* set message struct */
    message.instruction.content = qspi_read_cmd_format->instruction;
    message.instruction.qspi_lines = qspi_read_cmd_format->instruction_lines;

    message.address.content = addr;
    message.address.size = qspi_read_cmd_format->address_size;
    message.address.qspi_lines = qspi_read_cmd_format->address_lines;

    message.alternate_bytes.content = 0;
    message.alternate_bytes.size = 0;
    message.alternate_bytes.qspi_lines = 0;

    message.dummy_cycles = qspi_read_cmd_format->dummy_cycles;

    message.data.buf = read_buf;
    message.data.size = read_size;
    message.data.qspi_lines = qspi_read_cmd_format->data_lines;

    /* fast read data */
    qspi_send_cmd(&message);
    if (message.data.size != 0)
    {
        if (HAL_QSPI_Receive(&hqspi, read_buf, 5000) != HAL_OK)
        {
            sfud_log_info("qspi recv data failed(%d)!", hqspi.ErrorCode);
            hqspi.State = HAL_QSPI_STATE_READY;
            result = SFUD_ERR_READ;
        }
    }

    return result;
}

/* about 100 microsecond delay */
static void retry_delay_100us(void)
{
    uint32_t delay = 120;
    while (delay--);
}

static spi_user_data spi1 = { .spix = &hqspi, .cs_gpiox = NULL, .cs_gpio_pin = NULL };
sfud_err sfud_spi_port_init(sfud_flash *flash)
{
    sfud_err result = SFUD_SUCCESS;

    switch (flash->index)
    {
        case SFUD_W25_DEVICE_INDEX:
        {
            /* set the interfaces and data */
            flash->spi.wr = spi_write_read;
            flash->spi.qspi_read = qspi_read;
            flash->spi.lock = spi_lock;
            flash->spi.unlock = spi_unlock;
            flash->spi.user_data = &spi1;
            /* about 100 microsecond delay */
            flash->retry.delay = retry_delay_100us;
            /* adout 60 seconds timeout */
            flash->retry.times = 60 * 10000;

            break;
        }
    }

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}

/**
 * This function can send QSPI cmd.
 */
static void qspi_send_cmd(struct qspi_message *message)
{
    QSPI_CommandTypeDef Cmdhandler;

    Cmdhandler.Instruction = message->instruction.content;
    Cmdhandler.Address = message->address.content;
    Cmdhandler.DummyCycles = message->dummy_cycles;
    if (message->instruction.qspi_lines == 0)
    {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_NONE;
    }
    else if (message->instruction.qspi_lines == 1)
    {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    }
    else if (message->instruction.qspi_lines == 2)
    {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_2_LINES;
    }
    else if (message->instruction.qspi_lines == 4)
    {
        Cmdhandler.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    }
    if (message->address.qspi_lines == 0)
    {
        Cmdhandler.AddressMode = QSPI_ADDRESS_NONE;
    }
    else if (message->address.qspi_lines == 1)
    {
        Cmdhandler.AddressMode = QSPI_ADDRESS_1_LINE;
    }
    else if (message->address.qspi_lines == 2)
    {
        Cmdhandler.AddressMode = QSPI_ADDRESS_2_LINES;
    }
    else if (message->address.qspi_lines == 4)
    {
        Cmdhandler.AddressMode = QSPI_ADDRESS_4_LINES;
    }
    if (message->address.size == 24)
    {
        Cmdhandler.AddressSize = QSPI_ADDRESS_24_BITS;
    }
    else
    {
        Cmdhandler.AddressSize = QSPI_ADDRESS_32_BITS;
    }
    if (message->data.qspi_lines == 0)
    {
        Cmdhandler.DataMode = QSPI_DATA_NONE;
    }
    else if (message->data.qspi_lines == 1)
    {
        Cmdhandler.DataMode = QSPI_DATA_1_LINE;
    }
    else if (message->data.qspi_lines == 2)
    {
        Cmdhandler.DataMode = QSPI_DATA_2_LINES;
    }
    else if (message->data.qspi_lines == 4)
    {
        Cmdhandler.DataMode = QSPI_DATA_4_LINES;
    }

    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    Cmdhandler.NbData = message->data.size;
    HAL_QSPI_Command(&hqspi, &Cmdhandler, 5000);
}

/**
 * This function can read QSPI data.
 */
sfud_err qspi_send_then_recv(const void *send_buf, size_t send_length, void *recv_buf, size_t recv_length)
{
    assert_param(send_buf);
    assert_param(recv_buf);
    assert_param(send_length != 0);

    struct qspi_message message;
    unsigned char *ptr = (unsigned char *)send_buf;
    size_t count = 0;
    sfud_err result = SFUD_SUCCESS;

    message.instruction.content = ptr[0];
    message.instruction.qspi_lines = 1;
    count++;

    /* get address */
    if (send_length > 1)
    {
        if (send_length >= 4)
        {
            /* address size is 3 Byte */
            message.address.content = (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
            message.address.size = 24;
            count += 3;
        }
        else
        {
            return SFUD_ERR_READ;
        }
        message.address.qspi_lines = 1;
    }
    else
    {
        /* no address stage */
        message.address.content = 0 ;
        message.address.qspi_lines = 0;
        message.address.size = 0;
    }

    message.alternate_bytes.content = 0;
    message.alternate_bytes.size = 0;
    message.alternate_bytes.qspi_lines = 0;

    /* set dummy cycles */
    if (count != send_length)
    {
        message.dummy_cycles = (send_length - count) * 8;
    }
    else
    {
        message.dummy_cycles = 0;
    }

    /* set recv buf and recv size */
    message.data.buf = recv_buf;
    message.data.size = recv_length;
    message.data.qspi_lines = 1;

    qspi_send_cmd(&message);
    if (message.data.size != 0)
    {
        if (HAL_QSPI_Receive(&hqspi, recv_buf, 5000) != HAL_OK)
        {
            sfud_log_info("qspi recv data failed(%d)!", hqspi.ErrorCode);
            hqspi.State = HAL_QSPI_STATE_READY;
            result = SFUD_ERR_READ;
        }
    }

    return result;
}

/**
 * QSPI send data
 */
sfud_err qspi_send(const void *send_buf, size_t length)
{
    assert_param(send_buf);
    assert_param(length != 0);

    struct qspi_message message;
    char *ptr = (char *)send_buf;
    size_t  count = 0;
    sfud_err result = SFUD_SUCCESS;

    message.instruction.content = ptr[0];
    message.instruction.qspi_lines = 1;
    count++;

    /* get address */
    if (length > 1)
    {
        if (length >= 4)
        {
            /* address size is 3 Byte */
            message.address.content = (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
            message.address.size = 24;
            message.address.qspi_lines = 1;
            count += 3;
        }
        else
        {
            return SFUD_ERR_WRITE;
        }
    }
    else
    {
        /* no address stage */
        message.address.content = 0 ;
        message.address.qspi_lines = 0;
        message.address.size = 0;
    }

    message.alternate_bytes.content = 0;
    message.alternate_bytes.size = 0;
    message.alternate_bytes.qspi_lines = 0;

    message.dummy_cycles = 0;

    /* determine if there is data to send */
    if (length - count > 0)
    {
        message.data.qspi_lines = 1;
    }
    else
    {
        message.data.qspi_lines = 0;

    }

    /* set send buf and send size */
    message.data.buf = (uint8_t *)ptr + count;
    message.data.size = length - count;

    qspi_send_cmd(&message);
    if (message.data.size != 0)
    {
        if (HAL_QSPI_Transmit(&hqspi, message.data.buf, 5000) != HAL_OK)
        {
            sfud_log_info("qspi send data failed(%d)!", hqspi.ErrorCode);
            hqspi.State = HAL_QSPI_STATE_READY;
            result = SFUD_ERR_WRITE;
        }
    }

    return result;
}

/**
 * QSPI flash init
 */
sfud_err sfud_flash_init(char *spi_dev_name)
{
    char *spi_flash_dev_name_bak = NULL;
    extern sfud_err sfud_device_init(sfud_flash * flash);
    sfud_err result =  SFUD_SUCCESS;
    
    sfud_dev = (sfud_flash_t) malloc(sizeof(sfud_flash));
    if (sfud_dev == NULL)
    {
        sfud_log_info("low memory");
        return SFUD_ERR_NOT_FOUND;
    }
    memset(sfud_dev, 0, sizeof(sfud_flash));

    spi_flash_dev_name_bak = (char *) malloc(strlen(spi_dev_name) + 1);
    if (spi_flash_dev_name_bak == NULL)
    {
        sfud_log_info("low memory");
        return SFUD_ERR_NOT_FOUND;
    }
    strcpy(spi_flash_dev_name_bak, spi_dev_name);
    sfud_dev->name = spi_flash_dev_name_bak;

    if (sfud_device_init(sfud_dev) != SFUD_SUCCESS)
    {
        sfud_log_info("ERROR: SPI flash probe failed by SPI device %s.", spi_dev_name);
        result = SFUD_ERR_NOT_FOUND;
        goto error;
    }

    /* set qspi read mode */
    sfud_qspi_fast_read_enable(sfud_dev, 4);
    sfud_log_info("read instruction is %X", sfud_dev->read_cmd_format.instruction);

error:
    if (result == SFUD_ERR_NOT_FOUND && sfud_dev)
    {
        free(sfud_dev);
    }

    return result;
}
