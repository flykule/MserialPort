#include <jni.h>
#include <SerialPortManager.h>
#include <androidLog.h>
#include <random>
#include <SPWriteWorker.h>

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
    mManager.sendMessage(name, message, SerialPortManager::FLAG_WRITE);
    env->ReleaseStringUTFChars(path, path_utf);
    env->ReleaseStringUTFChars(msg, msg_utf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_closeSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path
) {
//    const char *path_utf = env->GetStringUTFChars(path, nullptr);
//    auto name = std::string(path_utf);
//    mManager.removeSerialPort(name);
//    env->ReleaseStringUTFChars(path, path_utf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_openSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate,
        jint flags
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    std::string name = std::string(path_utf);
    if (flags & SerialPortManager::FLAG_WRITE) {
        SPWriteWorker worker = SPWriteWorker(name.c_str(),&baudRate);
        mManager.addSerialPort(name, (int) flags,&worker);
    }
    env->ReleaseStringUTFChars(path, path_utf);
}


static JavaVM *g_vm;
static constexpr auto start = "start";

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_testRead(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate,
        jobject callback
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    auto name = std::string(path_utf);
    env->GetJavaVM(&g_vm);

    auto p = new SPReadWorker(name.c_str(),&baudRate,g_vm,&callback);
    mManager.addSerialPort(name, (int) baudRate,p);
    mManager.sendMessage(name, start,SerialPortManager::FLAG_READ);
    env->ReleaseStringUTFChars(path, path_utf);
}
