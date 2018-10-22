#include "mqtt_jni.h"
#include "mqtt_main.h"
#include <android/log.h>
#include <jni.h>
#include <stdio.h>
#include <string.h>

#define LOG_TAG "Mosquitto_Android"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

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
static jobject mMqttObj;
static jmethodID midMessageCallback;

/* main function prototype */
typedef int (*MQTT_MAIN)(int argc, char *argv[]);

JNIEXPORT jint JNICALL
Java_com_mqtt_jni_MosquittoJNI_nativeSetupJNI(JNIEnv *mEnv, jobject obj)
{
    LOGE("nativeSetupJNI");
    mMqttObj = (jobject)((*mEnv)->NewGlobalRef(mEnv, obj));
    jclass clazz = (*mEnv)->FindClass(mEnv, "com/mqtt/jni/MosquittoJNI");
    midMessageCallback =
        (*mEnv)->GetMethodID(mEnv, clazz, "messageCallback", "([B])V");
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeRunMain(
    JNIEnv *mEnv, jobject obj, jstring function, jobject arguments)
{
    int status = -1;
    int i;
    int argc;
    int len;

    /* Prepare the arguments. */
    len = (*mEnv)->GetArrayLength(mEnv, arguments);
    char *argv[len];
    argc = 0;
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

        status = SDL_main(argc, argv);

        /* Release the arguments. */
        for (i = 0; i < argc; ++i)
        {
            free(argv[i]);
        }
    }
    return status;
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_subscribe(
    JNIEnv *mEnv, jobject obj, jobjectArray topics)
{
    char *utf;
    utf = (*mEnv)->GetStringUTFChars(mEnv, topics, NULL);
    mqtt_subscribe(utf);
    (*mEnv)->ReleaseStringUTFChars(mEnv, topics, utf);
}

JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_unsubscribe(
    JNIEnv *mEnv, jobject obj, jobjectArray topics)
{
    char *utf;
    utf = (*mEnv)->GetStringUTFChars(mEnv, topics, NULL);
    mqtt_unsubscribe(utf);
    (*mEnv)->ReleaseStringUTFChars(mEnv, topics, utf);
}

JNIEXPORT void JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeQuit(JNIEnv *mEnv,
                                                                 jobject obj)
{
    mqtt_quit();
}