#pragma once

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>

const uint16_t UDP_PORT = 2222;
WiFiUDP udp;

void broadcastUDP(const char *text)
{
    IPAddress addr = WiFi.localIP();
    addr[3] = 0xff;
    udp.beginPacket(addr, udp.localPort());
    udp.print(text);
    udp.endPacket();
}

bool UDPRequest(const char *cmd, char *answer)
{
    bool res = false;
    answer[0] = '\0';
    if (udp.begin(UDP_PORT))
    {
        broadcastUDP(cmd);

        uint32_t beg = millis();
        while (true)
        {
            int packetSize = udp.parsePacket();
            if (packetSize)
            {
                int len = udp.read(answer, 0x100);
                answer[len] = '\0';
                res = true;
                break;
            }
            if (millis() - beg > 1000)
            {
                break;
            }
        }

        udp.stop();
    }
    return res;
}

void setBrokerAddr()
{
    if (udp.begin(UDP_PORT))
    {
        char cmd[128] = "";
        sprintf(cmd, "set role broker %s", WiFi.macAddress().c_str());
        broadcastUDP(cmd);
        udp.stop();
    }
}

bool getBrokerAddr(char *addr)
{
    return UDPRequest("get role broker", addr);
}