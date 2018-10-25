package com.mqtt.jni;

public interface MessageListener {
    void onMessage(byte[] message);
    void onConnect();
    void onDebugLog(String log);
}