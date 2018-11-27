#ifndef MQTT_MAIN_H
#define MQTT_MAIN_H
#include <android/log.h>
#include <mosquitto.h>
#include <mqtt_message.h>
#define LOG_TAG "Mosquitto_Android"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

typedef void (*android_callback)(const struct mosquitto_message *message);

int mqtt_main(int argc, char *argv[]);
int mqtt_subscribe(const char *topic, int qos);
int mqtt_unsubscribe(const char *topic);
int mqtt_publish(char *topic, void *payload, int qos);
int mqtt_quit();

void mqtt_message_callback(const struct mosquitto_message *message);
void mqtt_connect_callback();
void mqtt_log_callback(const char *str);

#endif