#pragma once

#include <Arduino.h>
#include <TopicList.h>
#include <SubList.h>
#include <TopicFile.h>

void printSub(const char *mac)
{
    Serial.printf("\t%s\n", mac);
}

void printTopic(Topic *topic)
{
    Serial.println(topic->name);
    topic->subscribers->forEach(printSub);
}

class EventBroker
{
private:
    TopicList *topics = new TopicList;

public:
    EventBroker()
    {
        // TopicFile::remove("/subs.txt");
    }

    void load()
    {
        Serial.println("Subscription loading");
        TopicFile::load("/subs.txt", topics);
        topics->forEach(printTopic);
    }

    void save()
    {
        Serial.println("Subscription saving");
        TopicFile::save("/subs.txt", topics);
        topics->forEach(printTopic);
    }

    SubList *publish(const char *topicName, const char *data)
    {
        // если уже есть такой топик, то он и вовзращается, нет - создется
        Topic *topic = topics->add(topicName);
        // замена данных
        if (topic->data != NULL)
        {
            // если было что-то, то удаляется
            free(topic->data);
        }
        // установка новых
        topic->data = strdup(data);
        // возвращается указатель на список подписчиков
        return topic->subscribers;
    }

    void subscribe(const char *id, const char *topicName)
    {
        Topic *topic = topics->find(topicName);
        // сохранить подписки только для новых подписчиков
        bool need_to_save = topic == NULL || topic->subscribers->find(id) == NULL;

        topic = topics->add(topicName);
        topic->subscribers->add(id);
        
        if(need_to_save)
        {
            save();
        }
    }

    void forEachTopic(void (*callback)(Topic *topic))
    {
        topics->forEach(callback);
    }
};