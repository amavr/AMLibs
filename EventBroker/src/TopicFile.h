/****************************************************************************************************************************
ConfigStorage.h - source for this class
for ESP32 and ESP8266 boards
*****************************************************************************************************************************/

#pragma once

#ifndef TopicFile_H

#define TopicFile_H

#ifndef ConfigStorage_DEBUG
#define ConfigStorage_DEBUG true
#endif // #ifndef ConfigStorage_DEBUG

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else // defined(ARDUINO) && (ARDUINO >= 100)
#include <WProgram.h>
#endif // #defined(ARDUINO) && (ARDUINO >= 100)

#ifndef CONFIG_STORAGE_VERSION
#define CONFIG_STORAGE_VERSION "ConfigStorage v0.0.1"
#define CONFIG_STORAGE_VERSION_MAJOR 0
#define CONFIG_STORAGE_VERSION_MINOR 0
#define CONFIG_STORAGE_VERSION_PATCH 1
#define CONFIG_STORAGE_VERSION_INT 0000001
#endif // #ifndef CONFIG_STORAGE_VERSION

#define ConfigStorage_VERSION CONFIG_STORAGE_VERSION

#define CS_USE_LITTLEFS true
// #define CS_USE_SPIFFS      false

#ifdef ESP32
// #    include <ArduinoJson.h>
#if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)

#if (ConfigStorage_DEBUG)
#warning Neither LittleFS nor SPIFFS selected. Default to LittleFS!
#endif // #if (ConfigStorage_DEBUG)

#ifdef CS_USE_LITTLEFS
#undef CS_USE_LITTLEFS
#define CS_USE_LITTLEFS true
#endif // #ifdef CS_USE_LITTLEFS

#endif // #if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)
#endif // #ifdef ESP32

#ifdef ESP8266
// #    include <ArduinoJson.h>
#if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)

#if (ConfigStorage_DEBUG)
#warning Neither LittleFS nor SPIFFS selected. Default to LittleFS!
#endif // #if (ConfigStorage_DEBUG)

#ifdef CS_USE_LITTLEFS
#undef CS_USE_LITTLEFS
#define CS_USE_LITTLEFS true
#endif // #ifdef CS_USE_LITTLEFS

#endif // #if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)
#endif // #ifdef ESP8266

// default to use LITTLEFS (higher priority), then SPIFFS
#if (CS_USE_LITTLEFS || CS_USE_SPIFFS)
#include <FS.h>
#ifdef ESP32
#if CS_USE_LITTLEFS

//       check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
#if (defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2))
#if (ConfigStorage_DEBUG)
#warning Using ESP32 Core 1.0.6 or 2.0.0+
#endif // #if (ConfigStorage_DEBUG)

//         the library has been merged into esp32 core from release 1.0.6
#include <LittleFS.h>
#define FileFS LittleFS

#else // #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )

#if (ConfigStorage_DEBUG)
#warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
#endif                // #if (ConfigStorage_DEBUG)

//         the library has been merged into esp32 core from release 1.0.6
#include <LITTLEFS.h> // https://github.com/lorol/LITTLEFS
#define FileFS LITTLEFS

#endif // #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )

#else // #if CS_USE_LITTLEFS

//       ESP32 core 1.0.4 still uses SPIFFS
#include "SPIFFS.h"
#define FileFS SPIFFS

#endif // #if CS_USE_LITTLEFS

#else // #ifdef ESP32

//     from ESP8266 core 2.7.1
#include <LittleFS.h>

#if CS_USE_LITTLEFS
#define FileFS LittleFS
#else // #if CS_USE_LITTLEFS
#define FileFS SPIFFS
#endif // #if CS_USE_LITTLEFS

#endif // #ifdef ESP32
#endif // #if ( CS_USE_LITTLEFS || CS_USE_SPIFFS )

#include <GParser.h>
#include <TopicList.h>
#include <SubList.h>

class TopicFile
{
public:
    static bool load(const char *fileName, TopicList *topics)
    {
        bool res = false;
        if (FileFS.begin())
        {
            if (FileFS.exists(fileName))
            {
                File configFile = FileFS.open(fileName, "r");
                if (!configFile)
                {
                    Serial.println("Reading config file failed");
                }
                else
                {
                    topics->clear();

                    // we could open the file
                    while (configFile.available())
                    {
                        // Lets read line by line from the file
                        String s = configFile.readStringUntil('\n');
                        if (s.length() > 2)
                        {
                            parseToTopic(s.c_str(), topics);
                        }
                    }
                    configFile.close();
                    res = true;
                }
            }
            else
            {
                Serial.printf("File %s not found\n", fileName);
            }
            FileFS.end();
        }
        else
        {
            Serial.println("FileFS not started");
        }
        return res;
    };

    static void save(const char *fileName, TopicList *topics)
    {
        if (FileFS.begin())
        {
            File configFile = FileFS.open(fileName, "w");
            if (configFile)
            {

                int len = topics->getSize();
                char *txt = (char *)malloc(len);
                topics->fillTo(txt);

                configFile.println(txt);
                configFile.close();

                Serial.println("Saving config file OK");
            }
            else
            {
                Serial.println("Saving config file failed");
            }
        }
    };

    static void remove(const char *fileName)
    {
        if (FileFS.begin())
        {
            if (FileFS.exists(fileName))
            {
                Serial.println("Config file exists");

                if (FileFS.remove(fileName))
                {
                    Serial.println("Removing config file OK");
                }
                else
                {
                    Serial.println("Removing config file failed");
                }
            }
            else
            {
                Serial.println("Config file missing");
            }
        }
    };

private:
    static void parseToTopic(const char *line, TopicList *topics)
    {
        Serial.printf("[%s]\n", line);

        char x[strlen(line) + 1];
        strcpy(x, line);

        GParser data(x, '|');
        int count = data.split();
        if (count == 2)
        {
            GParser sub(data[1], ',');
            int sub_count = sub.split();
            Topic *topic = topics->add(data[0]);
            for (int i = 0; i < sub_count; i++)
            {
                topic->subscribers->add(sub[i]);
            }
        }
    }
};

#endif