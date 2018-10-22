package com.mqtt.jni;

public interface MessageListener {
    public void onMessage(byte[] message);
}