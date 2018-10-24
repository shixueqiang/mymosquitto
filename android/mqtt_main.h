#ifndef MQTT_MAIN_H
#define MQTT_MAIN_H
#include <mosquitto.h>

typedef void (*android_callback)(const struct mosquitto_message *message);

int mqtt_main(int argc, char *argv[]);
int mqtt_subscribe(const char *topic);
int mqtt_unsubscribe(const char *topic);
int mqtt_quit();

void mqtt_callback(const struct mosquitto_message *message);

#endif