#include <Arduino.h>

uint16_t sizeEEPROM = 512;

#include "WiFiController.h"
WiFiController ctrl(0); // 0 - начальный адрес в памяти для WiFi настроек


void setup()
{
    Serial.begin(115200);
    delay(500);

    EEPROM.begin(sizeEEPROM);

    // Признак первого запуска
    bool isFirstTime = EEPROM[128] != 0x22;

    Serial.printf("isFirstTime: %d\n", isFirstTime);

    ctrl.connect(isFirstTime);

    // после инициализации и подключения
    // EEPROM проинициализирована, поэтому сброс флага первого запуска
    if(isFirstTime){
        EEPROM[128] = 0x22;
        EEPROM.commit();
    }
}

void loop()
{
    // проверка WiFi и если требуется - подключение
    ctrl.tick();
    delay(10);
}
