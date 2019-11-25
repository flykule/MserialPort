#include <jni.h>
#include <SerialPortManager.h>
#include <androidLog.h>
#include <random>
#include <SPWriteWorker.h>
#include <unistd.h>

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
        jobjectArray commands
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    int stringCount = env->GetArrayLength(commands);
    std::vector<std::string> msgs;
    for (int i = 0; i < stringCount; ++i) {
        auto message = static_cast<jstring>(env->GetObjectArrayElement(commands, i));
        const char *msg_utf = env->GetStringUTFChars(message, nullptr);
        msgs.emplace_back(std::string(msg_utf));
        env->ReleaseStringUTFChars(message, msg_utf);
    }
    auto name = std::string(path_utf);
    mManager->sendMessage(name, msgs, SerialPortManager::FLAG_WRITE);
    env->ReleaseStringUTFChars(path, path_utf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_closeSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    auto name = std::string(path_utf);
    mManager->removeSerialPort(name, SerialPortManager::FLAG_WRITE | SerialPortManager::FLAG_READ);
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
    std::string name = std::string(path_utf);
    mManager->addSerialPort(path_utf, SerialPortManager::FLAG_WRITE,
                            std::make_unique<SPWriteWorker>(name.c_str(), &baudRate));
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

static jbyteArray StringToJByteArray(JNIEnv *env, const std::string &nativeString) {
    jbyteArray arr = env->NewByteArray(nativeString.length());
    env->SetByteArrayRegion(arr, 0, nativeString.length(), (jbyte *) nativeString.c_str());
    return arr;
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
    g_callback_map[path_utf] = env->NewGlobalRef(callback);
    mManager->addSerialPort(path_utf, SerialPortManager::FLAG_READ,
                            std::make_unique<SPReadWorker>(name.c_str(), &baudRate, g_vm,
                                                           &g_callback_map[name]));
    mManager->sendMessage(name, {start}, SerialPortManager::FLAG_READ);
    env->ReleaseStringUTFChars(path, path_utf);
}
