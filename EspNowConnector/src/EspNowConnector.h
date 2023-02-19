#pragma once

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <ATools.h>
#include <espnow.h>

class EspNowConnector
{
private:
public:
    bool start()
    {
        WiFi.mode(WIFI_STA);
        bool res = esp_now_init() == 0;
        Serial.printf("esp_now_init: %d\n", res);
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
            esp_now_add_peer(mac, ESP_NOW_ROLE_COMBO, 0, NULL, 0);
        }
    }

    void pair(const char *mac)
    {
        uint8_t bytes[6];
        ATools::macToBytes(mac, bytes);
        pair(bytes);
    }

    void send(const char *macAddr, const char *data, bool makePair = false)
    {
        uint8_t mac[6];
        ATools::macToBytes(macAddr, mac);

        if (makePair)
        {
            pair(macAddr);
        }

        int len = strlen(data);
        // uint8_t *buf = (uint8_t *)malloc(len);
        // memcpy(buf, data, len);
        // buf[len] = 0;

        Serial.printf("%s <- [%s]\n", macAddr, data);
        esp_now_send(mac, (uint8_t *)data, len);

        // free(buf);
    }
};