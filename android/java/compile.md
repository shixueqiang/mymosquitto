javac -classpath /home/shixq/android/sdk/platforms/android-23/android.jar -sourcepath ./ com/mqtt/jni/MosquittoJNI.java

javah -jni -classpath /home/shixq/android/sdk/platforms/android-23/android.jar:/home/shixq/workspaces/mqttim/mosquitto/android/java com.mqtt.jni.MosquittoJNI