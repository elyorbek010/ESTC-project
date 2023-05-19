#include "estc_service.h"

#include "app_error.h"
#include "nrf_log.h"

#include "ble.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"

static ret_code_t estc_ble_add_characteristics(ble_estc_service_t *service);

const ble_uuid128_t p_vs_uuid = {
    .uuid128 = ESTC_BASE_UUID};

ret_code_t estc_ble_service_init(ble_estc_service_t *service)
{
    ret_code_t error_code = NRF_SUCCESS;

    ble_uuid_t service_uuid = {
        .uuid = ESTC_SERVICE_UUID,
    };

    // Add service UUIDs to the BLE stack table
    sd_ble_uuid_vs_add(&p_vs_uuid, &service_uuid.type);

    // Add service to the BLE stack
    sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &service->service_handle);

    NRF_LOG_DEBUG("%s:%d | Service UUID: 0x%04x", __FUNCTION__, __LINE__, service_uuid.uuid);
    NRF_LOG_DEBUG("%s:%d | Service UUID type: 0x%02x", __FUNCTION__, __LINE__, service_uuid.type);
    NRF_LOG_DEBUG("%s:%d | Service handle: 0x%04x", __FUNCTION__, __LINE__, service->service_handle);

    estc_ble_add_characteristics(service);

    return error_code;
}

static ret_code_t estc_ble_add_characteristics(ble_estc_service_t *service)
{
    ret_code_t error_code = NRF_SUCCESS;

    ble_uuid_t char_1_uuid = {
        .uuid = ESTC_GATT_CHAR_1_UUID};

    ble_uuid_t char_2_uuid = {
        .uuid = ESTC_GATT_CHAR_2_UUID};

    // Add custom characteristic UUID
    sd_ble_uuid_vs_add(&p_vs_uuid, &char_1_uuid.type);
    sd_ble_uuid_vs_add(&p_vs_uuid, &char_2_uuid.type);

    // Configure Characteristic metadata (read, write, and notify)
    ble_gatts_char_md_t char_1_md = {
        .char_props = {
            .write = true}};

    ble_gatts_char_md_t char_2_md = {
        .char_props = {
            .read = true,
            .notify = true}};

    // Configures attribute metadata. For now we only specify that the attribute will be stored in the softdevice
    ble_gatts_attr_md_t attr_1_md = {0};
    attr_1_md.vloc = BLE_GATTS_VLOC_STACK;

    ble_gatts_attr_md_t attr_2_md = {0};
    attr_2_md.vloc = BLE_GATTS_VLOC_STACK;

    // Set read/write security levels to our attribute metadata
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_1_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_2_md.read_perm);

    // Configure the characteristic value attribute (set the UUID and metadata)
    ble_gatts_attr_t attr_char_1_value = {
        .p_uuid = &char_1_uuid,
        .p_attr_md = &attr_1_md};

    ble_gatts_attr_t attr_char_2_value = {
        .p_uuid = &char_2_uuid,
        .p_attr_md = &attr_2_md};

    // Set characteristic length in number of bytes in attr_char_value structure
    attr_char_1_value.init_len = 3 * sizeof(uint8_t);
    attr_char_1_value.max_len = 3 * sizeof(uint8_t);

    attr_char_2_value.init_len = 3 * sizeof(uint8_t);
    attr_char_2_value.max_len = 3 * sizeof(uint8_t);

    // Add new characteristic to the service
    sd_ble_gatts_characteristic_add(service->service_handle, &char_1_md, &attr_char_1_value, &(service->char_1_handle));
    sd_ble_gatts_characteristic_add(service->service_handle, &char_2_md, &attr_char_2_value, &(service->char_2_handle));

    return error_code;
}

// void estc_update_characteristic_1_value(ble_estc_service_t *service, int32_t *value)
// {
//     ble_gatts_value_t value_gatts = {
//         .len = sizeof(*value),
//         .offset = 0,
//         .p_value = (uint8_t *)value};
//     sd_ble_gatts_value_set(service->connection_handle, service->char_1_handle.value_handle, &value_gatts);
// }

void estc_notify_char(ble_estc_service_t *service)
{
    uint16_t len = sizeof(3 * sizeof(uint8_t)); // size of RGB
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = service->char_2_handle.value_handle;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len = &len;
    hvx_params.p_data = NULL;

    sd_ble_gatts_hvx(service->connection_handle, &hvx_params);
}