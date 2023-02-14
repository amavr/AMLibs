#pragma once

#include <Arduino.h>
#include "TopicList.h"
#include "SubList.h"

class EventBroker
{
private:
    TopicList *topics = new TopicList;

public:
    EventBroker()
    {
    }

    SubList *publish(const char *topicName, const char *data)
    {
        // если уже есть такой топик, то он и вовзращается, нет - создется
        Topic *topic = topics->add(topicName);
        // замена данных
        if(topic->data != NULL)
            // если было что-то, то удаляется
            free(topic->data);
        // установка новых
        topic->data = strdup(data);
        // возвращается указатель на список подписчиков
        return topic->subscribers;
    }

    void subscribe(const char *id, const char *topicName)
    {
        Topic *topic = topics->add(topicName);
        topic->subscribers->add(id);
    }

    void forEachTopic(void (*callback)(Topic *topic))
    {
        topics->forEach(callback);
    }
};