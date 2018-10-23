#include "mqtt_jni.h"
#include "mqtt_main.h"
#include <android/log.h>
#include <jni.h>
#include <pthread.h>
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
static pthread_key_t mThreadKey;
static JavaVM *mJavaVM;

static jobject mMqttObj;
static jmethodID midMessageCallback;

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

void mqtt_callback(const struct mosquitto_message *message)
{
    const unsigned char *payload = message->payload;
    JNIEnv *env = Android_JNI_GetEnv();
    int len = sizeof(payload);
    jbyteArray byteArray = (*env)->NewByteArray(env, sizeof(payload));
    (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte *)payload);
    (*env)->CallObjectMethod(env, mMqttObj, midMessageCallback, byteArray);
    (*env)->ReleaseByteArrayElements(env, byteArray, payload, JNI_COMMIT);
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

        status = main(argc, argv);

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