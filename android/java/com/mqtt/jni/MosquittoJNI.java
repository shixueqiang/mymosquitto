package com.mqtt.jni;

class MosquittoJNI {
  private static volatile MosquittoJNI mInstance = null;
  private MessageListener messageListener;

  private MosquittoJNI() {}

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

  private native int nativeSetupJNI();

  private native int nativeRunMain(String function, Object arguments);

  private native int subscribe(String[] topics);

  private native int unsubscribe(String[] topics);

  private native void nativeQuit();

  private void messageCallback(byte[] message) {
    if (messageListener != null) {
      messageListener.onMessage(message);
    }
  }

  private void logCallback(String log) {}
}