#ifndef MQTT_SUB_H
#define MQTT_SUB_H
#include <android/log.h>
#include <mosquitto.h>
#define LOG_TAG "Mosquitto_Android"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define MQTT_MESSAGE_TEXT 0x1
#define MQTT_MESSAGE_IMG 0x2
#define MQTT_MESSAGE_VOICE 0x3
#define MQTT_MESSAGE_VIDEO 0x4
#define MQTT_MESSAGE_FILE 0x5

typedef void (*android_callback)(const struct mosquitto_message *message);
typedef struct
{
    uint8_t msg_type;/* 消息类型*/
    uint16_t msg_seq;/* 序号，每次+1*/
    uint32_t msg_time;/* 时间戳*/
    uint16_t msg_id_len;/* 消息id长度*/
    char* msg_id;/* 消息id*/
    char* msg_payload;/* 消息体*/
} mqtt_message;

int mqtt_main(int argc, char *argv[]);
int mqtt_subscribe(const char *topic, int qos);
int mqtt_unsubscribe(const char *topic);
int mqtt_publish(const char *topic, const void *payload, int qos);
int mqtt_quit();

void mqtt_message_callback(const struct mosquitto_message *message);
void mqtt_connect_callback();
void mqtt_log_callback(const char *str);

#endif