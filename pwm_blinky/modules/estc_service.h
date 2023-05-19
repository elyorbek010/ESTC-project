#ifndef ESTC_SERVICE_H__
#define ESTC_SERVICE_H__

#include <stdint.h>

#include "ble.h"
#include "sdk_errors.h"

// Generate random BLE UUID (Version 4 UUID)
#define ESTC_BASE_UUID { 0xf9, 0xc9, 0x01, 0xf5, 0x87, 0xc3, /* - */ 0xf6, 0x91, /* - */ 0xfe, 0x4f, /* - */ 0x4c, 0x4d, /* - */ 0x67, 0x20, 0xf5, 0x16 } // UUID: 16f52067-4d4c-4ffe-91f6-c387f501c9f9

// Pick a random service 16-bit UUID and define it:
#define ESTC_SERVICE_UUID 0x2067

// Pick a characteristic UUID and define it:
#define ESTC_GATT_CHAR_1_UUID 0x2068
#define ESTC_GATT_CHAR_2_UUID 0x2069

typedef struct
{
    uint16_t service_handle;
    uint16_t connection_handle;

    // Add handles for characterstic (type: ble_gatts_char_handles_t)
    ble_gatts_char_handles_t char_1_handle;
    ble_gatts_char_handles_t char_2_handle;
} ble_estc_service_t;

ret_code_t estc_ble_service_init(ble_estc_service_t *service);

// void estc_ble_service_on_ble_event(const ble_evt_t *ble_evt, void *ctx);

void estc_notify_char(ble_estc_service_t *service);

#endif /* ESTC_SERVICE_H__ */