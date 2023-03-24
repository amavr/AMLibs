#pragma once

#include <Arduino.h>

struct Item
{
    char id[20];   // ключ
    char data[18]; // mac address
    Item *next;
};

class DicList
{
private:
    Item *head;
    int count;

public:
    DicList()
    {
        count = 0;
        head = NULL;
    }

    int Count()
    {
        return count;
    }

    Item *find(const char *id)
    {
        Item *curr = head;
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

    const char *get(const char *id)
    {
        Item *item = find(id);
        return item == NULL ? NULL : item->data;
    }

    Item *set(const char *id, const char *data)
    {
        Item *item = find(id);

        // если не найден, то создать
        if (item == NULL)
        {
            // создание узла
            count++;
            item = new Item;
            strcpy(item->id, id);
            item->next = NULL;

            // пустой список? тогда в самое начало
            if (head == NULL)
            {
                head = item;
            }
            else
            {
                // в конец
                Item *curr = head;
                while (curr->next != NULL)
                {
                    curr = curr->next;
                }
                curr->next = item;
            }
        }
        strcpy(item->data, data);
        return item;
    }

    void forEach(void (*callback)(const char *id, const char *data, int index))
    {
        int i = 0;
        Item *curr = head;
        while (curr != NULL)
        {
            callback(curr->id, curr->data, i);
            curr = curr->next;
            i++;
        }
    }

    int getSize()
    {
        int len = 0;
        Item *curr = head;
        while (curr != NULL)
        {
            len += strlen(curr->id) + strlen(curr->data) + 2;
            // Serial.printf("%s(%d)-%s(%d)\n", curr->id, strlen(curr->id), curr->data, strlen(curr->data));
            curr = curr->next;
        }
        return len + 1;
    }

    // заполнить подготовленный снаружи буффер
    void fillTo(char *buf, char separator = '\t')
    {
        int len = 0;
        Item *curr = head;
        while (curr != NULL)
        {
            strcpy(buf + len, curr->id);
            len += strlen(curr->id);
            buf[len++] = separator;
            strcpy(buf + len, curr->data);
            len += strlen(curr->data);
            buf[len++] = '\n';

            curr = curr->next;
        }
        buf[len] = '\0';
    }
};