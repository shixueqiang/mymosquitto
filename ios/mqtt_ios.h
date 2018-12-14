#ifndef MQTT_ANDROID_H
#define MQTT_ANDROID_H

#include <clientcore/mqtt_main.h>
#include <mosquitto.h>

// #define LOG_TAG "Mosquitto_Android"
// #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
// #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

typedef void (*ios_callback)(const struct mosquitto_message *message);

void mqtt_message_callback(const struct mosquitto_message *message);
void mqtt_connect_callback();
void mqtt_log_callback(const char *str);

#endif