#include <stdio.h>
#include <string.h>
#include "esp_timer.h"
#include "sfud.h"
#include "esp_log.h"

#define SFUD_DEMO_TEST_BUFFER_SIZE 1024

static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];

static void sfud_demo(uint32_t addr, size_t size, uint8_t *data);

static void sfud_benchmark();

void app_main(void) {
    if (sfud_init() == SFUD_SUCCESS) {
        sfud_demo(0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);
        sfud_benchmark();
    }
}


/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
static void sfud_demo(uint32_t addr, size_t size, uint8_t *data) {
    static const char *TAG = "SFUD_DEMO";
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;
    /* prepare write data */
    for (i = 0; i < size; i++) {
        data[i] = i;
    }
    /* erase test */
    result = sfud_erase(flash, addr, size);
    if (result == SFUD_SUCCESS) {
        ESP_LOGI(TAG, "Erase the %s flash data finish. Start from 0x%08lu, size is %u.",
                 flash->name, addr, size);
    } else {
        ESP_LOGW(TAG, "Erase the %s flash data failed.", flash->name);
        return;
    }
    /* write test */
    result = sfud_write(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        ESP_LOGI(TAG, "Write the %s flash data finish. Start from 0x%08lu, size is %u.",
                 flash->name, addr, size);
    } else {
        ESP_LOGW(TAG, "Write the %s flash data failed.", flash->name);
        return;
    }
    /* read test */
    result = sfud_read(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        ESP_LOGI(TAG, "Read the %s flash data success. Start from 0x%08lu, size is %u. The data is:",
                 flash->name, addr, size);
        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++) {
            if (i % 16 == 0) {
                printf("[%08lX] ", addr + i);
            }
            printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1) {
                printf("\r\n");
            }
        }
        printf("\r\n");
    } else {
        ESP_LOGW(TAG, "Read the %s flash data failed.", flash->name);
    }
    /* data check */
    for (i = 0; i < size; i++) {
        if (data[i] != i % 256) {
            ESP_LOGW(TAG, "Read and check write data has an error. Write the %s flash data failed.",
                     flash->name);
            break;
        }
    }
    if (i == size) {
        ESP_LOGI(TAG, "The %s flash test is success.", flash->name);
    }
}

static void sfud_benchmark() {
    /* full chip benchmark test */
    static const char *TAG = "SFUD_BENCHMARK";
    sfud_err result = SFUD_SUCCESS;
    size_t addr = 0;
    const sfud_flash *sfud_dev = sfud_get_device_table() + 0;
    uint32_t size = sfud_dev->chip.capacity;
    uint64_t start_time, time_cast;
    size_t write_size = SFUD_WRITE_MAX_PAGE_SIZE, read_size = 4092;
    uint8_t *write_data = malloc(write_size), *read_data = malloc(read_size);

    if (write_data && read_data) {
        memset(write_data, 0x55, write_size);
        /* benchmark testing */
        ESP_LOGI(TAG, "Erasing the %s %ld bytes data, waiting...", sfud_dev->name, size);
        start_time = (uint64_t) esp_timer_get_time() / 1000ULL;
        result = sfud_erase(sfud_dev, addr, size);
        if (result == SFUD_SUCCESS) {
            time_cast = (uint64_t) esp_timer_get_time() / 1000ULL - start_time;
            ESP_LOGI(TAG, "Erase benchmark success, total time: %lld.%03lldS.",
                     time_cast / 1000, time_cast % 1000);
        } else {
            ESP_LOGW(TAG, "Erase benchmark has an error. Error code: %d.", result);
        }
        /* write test */
        ESP_LOGI(TAG, "Writing the %s %ld bytes data, waiting...", sfud_dev->name, size);
        start_time = (uint64_t) esp_timer_get_time() / 1000ULL;
        for (size_t i = 0; i < size; i += write_size) {
            result = sfud_write(sfud_dev, addr + i, write_size, write_data);
            if (result != SFUD_SUCCESS) {
                break;
            }
        }
        if (result == SFUD_SUCCESS) {
            time_cast = (uint64_t) esp_timer_get_time() / 1000ULL - start_time;
            ESP_LOGI(TAG, "Write benchmark success, total time: %lld.%03lldS.",
                     time_cast / 1000, time_cast % 1000);
        } else {
            ESP_LOGW(TAG, "Write benchmark has an error. Error code: %d.", result);
        }
        /* read test */
        ESP_LOGI(TAG, "Reading the %s %ld bytes data, waiting...", sfud_dev->name, size);
        start_time = (uint64_t) esp_timer_get_time() / 1000ULL;
        for (size_t i = 0; i < size; i += read_size) {
            if (i + read_size <= size) {
                result = sfud_read(sfud_dev, addr + i, read_size, read_data);
            } else {
                result = sfud_read(sfud_dev, addr + i, size - i, read_data);
            }
            if (result != SFUD_SUCCESS) {
                break;
            }
        }
        if (result == SFUD_SUCCESS) {
            time_cast = (uint64_t) esp_timer_get_time() / 1000ULL - start_time;
            ESP_LOGI(TAG, "Read benchmark success, total time: %lld.%03lldS.",
                     time_cast / 1000, time_cast % 1000);
        } else {
            ESP_LOGW(TAG, "Read benchmark has an error. Error code: %d.", result);
        }
    } else {
        ESP_LOGW(TAG, "Low memory!");
    }
}

