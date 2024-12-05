#include "unit_acmeasure.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "UNIT_ACMEASURE";

static void writeBytes(UNIT_ACMEASURE *device, uint8_t reg, uint8_t *buffer, uint8_t length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device->addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write(cmd, buffer, length, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(device->i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_cmd_begin error: %s", esp_err_to_name(ret));
    }
    i2c_cmd_link_delete(cmd);
}

static void readBytes(UNIT_ACMEASURE *device, uint8_t reg, uint8_t *buffer, uint8_t length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device->addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device->addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buffer, length, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(device->i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_cmd_begin error: %s", esp_err_to_name(ret));
    }
    i2c_cmd_link_delete(cmd);
}

bool UNIT_ACMEASURE_begin(UNIT_ACMEASURE *device, i2c_port_t i2c_num, gpio_num_t sda, gpio_num_t scl, uint32_t speed) {
    device->i2c_num = i2c_num;
    device->addr = UNIT_ACMEASURE_ADDR;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = speed,
    };
    i2c_param_config(i2c_num, &conf);
    esp_err_t ret = i2c_driver_install(i2c_num, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2C driver: %s", esp_err_to_name(ret));
        return false;
    }

    return true;
}

uint8_t UNIT_ACMEASURE_getFirmwareVersion(UNIT_ACMEASURE *device) {
    uint8_t data;
    readBytes(device, FIRMWARE_VERSION_REG, &data, 1);
    return data;
}

uint16_t UNIT_ACMEASURE_getVoltage(UNIT_ACMEASURE *device) {
    uint8_t data[2];
    readBytes(device, UNIT_ACMEASURE_VOLTAGE_REG, data, 2);
    return (uint16_t)(data[0] | (data[1] << 8));
}

uint16_t UNIT_ACMEASURE_getCurrent(UNIT_ACMEASURE *device) {
    uint8_t data[2];
    readBytes(device, UNIT_ACMEASURE_CURRENT_REG, data, 2);
    return (uint16_t)(data[0] | (data[1] << 8));
}

uint32_t UNIT_ACMEASURE_getPower(UNIT_ACMEASURE *device) {
    uint8_t data[4];
    readBytes(device, UNIT_ACMEASURE_POWER_REG, data, 4);
    return (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
}

uint32_t UNIT_ACMEASURE_getApparentPower(UNIT_ACMEASURE *device) {
    uint8_t data[4];
    readBytes(device, UNIT_ACMEASURE_APPARENT_POWER_REG, data, 4);
    return (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
}

uint8_t UNIT_ACMEASURE_getPowerFactor(UNIT_ACMEASURE *device) {
    uint8_t data;
    readBytes(device, UNIT_ACMEASURE_POWER_FACTOR_REG, &data, 1);
    return data;
}

uint32_t UNIT_ACMEASURE_getKWH(UNIT_ACMEASURE *device) {
    uint8_t data[4];
    readBytes(device, UNIT_ACMEASURE_KWH_REG, data, 4);
    return (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
}

void UNIT_ACMEASURE_setKWH(UNIT_ACMEASURE *device, uint32_t value) {
    writeBytes(device, UNIT_ACMEASURE_KWH_REG, (uint8_t *)&value, 4);
}

void UNIT_ACMEASURE_getVoltageString(UNIT_ACMEASURE *device, char *str) {
    char read_buf[7] = {0};
    readBytes(device, UNIT_ACMEASURE_VOLTAGE_STRING_REG, (uint8_t *)read_buf, 7);
    memcpy(str, read_buf, sizeof(read_buf));
}

void UNIT_ACMEASURE_getCurrentString(UNIT_ACMEASURE *device, char *str) {
    char read_buf[7] = {0};
    readBytes(device, UNIT_ACMEASURE_CURRENT_STRING_REG, (uint8_t *)read_buf, 7);
    memcpy(str, read_buf, sizeof(read_buf));
}

void UNIT_ACMEASURE_getPowerString(UNIT_ACMEASURE *device, char *str) {
    char read_buf[7] = {0};
    readBytes(device, UNIT_ACMEASURE_POWER_STRING_REG, (uint8_t *)read_buf, 7);
    memcpy(str, read_buf, sizeof(read_buf));
}

void UNIT_ACMEASURE_getApparentPowerString(UNIT_ACMEASURE *device, char *str) {
    char read_buf[7] = {0};
    readBytes(device, UNIT_ACMEASURE_APPARENT_POWER_STRING_REG, (uint8_t *)read_buf, 7);
    memcpy(str, read_buf, sizeof(read_buf));
}

void UNIT_ACMEASURE_getPowerFactorString(UNIT_ACMEASURE *device, char *str) {
    char read_buf[4] = {0};
    readBytes(device, UNIT_ACMEASURE_POWER_FACTOR_STRING_REG, (uint8_t *)read_buf, 4);
    memcpy(str, read_buf, sizeof(read_buf));
}

void UNIT_ACMEASURE_getKWHString(UNIT_ACMEASURE *device, char *str) {
    char read_buf[11] = {0};
    readBytes(device, UNIT_ACMEASURE_KWH_STRING_REG, (uint8_t *)read_buf, 11);
    memcpy(str, read_buf, sizeof(read_buf));
}

uint8_t UNIT_ACMEASURE_getVoltageFactor(UNIT_ACMEASURE *device) {
    uint8_t data;
    readBytes(device, UNIT_ACMEASURE_VOLTAGE_FACTOR_REG, &data, 1);
    return data;
}

uint8_t UNIT_ACMEASURE_getCurrentFactor(UNIT_ACMEASURE *device) {
    uint8_t data;
    readBytes(device, UNIT_ACMEASURE_CURRENT_FACTOR_REG, &data, 1);
    return data;
}

uint8_t UNIT_ACMEASURE_getReady(UNIT_ACMEASURE *device) {
    uint8_t data;
    readBytes(device, UNIT_ACMEASURE_GET_READY_REG, &data, 1);
    return data;
}

void UNIT_ACMEASURE_setVoltageFactor(UNIT_ACMEASURE *device, uint8_t value) {
    writeBytes(device, UNIT_ACMEASURE_VOLTAGE_FACTOR_REG, &value, 1);
}

void UNIT_ACMEASURE_setCurrentFactor(UNIT_ACMEASURE *device, uint8_t value) {
    writeBytes(device, UNIT_ACMEASURE_CURRENT_FACTOR_REG, &value, 1);
}

void UNIT_ACMEASURE_saveVoltageCurrentFactor(UNIT_ACMEASURE *device) {
    uint8_t value = 1;
    writeBytes(device, UNIT_ACMEASURE_SAVE_FACTOR_REG, &value, 1);
}

void UNIT_ACMEASURE_jumpBootloader(UNIT_ACMEASURE *device) {
    uint8_t value = 1;
    writeBytes(device, JUMP_TO_BOOTLOADER_REG, &value, 1);
}

uint8_t UNIT_ACMEASURE_setI2CAddress(UNIT_ACMEASURE *device, uint8_t addr) {
    writeBytes(device, I2C_ADDRESS_REG, &addr, 1);
    device->addr = addr;
    return addr;
}

uint8_t UNIT_ACMEASURE_getI2CAddress(UNIT_ACMEASURE *device) {
    uint8_t data;
    readBytes(device, I2C_ADDRESS_REG, &data, 1);
    return data;
}
