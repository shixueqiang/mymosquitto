package com.mqtt.jni;

public class MosquittoJNI {
  private static volatile MosquittoJNI mInstance = null;
  private MessageListener messageListener;

  static {
    System.loadLibrary("mosquitto");
    System.loadLibrary("mosquitto_sub");
  }

  private MosquittoJNI() {
  }

  public static MosquittoJNI getInstance() {
    if (mInstance == null) {
      synchronized (MosquittoJNI.class) {
        if (mInstance == null) {
          mInstance = new MosquittoJNI();
        }
      }
    }
    return mInstance;
  }

  public native int nativeSetupJNI();

  public native int nativeRunMain(String function, Object arguments);

  public native int subscribe(String[] topics);

  public native int unsubscribe(String[] topics);

  public native void nativeQuit();

  public void messageCallback(byte[] message) {
    if (messageListener != null) {
      messageListener.onMessage(message);
    }
  }

  private void logCallback(String log) {
  }

  public void setMessageListener(MessageListener messageListener) {
    this.messageListener = messageListener;
  }
}