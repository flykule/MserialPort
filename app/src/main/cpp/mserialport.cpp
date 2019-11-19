#include <jni.h>
#include <SerialPortManager.h>
#include <androidLog.h>
#include <random>
#include <SPWriteWorker.h>

static SerialPortManager *mManager;
static JavaVM *g_vm;
//用于储存读回调
static std::unordered_map<std::string, jobject> g_callback_map;
static constexpr auto start = "start";

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
    mManager->sendMessage(path_utf, message, SerialPortManager::FLAG_WRITE);
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
    mManager->removeSerialPort(path_utf, SerialPortManager::FLAG_WRITE | SerialPortManager::FLAG_READ);
    g_callback_map.erase(path_utf);
    env->ReleaseStringUTFChars(path, path_utf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_openWriteSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    mManager->addSerialPort(path_utf, SerialPortManager::FLAG_WRITE,
                           new SPWriteWorker(path_utf, &baudRate));
    env->ReleaseStringUTFChars(path, path_utf);
    LOGD("打开写串口成功");
}


jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    mManager = new SerialPortManager();
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
    g_callback_map[path_utf] = env->NewGlobalRef(callback);
    mManager->addSerialPort(path_utf, SerialPortManager::FLAG_READ,
                           new SPReadWorker(path_utf, &baudRate, g_vm, &g_callback_map[path_utf]));
    mManager->sendMessage(path_utf, start, SerialPortManager::FLAG_READ);
    env->ReleaseStringUTFChars(path, path_utf);
}
