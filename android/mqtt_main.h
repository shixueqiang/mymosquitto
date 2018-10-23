#ifndef MQTT_MAIN_H
#define MQTT_MAIN_H
#include <mosquitto.h>

typedef void (*android_callback)(const struct mosquitto_message *message);

int mqtt_subscribe(char *topic);
int mqtt_unsubscribe(char *topic);
int mqtt_quit();

void mqtt_callback(const struct mosquitto_message *message);

#endif