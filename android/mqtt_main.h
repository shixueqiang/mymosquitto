#ifndef MQTT_MAIN_H
#define MQTT_MAIN_H

#define main MQTT_MAIN

int mqtt_init();
int mqtt_subscribe(char *topic);
int mqtt_unsubscribe(char *topic);
int mqtt_quit();

#endif