#pragma once

#include <Arduino.h>

struct Sub
{
    char *id; // это адрес (mac, ip, ...)
    Sub *next;
};

class SubList
{
private:
    Sub *head;
    int count;

public:
    SubList()
    {
        count = 0;
        head = NULL;
    }

    int Count()
    {
        return count;
    }

    void clear()
    {
        Sub *curr = head;
        while (curr != NULL)
        {
            curr = curr->next;

            free(head->id);
            free(head);
            head = curr;
        }
        count = 0;
    }

    Sub *find(const char *id)
    {
        Sub *curr = head;
        while (curr != NULL)
        {
            if (strcmp(curr->id, id) == 0)
            {
                return curr;
            }
            curr = curr->next;
        }
        return NULL;
    }

    bool exists(const char *id)
    {
        return find(id) != NULL;
    }

    void add(const char *id)
    {
        // если не найден, то создать и добавить
        if (!exists(id))
        {
            // создание узла
            count++;
            Sub *sub = new Sub;
            sub->id = (char *)malloc(strlen(id) + 1);
            strcpy(sub->id, id);
            sub->next = NULL;

            // пустой список? тогда в самое начало
            if (head == NULL)
            {
                // Serial.printf("as first: %s\n", sub->id);
                head = sub;
            }
            else
            {
                // в конец
                Sub *curr = head;
                while (curr->next != NULL)
                {
                    curr = curr->next;
                }
                // Serial.printf("as last: %s\n", sub->id);
                curr->next = sub;
            }
        }
    }

    int getSize()
    {
        int len = 0;
        Sub *curr = head;
        while (curr != NULL)
        {
            len += strlen(curr->id) + 1;
            curr = curr->next;
        }
        return len;
    }

    // заполнить подготовленный снаружи буффер
    int fillTo(char *buf)
    {
        int len = 0;
        Sub *curr = head;
        while (curr != NULL)
        {
            strcpy(buf + len, curr->id);
            len += strlen(curr->id);
            buf[len++] = ',';

            curr = curr->next;
        }
        len--;
        buf[len] = '\0'; // вместо последнего разделителя (,)
        return len;
    }

    // не забывать зачищать выделенную под строку память
    char *toString(char separator = ',')
    {
        int len = getSize();
        if (len == 0)
        {
            return NULL;
        }

        char *s = (char *)malloc(len);
        memset(s, 0, len);

        Sub *curr = head;
        while (curr != NULL)
        {
            strcat(s, curr->id);
            s[strlen(s)] = separator;
            curr = curr->next;
        }
        s[strlen(s) - 1] = '\0';

        return s;
    }

    void forEach(void (*callback)(const char *id))
    {
        Sub *curr = head;
        while (curr != NULL)
        {
            callback(curr->id);
            curr = curr->next;
        }
    }

    void fill(char **subIds)
    {
        int i = 0;
        Sub *curr = head;
        while (curr != NULL)
        {
            subIds[i++] = strdup(curr->id);
            curr = curr->next;
        }
    }
};
