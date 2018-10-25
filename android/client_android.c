#include "mqtt_jni.h"
#include "mqtt_main.h"
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void message_callback(const struct mosquitto_message *message)
{
    char *log = message->payload;
    LOGE("mqtt_callback %s", log);
    const unsigned char *payload = message->payload;
    JNIEnv *env = Android_JNI_GetEnv();
    jstring topic = (*env)->NewStringUTF(env, message->topic);
    int len = message->payloadlen;
    jbyteArray byteArray = (*env)->NewByteArray(env, len);
    (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte *)payload);
    (*env)->CallVoidMethod(env, mMqttObj, midOnMessage, topic, byteArray);
    (*env)->ReleaseByteArrayElements(env, byteArray, (jbyte *)payload,
                                     JNI_COMMIT);
    (*env)->DeleteLocalRef(env, topic);
}

void connect_callback()
{
    JNIEnv *env = Android_JNI_GetEnv();
    (*env)->CallVoidMethod(env, mMqttObj, midOnConnect);
}

void log_callback(const char *str)
{
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
    jclass clazz = (*mEnv)->FindClass(mEnv, "com/mqtt/jni/MosquittoJNI");
    if (clazz)
    {
        midOnMessage = (*mEnv)->GetMethodID(mEnv, clazz, "onMessage", "(Ljava/lang/String;[B)V");
        midOnConnect = (*mEnv)->GetMethodID(mEnv, clazz, "onConnect", "()V");
        midOnDebugLog = (*mEnv)->GetMethodID(mEnv, clazz, "onDebugLog",
                                             "(Ljava/lang/String;)V");
    }
    return 1;
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

JNIEXPORT void JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeQuit(JNIEnv *mEnv,
                                                                 jobject obj)
{
    mqtt_quit();
}