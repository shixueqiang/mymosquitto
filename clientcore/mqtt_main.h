#ifndef MQTT_MAIN_H
#define MQTT_MAIN_H
#include <mosquitto.h>
#include "mqtt_message.h"
#ifdef __cplusplus
extern "C"
{
#endif
    int mqtt_main(int argc, char *argv[]);
    int mqtt_subscribe(const char *topic, int qos);
    int mqtt_unsubscribe(const char *topic);
    int mqtt_publish(uint8_t msg_type, const char *topic, const char *payload, int qos);
    /*　注销*/
    int mqtt_logout();
    /* 异常退出*/
    int mqtt_quit();
#ifdef __cplusplus
}
#endif
#endif