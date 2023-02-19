#pragma once

#include <Arduino.h>
#include <SubList.h>

struct Topic
{
    char *name;
    char *data = NULL;
    SubList *subscribers;
    Topic *next;
};

class TopicList
{
private:
    Topic *head;
    int count;

public:
    TopicList()
    {
        head = NULL;
        count = 0;
    }

    int Count()
    {
        return count;
    }

    void clear()
    {
        Topic *curr = head;
        while (curr != NULL)
        {
            curr = curr->next;

            head->subscribers->clear();
            free(head->name);
            free(head->subscribers);
            free(head);
            head = curr;
        }
        count = 0;
    }

    Topic *find(const char *topicName)
    {
        Topic *curr = head;
        while (curr != NULL)
        {
            if (strcmp(curr->name, topicName) == 0)
            {
                return curr;
            }
            curr = curr->next;
        }
        return NULL;
    }

    bool exists(const char *topicName)
    {
        return find(topicName) != NULL;
    }

    Topic *add(const char *topicName)
    {
        Topic *topic = find(topicName);
        // если не найден, добавляется в конец
        if (topic == NULL)
        {
            count++;
            // создание нового узла
            topic = new Topic;
            topic->name = (char *)malloc(strlen(topicName) + 1);
            strcpy(topic->name, topicName);
            topic->subscribers = new SubList;
            topic->next = NULL;

            Topic *curr = head, *prev = NULL;
            while (curr != NULL)
            {
                prev = curr;
                curr = curr->next;
            }
            // нет предпоследнего элемента - список пуст
            if (prev == NULL)
            {
                head = topic;
            }
            // указать в предпоследнем элементе на новый последний
            else
            {
                curr = topic;
                prev->next = curr;
            }
        }
        return topic;
    }

    int getSize()
    {
        int len = 0;
        Topic *curr = head;
        while (curr != NULL)
        {
            // сохранять надо только если есть подписчики
            if (curr->subscribers->Count() > 0)
            {
                len += strlen(curr->name) + 1 + curr->subscribers->getSize();
            }
            curr = curr->next;
        }
        return len;
    }

    // заполнить подготовленный снаружи буффер
    void fillTo(char *buf, char separator = '|')
    {
        int len = 0;
        Topic *curr = head;
        while (curr != NULL)
        {
            // сохранять надо только если есть подписчики
            if (curr->subscribers->Count() > 0)
            {
                strcpy(buf + len, curr->name);
                len += strlen(curr->name);
                buf[len++] = separator;
                len += curr->subscribers->fillTo(buf + len);
                buf[len++] = '\n';
            }
            curr = curr->next;
        }
        // buf[len - 1] = '\0';
    }

    void forEach(void (*callback)(Topic *topic))
    {
        Topic *curr = head;
        while (curr != NULL)
        {
            callback(curr);
            curr = curr->next;
        }
    }
};
