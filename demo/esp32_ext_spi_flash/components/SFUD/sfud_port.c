/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
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
#include <esp_log.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <hal/gpio_types.h>
#include <driver/gpio.h>
#include <hal/spi_types.h>
#include <inttypes.h>
#include "esp_timer.h"

#define FLASH_MISO_Pin  10
#define FLASH_MOSI_Pin  3
#define FLASH_SCLK_Pin  2
#define FLASH_CS_Pin    7

static const char *TAG = "SFUD";

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

typedef struct {
    spi_device_handle_t spix;
} spi_user_data_t;

static spi_user_data_t flash_spi_handle = {
        .spix = NULL
};

static SemaphoreHandle_t s_lock = NULL;

static inline void spi_lock(const sfud_spi *spi) {
    xSemaphoreTake(s_lock, portMAX_DELAY);
}

static inline void spi_unlock(const sfud_spi *spi) {
    xSemaphoreGive(s_lock);
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    spi_device_handle_t spi_handle = ((spi_user_data_t *) spi->user_data)->spix;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

    gpio_set_level(FLASH_CS_Pin, 0);

    if (write_size && read_size) {
        spi_transaction_t t = {
                .length = 8 * write_size,
                .tx_buffer = write_buf
        };
        if (ESP_OK != spi_device_polling_transmit(spi_handle, &t)) {
            result = SFUD_ERR_TIMEOUT;
        }
        spi_transaction_t tt = {
                .length = 8 * read_size,
                .rxlength = 8 * read_size,
                .rx_buffer = read_buf
        };
        if (ESP_OK != spi_device_polling_transmit(spi_handle, &tt)) {
            result |= SFUD_ERR_TIMEOUT;
        }
    } else if (write_size) {
        spi_transaction_t t = {
                .length = 8 * write_size,
                .tx_buffer = write_buf
        };
        if (ESP_OK != spi_device_polling_transmit(spi_handle, &t)) {
            result = SFUD_ERR_WRITE;
        }
    } else if (read_size) {
        spi_transaction_t t = {
                .length = 8 * read_size,
                .rxlength = 8 * read_size,
                .rx_buffer = read_buf
        };
        if (ESP_OK != spi_device_polling_transmit(flash_spi_handle.spix, &t)) {
            result = SFUD_ERR_READ;
        }
    }

    if (SFUD_SUCCESS != result) {
        ESP_LOGW(TAG, "SPI write/read error: %d", result);
    }

    gpio_set_level(FLASH_CS_Pin, 1);

    return result;
}

#ifdef SFUD_USING_QSPI

/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */

    return result;
}

#endif /* SFUD_USING_QSPI */

void IRAM_ATTR retry_delay_100us() {
    uint64_t m = (uint64_t) esp_timer_get_time();
    uint64_t e = m + 100;
    if (m > e) { //overflow
        while ((uint64_t) esp_timer_get_time() > e) {
            asm volatile ("nop");
        }
    }
    while ((uint64_t) esp_timer_get_time() < e) {
        asm volatile ("nop");
    }
}


sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    ESP_LOGI(TAG, "Initializing SPI bus...");
    spi_bus_config_t buscfg = {
            .miso_io_num = FLASH_MISO_Pin,
            .mosi_io_num = FLASH_MOSI_Pin,
            .sclk_io_num = FLASH_SCLK_Pin,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE
    };
    result = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(result);
    //Attach the flash to the SPI bus
    spi_device_interface_config_t devcfg = {
            .flags = SPI_DEVICE_HALFDUPLEX,
            .clock_speed_hz = SPI_MASTER_FREQ_40M,
            .mode = 0,
            .spics_io_num = -1,     // control cs pin manually
            .queue_size = 7,
    };
    result = spi_bus_add_device(SPI2_HOST, &devcfg, &flash_spi_handle.spix);
    ESP_ERROR_CHECK(result);
    // init flash cs pin
    gpio_config_t gpio_conf = {
            .pin_bit_mask = (1 << FLASH_CS_Pin),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_conf);
    gpio_set_level(FLASH_CS_Pin, 1);

    ESP_LOGI(TAG, "Creating Semaphore for SFUD...");
    s_lock = xSemaphoreCreateCounting(1, 1);
    SFUD_ASSERT(s_lock != NULL);

    ESP_LOGI(TAG, "Registering flash in SFUD...");
    switch (flash->index) {
        case SFUD_W25Q128_DEVICE_INDEX: {
            flash->spi.wr = spi_write_read;
            flash->spi.lock = spi_lock;
            flash->spi.unlock = spi_unlock;
            flash->spi.user_data = &flash_spi_handle;
            /* about 100 microsecond delay */
            flash->retry.delay = retry_delay_100us;
            /* about 300 seconds timeout */
            /* a large delay for benchmark */
            flash->retry.times = 300 * 10000;
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
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    ESP_LOGI(TAG, "(%s:%ld)%s", file, line, log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    ESP_LOGI(TAG, "%s", log_buf);
    va_end(args);
}
