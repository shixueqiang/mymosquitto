#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H
#include <stdint.h>
#include <msgpack.h>
#define MQTT_MESSAGE_TEXT 0x1
#define MQTT_MESSAGE_IMG 0x2
#define MQTT_MESSAGE_VOICE 0x3
#define MQTT_MESSAGE_VIDEO 0x4
#define MQTT_MESSAGE_FILE 0x5
typedef struct
{
    uint8_t msg_type;        /* 消息类型*/
    uint32_t msg_timestamp;  /* 时间戳*/
    const char *client_id;   /* 设备id*/
    const char *topic;       /* 主题*/
    const char *msg_id;      /* 消息id*/
    const char *msg_payload; /* 消息体*/
} mqtt_message;

static int pack_message(mqtt_message *message, msgpack_sbuffer *sbuf)
{
    msgpack_packer pk;
    /* serialize values into the buffer using msgpack_sbuffer_write callback function. */
    msgpack_packer_init(&pk, sbuf, msgpack_sbuffer_write);

    msgpack_pack_array(&pk, 6);
    msgpack_pack_uint8(&pk, message->msg_type);
    msgpack_pack_uint32(&pk, message->msg_timestamp);

    size_t client_id_len = strlen(message->client_id);
    msgpack_pack_str(&pk, client_id_len);
    msgpack_pack_str_body(&pk, message->client_id, client_id_len);

    size_t topic_len = strlen(message->topic);
    msgpack_pack_str(&pk, topic_len);
    msgpack_pack_str_body(&pk, message->topic, topic_len);

    size_t msg_id_len = strlen(message->msg_id);
    msgpack_pack_str(&pk, msg_id_len);
    msgpack_pack_str_body(&pk, message->msg_id, msg_id_len);

    size_t msg_payload_len = strlen(message->msg_payload);
    msgpack_pack_str(&pk, msg_payload_len);
    msgpack_pack_str_body(&pk, message->msg_payload, msg_payload_len);
    free(message);
    return 0;
}

static int unpack_message(char *data, size_t size, mqtt_message *message)
{
    msgpack_zone mempool;
    msgpack_object deserialized;

    /* deserialize the buffer into msgpack_object instance. */
    /* deserialized object is valid during the msgpack_zone instance alive. */
    msgpack_zone_init(&mempool, 2048);
    int rc = msgpack_unpack(data, size, NULL, &mempool, &deserialized);
    if (rc == MSGPACK_UNPACK_SUCCESS)
    {
        /* print the deserialized object. */
        if (deserialized.type == MSGPACK_OBJECT_ARRAY)
        {
            if (deserialized.via.array.size != 0)
            {
                msgpack_object *p = deserialized.via.array.ptr;
                message->msg_type = p++->via.u64;
                message->msg_timestamp = p++->via.u64;
                message->client_id = p++->via.str.ptr;
                message->topic = p++->via.str.ptr;
                message->msg_id = p++->via.str.ptr;
                message->msg_payload = p++->via.str.ptr;
            }
        }
        else
        {
            message->msg_payload = "parse mqtt_message error";
            return MSGPACK_UNPACK_PARSE_ERROR;
        }
    }
    else
    {
        message->msg_payload = "parse mqtt_message error";
        return rc;
    }

    msgpack_zone_destroy(&mempool);
    free(data);
    return MSGPACK_UNPACK_SUCCESS;
}
#endif