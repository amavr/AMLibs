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
        esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
        return res;
    }

    void setReceiveCallback(esp_now_recv_cb_t onMsg)
    {
        esp_now_register_recv_cb(onMsg);
    }

    void send(const char *macAddr, const char *data)
    {
        uint8_t mac[6];
        ATools::macToBytes(macAddr, mac);

        // esp_now_add_peer(bytes, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
        // esp_now_peer_info_t peerInfo;

        Serial.printf("ADDR:%s\tMSG:%s\n", macAddr, data);
        esp_now_send(mac, (uint8_t *)data, strlen(data));
    }
};