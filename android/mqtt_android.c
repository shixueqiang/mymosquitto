#include "mqtt_jni.h"
#include "mqtt_android.h"
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strdup(const char *string)
{
    size_t len = strlen(string) + 1;
    char *newstr = (char *)malloc(sizeof(char) * len);
    if (newstr)
    {
        memcpy(newstr, string, len);
    }
    return newstr;
}

/*******************************************************************************
                               Globals
*******************************************************************************/
static pthread_key_t mThreadKey;
static JavaVM *mJavaVM;

static jobject mMqttObj;
static jmethodID midOnMessage;
static jmethodID midOnConnect;
static jmethodID midOnDebugLog;

JNIEnv *Android_JNI_GetEnv(void)
{
    JNIEnv *env;
    int status = (*mJavaVM)->AttachCurrentThread(mJavaVM, &env, NULL);
    if (status < 0)
    {
        LOGE("failed to attach current thread");
        return 0;
    }

    pthread_setspecific(mThreadKey, (void *)env);

    return env;
}

void mqtt_message_callback(const struct mosquitto_message *msg)
{
    const char *log = msg->payload;
    LOGE("mqtt_message_callback %s", log);
    mqtt_message *message = (mqtt_message *)malloc(sizeof(mqtt_message));
    memset(message, 0, sizeof(mqtt_message));
    int rc = unpack_message(msg->payload, msg->payloadlen, message);
    LOGE("mqtt_message_callback unpack_message rc %d message: %s topic: %s\n", rc, message->msg_payload, message->topic);
    JNIEnv *env = Android_JNI_GetEnv();
    jclass clazz = (*env)->FindClass(env, "com/mqtt/jni/ReceiveMessage");
    jfieldID fidMsgType = (*env)->GetFieldID(env, clazz, "msgType", "I");
    jfieldID fidMsgTimestamp = (*env)->GetFieldID(env, clazz, "msgTimestamp", "J");
    jfieldID fidClientId = (*env)->GetFieldID(env, clazz, "clientId", "Ljava/lang/String;");
    jfieldID fidTopic = (*env)->GetFieldID(env, clazz, "topic", "Ljava/lang/String;");
    jfieldID fidMsgId = (*env)->GetFieldID(env, clazz, "msgId", "Ljava/lang/String;");
    jfieldID fidMsgPayload = (*env)->GetFieldID(env, clazz, "msgPayload", "Ljava/lang/String;");
    jobject jReceiveMessage = (*env)->AllocObject(env, clazz);
    (*env)->SetIntField(env, jReceiveMessage, fidMsgType, message->msg_type);
    (*env)->SetLongField(env, jReceiveMessage, fidMsgTimestamp, message->msg_timestamp);
    jstring jClientId = (*env)->NewStringUTF(env, message->client_id);
    (*env)->SetObjectField(env, jReceiveMessage, fidClientId, jClientId);
    jstring jTopic = (*env)->NewStringUTF(env, message->topic);
    (*env)->SetObjectField(env, jReceiveMessage, fidTopic, jTopic);
    jstring jMsgId = (*env)->NewStringUTF(env, message->msg_id);
    (*env)->SetObjectField(env, jReceiveMessage, fidMsgId, jMsgId);
    jstring jMsgPayload = (*env)->NewStringUTF(env, message->msg_payload);
    (*env)->SetObjectField(env, jReceiveMessage, fidMsgPayload, jMsgPayload);
    // const unsigned char *payload = message->msg_payload;
    // jstring topic = (*env)->NewStringUTF(env, message->topic);
    // int len = strlen(message->msg_payload);
    // jbyteArray byteArray = (*env)->NewByteArray(env, len);
    // (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte *)payload);
    (*env)->CallVoidMethod(env, mMqttObj, midOnMessage, jReceiveMessage);
    // (*env)->ReleaseByteArrayElements(env, byteArray, (jbyte *)payload,
    //                                  JNI_COMMIT);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, jReceiveMessage);
    (*env)->DeleteLocalRef(env, jClientId);
    (*env)->DeleteLocalRef(env, jTopic);
    (*env)->DeleteLocalRef(env, jMsgId);
    (*env)->DeleteLocalRef(env, jMsgPayload);
    message__cleanup(&message);
}

void mqtt_connect_callback()
{
    LOGE("mqtt_connect_callback");
    JNIEnv *env = Android_JNI_GetEnv();
    (*env)->CallVoidMethod(env, mMqttObj, midOnConnect);
}

void mqtt_log_callback(const char *str)
{
    LOGE("mqtt_log_callback %s", str);
    JNIEnv *env = Android_JNI_GetEnv();
    jstring log = (*env)->NewStringUTF(env, str);
    (*env)->CallVoidMethod(env, mMqttObj, midOnDebugLog, log);
    (*env)->DeleteLocalRef(env, log);
}

static void Android_JNI_ThreadDestroyed(void *value)
{
    /* The thread is being destroyed, detach it from the Java VM and set the
   * mThreadKey value to NULL as required */
    JNIEnv *env = (JNIEnv *)value;
    if (env != NULL)
    {
        (*mJavaVM)->DetachCurrentThread(mJavaVM);
        pthread_setspecific(mThreadKey, NULL);
    }
}

int Android_JNI_SetupThread(void)
{
    Android_JNI_GetEnv();
    return 1;
}

