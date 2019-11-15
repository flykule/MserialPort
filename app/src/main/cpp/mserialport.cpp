#include <jni.h>
#include <SerialPortManager.h>
#include <androidLog.h>

SerialPortManager mManager;

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_sendMessage(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jstring msg
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    const char *msg_utf = env->GetStringUTFChars(msg, nullptr);
    auto message = std::string(msg_utf);
    auto name = std::string(path_utf);
//    LOGD("发送消息%s给目标%s", msg_utf, path_utf);
    mManager.sendMessage(name, message);
    env->ReleaseStringUTFChars(path, path_utf);
    env->ReleaseStringUTFChars(msg, msg_utf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_closeSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    auto name = std::string(path_utf);
    mManager.removeSerialPort(name);
    env->ReleaseStringUTFChars(path, path_utf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_openSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    auto name = std::string(path_utf);
    mManager.addSerialPort(name, (int) baudRate);
    env->ReleaseStringUTFChars(path, path_utf);
}

jbyteArray StringToJByteArray(JNIEnv *env, const std::string &nativeString) {
    jbyteArray arr = env->NewByteArray(nativeString.length());
    env->SetByteArrayRegion(arr, 0, nativeString.length(), (jbyte *) nativeString.c_str());
    return arr;
}

JavaVM *g_VM;

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_castle_serialport_SerialPortManager_testRead(
        JNIEnv *env,
        jobject thiz,
        jstring path
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    auto name = std::string(path_utf);
    SerialPort serialPort(name, 9600);
    serialPort.Open();
    std::string data;
    serialPort.Read(data);
    env->ReleaseStringUTFChars(path, path_utf);
    serialPort.Close();
    return StringToJByteArray(env, data);
}
