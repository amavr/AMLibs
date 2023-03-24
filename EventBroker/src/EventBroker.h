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
    SubList *tmp_subs = new SubList;

public:
    EventBroker()
    {
        topics->add("#");
        TopicFile::remove("/subs.txt");
    }

    void load()
    {
        TopicFile::load("/subs.txt", topics);
        topics->forEach(printTopic);
        topics->add("#");
    }

    void save()
    {
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

        // указатель на список подписчиков
        SubList *subs = topic->subscribers;
        // подписчики на все топики
        tmp_subs = topics->find("#")->subscribers;
        // объединение подписчиков в один список
        tmp_subs->addList(subs);
        return tmp_subs;
    }

    SubList *getSubscribers(const char *topicName)
    {
        // если уже есть такой топик, то он и вовзращается, нет - создется
        Topic *topic = topics->find(topicName);
        if (topic == NULL)
        {
            return NULL;
        }
        else
        {
            // возвращается указатель на список подписчиков
            return topic->subscribers;
        }
    }

    void subscribe(const char *id, const char *topicName)
    {
        Topic *topic = topics->find(topicName);
        // сохранить подписки только для новых подписчиков
        bool need_to_save = topic == NULL || topic->subscribers->find(id) == NULL;

        topic = topics->add(topicName);
        topic->subscribers->add(id);

        if (need_to_save)
        {
            save();
        }
    }

    void forEachTopic(void (*callback)(Topic *topic))
    {
        topics->forEach(callback);
    }
};