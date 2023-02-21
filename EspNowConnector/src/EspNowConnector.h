#pragma once

#include <ATools.h>
#include <espnow.h>

class EspNowConnector
{
private:
public:
    bool start()
    {
        bool res = esp_now_init() == 0;
        Serial.printf("esp_now_init: %s\n", res ? "true" : "false");
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
        return res;
    }

    void setReceiveCallback(esp_now_recv_cb_t onMsg)
    {
        int res = esp_now_register_recv_cb(onMsg);
        Serial.printf("esp_now_register_recv_cb: %d\n", res);
    }

    void setSendCallback(esp_now_send_cb_t onSent)
    {
        int res = esp_now_register_send_cb(onSent);
        Serial.printf("esp_now_register_send_cb: %d\n", res);
    }

    void pair(uint8_t *mac)
    {
        if (!esp_now_is_peer_exist(mac))
        {
            esp_now_add_peer(mac, ESP_NOW_ROLE_COMBO, 2, NULL, 0);
        }
    }

    void pair(const char *mac)
    {
        uint8_t bytes[6];
        ATools::macToBytes(mac, bytes);
        pair(bytes);
    }

    void send(u8 *mac, const char *data, u8 len)
    {
        Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x <- [%s]\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], data);

        u8 *buf = (u8 *)malloc(len);
        memcpy(buf, data, len);
        buf[len] = 0;

        esp_now_send(mac, buf, len);

        free(buf);
    }

    void send(const char *addr, const char *data)
    {
        uint8_t mac[6];
        ATools::macToBytes(addr, mac);

        int len = strlen(data) + 1;
        send(mac, data, len);

        // Serial.printf("%s <- [%s]\n", macAddr, data);
        // esp_now_send(mac, (uint8_t *)data, len);

    }
};