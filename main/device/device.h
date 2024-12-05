#ifndef DEVICE_H
#define DEVICE_H

#include <stdbool.h>
#include "driver/gpio.h"
#include "nvs/nvs.h"

#define MAX_MODULES 3

typedef struct
{
    int index;
    int relayGpio;
    bool status;
} relay_t;

typedef struct
{
    const char *productId;
    const char *deviceName;
    const char *deviceMqtt;
    char *deviceServer;
    relay_t relayModules[MAX_MODULES];
    int numOfRelay;
} device_t;

void device_init();
void turnOnRelay(relay_t *relay);
void turnOffRelay(relay_t *relay);
void getStateRelay(relay_t *relay);

extern device_t device;

#endif // DEVICE_H