/* Library init */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    mJavaVM = vm;
    LOGE("JNI_OnLoad called");
    if ((*mJavaVM)->GetEnv(mJavaVM, (void **)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return -1;
    }
    /*
   * Create mThreadKey so we can keep track of the JNIEnv assigned to each
   * thread Refer to
   * http://developer.android.com/guide/practices/design/jni.html for the
   * rationale behind this
   */
    if (pthread_key_create(&mThreadKey, Android_JNI_ThreadDestroyed) != 0)
    {
        LOGE("Error initializing pthread key");
    }
    Android_JNI_SetupThread();

    return JNI_VERSION_1_4;
}

JNIEXPORT jint JNICALL
Java_com_mqtt_jni_MosquittoJNI_nativeSetupJNI(JNIEnv *mEnv, jobject obj)
{
    LOGE("nativeSetupJNI");
    mMqttObj = (jobject)((*mEnv)->NewGlobalRef(mEnv, obj));
    jclass clazz = (*mEnv)->GetObjectClass(mEnv, obj);
    // jclass clazz = (*mEnv)->FindClass(mEnv, "com/mqtt/jni/MosquittoJNI");
    if (clazz)
    {
        midOnMessage = (*mEnv)->GetMethodID(mEnv, clazz, "onMessage",
                                            "(Lcom/mqtt/jni/ReceiveMessage;)V");
        midOnConnect = (*mEnv)->GetMethodID(mEnv, clazz, "onConnect", "()V");
        midOnDebugLog = (*mEnv)->GetMethodID(mEnv, clazz, "onDebugLog",
                                             "(Ljava/lang/String;)V");
    }
    (*mEnv)->DeleteLocalRef(mEnv, clazz);
    return 1;
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeRunMain(
    JNIEnv *mEnv, jobject obj, jobject arguments)
{
    int status = -1;
    int i;
    int argc;
    int len;

    /* Prepare the arguments. */
    len = (*mEnv)->GetArrayLength(mEnv, arguments);
    char *argv[len];
    argc = 0;
    LOGE("nativeRunMain");
    for (i = 0; i < len; ++i)
    {
        const char *utf;
        char *arg = NULL;
        jstring string = (*mEnv)->GetObjectArrayElement(mEnv, arguments, i);
        if (string)
        {
            utf = (*mEnv)->GetStringUTFChars(mEnv, string, 0);
            if (utf)
            {
                arg = strdup(utf);
                (*mEnv)->ReleaseStringUTFChars(mEnv, string, utf);
            }
            (*mEnv)->DeleteLocalRef(mEnv, string);
        }
        if (!arg)
        {
            arg = strdup("");
        }
        argv[argc++] = arg;
    }
    status = mqtt_main(argc, argv);

    /* Release the arguments. */
    for (i = 0; i < argc; ++i)
    {
        free(argv[i]);
    }
    return status;
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_subscribe(
    JNIEnv *mEnv, jobject obj, jobjectArray topics, jint qos)
{
    int i;
    int len;
    int status;
    len = (*mEnv)->GetArrayLength(mEnv, topics);
    for (i = 0; i < len; ++i)
    {
        const char *utf;
        jstring string = (*mEnv)->GetObjectArrayElement(mEnv, topics, i);
        if (string)
        {
            utf = (*mEnv)->GetStringUTFChars(mEnv, string, 0);
            LOGE("subscribe utf %s", utf);
            if (utf)
            {
                status = mqtt_subscribe(utf, qos);
                (*mEnv)->ReleaseStringUTFChars(mEnv, string, utf);
            }
            (*mEnv)->DeleteLocalRef(mEnv, string);
        }
    }
    return status;
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_unsubscribe(
    JNIEnv *mEnv, jobject obj, jobjectArray topics)
{
    int i;
    int len;
    int status;
    len = (*mEnv)->GetArrayLength(mEnv, topics);
    for (i = 0; i < len; ++i)
    {
        const char *utf;
        jstring string = (*mEnv)->GetObjectArrayElement(mEnv, topics, i);
        if (string)
        {
            utf = (*mEnv)->GetStringUTFChars(mEnv, string, 0);
            if (utf)
            {
                status = mqtt_unsubscribe(utf);
                (*mEnv)->ReleaseStringUTFChars(mEnv, string, utf);
            }
            (*mEnv)->DeleteLocalRef(mEnv, string);
        }
    }
    return status;
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_publish(
    JNIEnv *mEnv, jobject obj, jint jmsgType, jstring jtopic, jstring jpayload, jint qos)
{
    int status = 0;
    char *topic = (char *)(*mEnv)->GetStringUTFChars(mEnv, jtopic, JNI_FALSE);
    // int len = (*mEnv)->GetArrayLength(mEnv, jpayload);
    // unsigned char *payload = (char *)malloc(sizeof(char) * (len + 1));
    // memset(payload, 0, sizeof(char) * (len + 1));
    // (*mEnv)->GetByteArrayRegion(mEnv, jpayload, 0, len, (jbyte *)payload);
    char *payload = (char *)(*mEnv)->GetStringUTFChars(mEnv, jpayload, JNI_FALSE);
    status = mqtt_publish(jmsgType, topic, payload, qos);
    (*mEnv)->ReleaseStringUTFChars(mEnv, jtopic, topic);
    (*mEnv)->ReleaseStringUTFChars(mEnv, jpayload, payload);
    return status;
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeLogout(JNIEnv *mEnv, jobject obj)
{
    mqtt_logout();
}

JNIEXPORT void JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeQuit(JNIEnv *mEnv,
                                                                 jobject obj)
{
    mqtt_quit();
}