#ifndef $WiFiController
#define $WiFiController

#include <Arduino.h>
#include <EEPROM.h>

// подключение к WiFi
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
typedef WiFiMode_t WiFiMode;
#endif

// сайт настроек для поключения к WiFi
#include "ConfigSite.h"

struct TWiFiParams
{
    uint16_t connectPeriod = 30; // длительность попытки подключения к WiFi в секундах
    uint16_t siteLifeTime = 240; // длительность работы сайта для введения параметров подключения в секундах
    char ssid[20] = "x";
    char pass[20] = "x";
};

void xDelay(uint16_t timeout)
{
#ifdef ESP8266
    delay(timeout);
#else
    vTaskDelay(timeout);
#endif
}

class WiFiController
{
public:
    WiFiController();
    void connect(bool isFirtsTime, WiFiMode mode = WIFI_STA);
    void disconnect();
    void init();
    void tick();
    size_t useEEPROMSize();

private:
    TWiFiParams cfg;
    void loadWiFiParams();
    void saveWiFiParams();
    bool runSite();
};

WiFiController::WiFiController()
{
}

void WiFiController::connect(bool isFirstTime, WiFiMode mode)
{
    if (isFirstTime)
    {
        init();
    }

    WiFi.mode(mode);

    while (true)
    {
        bool changed = false;
        loadWiFiParams();
        WiFi.begin(cfg.ssid, cfg.pass, 0);
        unsigned long started = millis();
        uint32_t period = cfg.connectPeriod * 1000;
        Serial.printf("Connecting to %s", cfg.ssid);
        while (WiFi.status() != WL_CONNECTED)
        {
            xDelay(500);
            Serial.print('.');
            if (millis() - started > period)
            {
                started = millis();
                Serial.println("timeout");
                Serial.println("Start AP to change config.");
                changed = runSite();
                Serial.println(changed ? "Stop AP with changes" : "Stop AP without changes");
                if (changed)
                {
                    saveWiFiParams();
                    Serial.printf("saved ssid:%s passw:%s\n", cfg.ssid, cfg.pass);
                }
                // если не было изменений параметров
                // все равно проверять подключение,
                // может роутер выключался
                WiFi.disconnect(true);
                break;
            }
        }
        if (WiFi.status() == WL_CONNECTED)
        {
            break;
        }
    }
    Serial.printf("done! Channel: %d\n", WiFi.channel());
    Serial.println(WiFi.localIP());
}

void WiFiController::disconnect()
{
    WiFi.disconnect();
}

void WiFiController::init()
{
    // for (int i = 0; i < (int)EEPROM.length(); i++)
    // {
    //     EEPROM.put(i, 0);
    // }
    EEPROM.put(2, cfg);
    EEPROM.commit();
}

void WiFiController::tick()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        connect(false);
    }
}

size_t WiFiController::useEEPROMSize()
{
    return sizeof(cfg);
}

void WiFiController::loadWiFiParams()
{
    EEPROM.get(2, cfg);
    Serial.printf("load ssid:%s pass:%s\n", cfg.ssid, cfg.pass);
}

void WiFiController::saveWiFiParams()
{
    EEPROM.put(2, cfg);
    EEPROM.commit();
}

bool WiFiController::runSite()
{
    strcpy(siteCfg.ssid, cfg.ssid);
    strcpy(siteCfg.pass, cfg.pass);
    siteCfg.lifeTimeSec = cfg.siteLifeTime;

    run();

    if (changed())
    {
        strcpy(cfg.ssid, siteCfg.ssid);
        strcpy(cfg.pass, siteCfg.pass);
    }
    return changed();
}

#endif