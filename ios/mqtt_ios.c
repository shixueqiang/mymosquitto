
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void mqtt_message_callback(const struct mosquitto_message *msg)
{
    // const char *log = msg->payload;
    // LOGE("mqtt_message_callback %s", log);
    // mqtt_message *message = (mqtt_message *)malloc(sizeof(mqtt_message));
    // memset(message, 0, sizeof(mqtt_message));
    // int rc = unpack_message(msg->payload, msg->payloadlen, message);
    // LOGE("mqtt_message_callback unpack_message rc %d message: %s topic: %s\n", rc, message->msg_payload, message->topic);
    // JNIEnv *env = Android_JNI_GetEnv();
    // jclass clazz = (*env)->FindClass(env, "com/mqtt/jni/ReceiveMessage");
    // jfieldID fidMsgType = (*env)->GetFieldID(env, clazz, "msgType", "I");
    // jfieldID fidMsgTimestamp = (*env)->GetFieldID(env, clazz, "msgTimestamp", "J");
    // jfieldID fidClientId = (*env)->GetFieldID(env, clazz, "clientId", "Ljava/lang/String;");
    // jfieldID fidTopic = (*env)->GetFieldID(env, clazz, "topic", "Ljava/lang/String;");
    // jfieldID fidMsgId = (*env)->GetFieldID(env, clazz, "msgId", "Ljava/lang/String;");
    // jfieldID fidMsgPayload = (*env)->GetFieldID(env, clazz, "msgPayload", "Ljava/lang/String;");
    // jobject jReceiveMessage = (*env)->AllocObject(env, clazz);
    // (*env)->SetIntField(env, jReceiveMessage, fidMsgType, message->msg_type);
    // (*env)->SetLongField(env, jReceiveMessage, fidMsgTimestamp, message->msg_timestamp);
    // jstring jClientId = (*env)->NewStringUTF(env, message->client_id);
    // (*env)->SetObjectField(env, jReceiveMessage, fidClientId, jClientId);
    // jstring jTopic = (*env)->NewStringUTF(env, message->topic);
    // (*env)->SetObjectField(env, jReceiveMessage, fidTopic, jTopic);
    // jstring jMsgId = (*env)->NewStringUTF(env, message->msg_id);
    // (*env)->SetObjectField(env, jReceiveMessage, fidMsgId, jMsgId);
    // jstring jMsgPayload = (*env)->NewStringUTF(env, message->msg_payload);
    // (*env)->SetObjectField(env, jReceiveMessage, fidMsgPayload, jMsgPayload);
    // // const unsigned char *payload = message->msg_payload;
    // // jstring topic = (*env)->NewStringUTF(env, message->topic);
    // // int len = strlen(message->msg_payload);
    // // jbyteArray byteArray = (*env)->NewByteArray(env, len);
    // // (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte *)payload);
    // (*env)->CallVoidMethod(env, mMqttObj, midOnMessage, jReceiveMessage);
    // // (*env)->ReleaseByteArrayElements(env, byteArray, (jbyte *)payload,
    // //                                  JNI_COMMIT);
    // (*env)->DeleteLocalRef(env, clazz);
    // (*env)->DeleteLocalRef(env, jReceiveMessage);
    // (*env)->DeleteLocalRef(env, jClientId);
    // (*env)->DeleteLocalRef(env, jTopic);
    // (*env)->DeleteLocalRef(env, jMsgId);
    // (*env)->DeleteLocalRef(env, jMsgPayload);
    // message__cleanup(&message);
}

void mqtt_connect_callback()
{
    // LOGE("mqtt_connect_callback");
    // JNIEnv *env = Android_JNI_GetEnv();
    // (*env)->CallVoidMethod(env, mMqttObj, midOnConnect);
}

void mqtt_log_callback(const char *str)
{
    // LOGE("mqtt_log_callback %s", str);
    // JNIEnv *env = Android_JNI_GetEnv();
    // jstring log = (*env)->NewStringUTF(env, str);
    // (*env)->CallVoidMethod(env, mMqttObj, midOnDebugLog, log);
    // (*env)->DeleteLocalRef(env, log);
}

