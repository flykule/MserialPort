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
Java_com_castle_serialport_SerialPortManager_openWriteSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    std::string name = std::string(path_utf);
    SPWriteWorker worker = SPWriteWorker(name.c_str(), &baudRate);
    mManager.addSerialPort(name, SerialPortManager::FLAG_WRITE, &worker);
    env->ReleaseStringUTFChars(path, path_utf);
}

static JavaVM *g_vm;
//用于储存读回调
static std::unordered_map<std::string, jobject> g_callback_map;
static constexpr auto start = "start";

jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    g_vm = jvm;
    JNIEnv *env;
    LOGD("JNI onload been called");
    if (g_vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        std::__throw_runtime_error("Get java env failed!");
        return -1;
    }
    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_openReadSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate,
        jobject callback
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    auto name = std::string(path_utf);
    g_callback_map[name] = env->NewGlobalRef(callback);
    auto p = new SPReadWorker(name.c_str(), &baudRate, g_vm, &g_callback_map[name]);
    mManager.addSerialPort(name, SerialPortManager::FLAG_READ, p);
    mManager.sendMessage(name, start, SerialPortManager::FLAG_READ);
    env->ReleaseStringUTFChars(path, path_utf);
}
