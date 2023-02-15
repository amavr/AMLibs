#ifndef AToolsH
#define AToolsH

#include <Arduino.h>

class ATools
{
public:
    // нормализация пробелов + trim
    static void normalize(const char *sour, char *dest)
    {
        int sour_len = strlen(sour);
        if (sour_len == 0)
        {
            dest[0] = '\0';
            return;
        }

        int di = 0;
        bool prev_space = true;
        for (int si = 0; si < sour_len; si++)
        {
            // обработка пробелов
            if (sour[si] == ' ')
            {
                // ранее было не начало строки и не пробел?
                if (!prev_space)
                {
                    // значимый пробел (после непробельного символа)
                    dest[di++] = sour[si];
                    // но след.пробел надо пропустить
                    prev_space = true;
                }
            }
            // пришел не пробел
            else
            {
                dest[di++] = sour[si];
                prev_space = false;
            }
        }

        dest[di] = '\0';
        // на конце может оставаться значимый пробел
        while (di > 0 && dest[--di] == ' ')
        {
            dest[di] = '\0';
        }
    }

    static bool isCmd(
        const char *cmd,  // команда с которой происходит сравнение
        const char *data, // принятый текст
        char *rest        // выделенные здесь параметры команды
    )
    {
        size_t cmd_len = strlen(cmd);
        size_t data_len = strlen(data);
        // команда длиннее, чем анализируемый текст
        // т.е. явно не та команда
        if (data_len < cmd_len)
        {
            return false;
        }

        // длина команды не менее 3 символов?
        if (cmd_len >= 3)
        {
            // команда совпала?
            if (strncmp(cmd, data, cmd_len) == 0)
            {
                // забрать параметры
                // нет параметров?
                if (cmd_len == data_len)
                {
                    rest[0] = '\0';
                }
                else
                {
                    size_t prm_len = data_len - cmd_len;
                    strncpy(rest, data + cmd_len, prm_len);
                    rest[prm_len] = '\0';
                }
                return true;
            }
        }
        return false;
    }

    static void macToChars(const uint8_t *mac, char *chars)
    {
        char buf[18];
        snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        strcpy(chars, buf);
    }

    static void macToBytes(const char *text, uint8_t *bytes)
    {
        char *t_ptr;
        char mac[18];
        strcpy(mac, text);

        t_ptr = strtok(mac, ":");
        int i = 0;
        while (t_ptr != NULL)
        {
            bytes[i] = strtol(t_ptr, NULL, 16);
            t_ptr = strtok(NULL, ":");
            i++;
        }
    }

    static bool macValidate(char *mac)
    {
        char mac_address[18];
        char *t_ptr;

        strcpy(mac_address, mac);
        t_ptr = strtok(mac_address, ":");

        int i = 0;
        while (t_ptr != NULL)
        {
            int octet = strtol(t_ptr, NULL, 16);
            if (octet < 0 || octet > 255)
                return false;

            t_ptr = strtok(NULL, ":");
            i++;
        }

        // loop should've covered exactly 6 octets
        if (i != 6)
            return false;

        return true;
    }

    static void split(const char *line, char delim, void (*cb)(const char *part, int index))
    {
        char *ptr;
        char buf[strlen(line) + 1];
        strcpy(buf, line);

        ptr = strtok(buf, ":");
        int i = 0;
        while (ptr != NULL)
        {
            cb(ptr, i);
            ptr = strtok(NULL, ":");
            i++;
        }
    }

};

#endif