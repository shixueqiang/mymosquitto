/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_mqtt_jni_MosquittoJNI */

#ifndef _Included_com_mqtt_jni_MosquittoJNI
#define _Included_com_mqtt_jni_MosquittoJNI
#define main client_main
#ifdef __cplusplus
extern "C"
{
#endif
  /*
 * Class:     com_mqtt_jni_MosquittoJNI
 * Method:    nativeSetupJNI
 * Signature: ()I
 */
  JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeSetupJNI(JNIEnv *,
                                                                       jobject);

  /*
 * Class:     com_mqtt_jni_MosquittoJNI
 * Method:    nativeRunMain
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
  JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeRunMain(JNIEnv *,
                                                                      jobject,
                                                                      jstring,
                                                                      jobject);

  /*
 * Class:     com_mqtt_jni_MosquittoJNI
 * Method:    subscribe
 * Signature: ([Ljava/lang/String;)I
 */
  JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_subscribe(JNIEnv *,
                                                                  jobject,
                                                                  jobjectArray);

  /*
 * Class:     com_mqtt_jni_MosquittoJNI
 * Method:    unsubscribe
 * Signature: ([Ljava/lang/String;)I
 */
  JNIEXPORT jint JNICALL Java_com_mqtt_jni_MosquittoJNI_unsubscribe(JNIEnv *,
                                                                    jobject,
                                                                    jobjectArray);

  /*
 * Class:     com_mqtt_jni_MosquittoJNI
 * Method:    nativeQuit
 * Signature: ()V
 */
  JNIEXPORT void JNICALL Java_com_mqtt_jni_MosquittoJNI_nativeQuit(JNIEnv *,
                                                                   jobject);

#ifdef __cplusplus
}
#endif
#endif
