#ifndef __UNIT_ACMEASURE_H
#define __UNIT_ACMEASURE_H

#include "driver/i2c.h"

#define UNIT_ACMEASURE_ADDR                      0x42
#define UNIT_ACMEASURE_VOLTAGE_STRING_REG        0x00
#define UNIT_ACMEASURE_CURRENT_STRING_REG        0x10
#define UNIT_ACMEASURE_POWER_STRING_REG          0x20
#define UNIT_ACMEASURE_APPARENT_POWER_STRING_REG 0x30
#define UNIT_ACMEASURE_POWER_FACTOR_STRING_REG   0x40
#define UNIT_ACMEASURE_KWH_STRING_REG            0x50
#define UNIT_ACMEASURE_VOLTAGE_REG               0x60
#define UNIT_ACMEASURE_CURRENT_REG               0x70
#define UNIT_ACMEASURE_POWER_REG                 0x80
#define UNIT_ACMEASURE_APPARENT_POWER_REG        0x90
#define UNIT_ACMEASURE_POWER_FACTOR_REG          0xA0
#define UNIT_ACMEASURE_KWH_REG                   0xB0
#define UNIT_ACMEASURE_VOLTAGE_FACTOR_REG        0xC0
#define UNIT_ACMEASURE_CURRENT_FACTOR_REG        0xD0
#define UNIT_ACMEASURE_SAVE_FACTOR_REG           0xE0
#define UNIT_ACMEASURE_GET_READY_REG             0xFC
#define JUMP_TO_BOOTLOADER_REG                   0xFD
#define FIRMWARE_VERSION_REG                     0xFE
#define I2C_ADDRESS_REG                          0xFF

typedef struct {
    uint8_t addr;
    i2c_port_t i2c_num;
} UNIT_ACMEASURE;

bool UNIT_ACMEASURE_begin(UNIT_ACMEASURE *device, i2c_port_t i2c_num, gpio_num_t sda, gpio_num_t scl, uint32_t speed);
uint8_t UNIT_ACMEASURE_getFirmwareVersion(UNIT_ACMEASURE *device);
uint16_t UNIT_ACMEASURE_getVoltage(UNIT_ACMEASURE *device);
uint16_t UNIT_ACMEASURE_getCurrent(UNIT_ACMEASURE *device);
uint32_t UNIT_ACMEASURE_getPower(UNIT_ACMEASURE *device);
uint32_t UNIT_ACMEASURE_getApparentPower(UNIT_ACMEASURE *device);
uint8_t UNIT_ACMEASURE_getPowerFactor(UNIT_ACMEASURE *device);
uint32_t UNIT_ACMEASURE_getKWH(UNIT_ACMEASURE *device);
void UNIT_ACMEASURE_getVoltageString(UNIT_ACMEASURE *device, char *str);
void UNIT_ACMEASURE_getCurrentString(UNIT_ACMEASURE *device, char *str);
void UNIT_ACMEASURE_getPowerString(UNIT_ACMEASURE *device, char *str);
void UNIT_ACMEASURE_getApparentPowerString(UNIT_ACMEASURE *device, char *str);
void UNIT_ACMEASURE_getPowerFactorString(UNIT_ACMEASURE *device, char *str);
void UNIT_ACMEASURE_getKWHString(UNIT_ACMEASURE *device, char *str);
uint8_t UNIT_ACMEASURE_getReady(UNIT_ACMEASURE *device);
void UNIT_ACMEASURE_setKWH(UNIT_ACMEASURE *device, uint32_t value);
uint8_t UNIT_ACMEASURE_getVoltageFactor(UNIT_ACMEASURE *device);
uint8_t UNIT_ACMEASURE_getCurrentFactor(UNIT_ACMEASURE *device);
void UNIT_ACMEASURE_setVoltageFactor(UNIT_ACMEASURE *device, uint8_t value);
void UNIT_ACMEASURE_setCurrentFactor(UNIT_ACMEASURE *device, uint8_t value);
void UNIT_ACMEASURE_saveVoltageCurrentFactor(UNIT_ACMEASURE *device);
void UNIT_ACMEASURE_jumpBootloader(UNIT_ACMEASURE *device);
uint8_t UNIT_ACMEASURE_setI2CAddress(UNIT_ACMEASURE *device, uint8_t addr);
uint8_t UNIT_ACMEASURE_getI2CAddress(UNIT_ACMEASURE *device);

#endif
