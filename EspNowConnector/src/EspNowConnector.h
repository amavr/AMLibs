#pragma once

#include <ATools.h>

#ifdef ESP8266
#include <espnow.h>
#else
#include <esp_now.h>
typedef uint8_t u8;
#endif

#define WIFI_CHANNEL 4

char BROKER_MAC[18] = "00:22:00:22:00:22";
char GATEWAY_MAC[18] = "01:22:01:22:01:22";
// char BROKER_ADDR[18] = "0C:B8:15:73:DB:24";

class EspNowConnector
{
private:
public:
    bool start()
    {
        bool res = esp_now_init() == 0;
        Serial.printf("esp_now_init: %s\n", res ? "true" : "false");
#ifdef ESP8266
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
#else
        // WiFi.mode(WIFI_);
#endif
        return res;
    }

    void setReceiveCallback(esp_now_recv_cb_t onMsg)
    {
        int res = esp_now_register_recv_cb(onMsg);
#ifdef DEBUG        
        Serial.printf("esp_now_register_recv_cb: %d\n", res);
#endif
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
#ifdef ESP8266
            esp_now_add_peer(mac, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
#else
            esp_now_peer_info_t peerInfo;
            peerInfo.channel = WIFI_CHANNEL;
            peerInfo.encrypt = false;
            peerInfo.ifidx = WIFI_IF_STA;
            // первая плата
            // uint8_t mac[6];
            // mac = WiFi.macAddress(mac);
            memcpy(peerInfo.peer_addr, mac, 6);
            if (esp_now_add_peer(&peerInfo) != ESP_OK)
            {
                Serial.println("Failed to add peer");
            }
#endif

#ifdef DEBUG        
            Serial.printf("paired %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
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
        // Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x <- [%s]\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], data);

        u8 *buf = (u8 *)malloc(len + 1);
        memcpy(buf, data, len);
        buf[len] = 0;
        esp_now_send(mac, buf, len + 1);

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