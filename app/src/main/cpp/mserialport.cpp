#include <jni.h>
#include <SerialPortManager.h>
#include <androidLog.h>
#include <SPReadWriteWorker.h>
#include <random>
#include <unistd.h>

static SerialPortManager *mManager;
static JavaVM *g_vm;
//用于储存读回调
static std::unordered_map<std::string, jobject> g_callback_map;

const char *ConvertJByteaArrayToChars(JNIEnv *env, jbyteArray *bytearray) {
    char *chars = nullptr;
    jbyte *bytes;
    bytes = env->GetByteArrayElements(*bytearray, nullptr);
    int chars_len = env->GetArrayLength(*bytearray);
    LOGD("Char len %d", chars_len);
    chars = new char[chars_len + 1];
    memset(chars, 0, static_cast<size_t>(chars_len + 1));
    memcpy(chars, bytes, static_cast<size_t>(chars_len));
    chars[chars_len] = 0;
    env->ReleaseByteArrayElements(*bytearray, bytes, 0);
    return chars;
}

std::vector<char> ConvertJByteArrayToVectorOfChars(JNIEnv *env, jbyteArray *bytearray) {
    jbyte *bytes;
    bytes = env->GetByteArrayElements(*bytearray, nullptr);
    int chars_len = env->GetArrayLength(*bytearray);
    std::vector<char> result(bytes, bytes + chars_len);
    env->ReleaseByteArrayElements(*bytearray, bytes, 0);
    return result;
}

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
        env->DeleteLocalRef(message);
    }
    auto name = std::string(path_utf);
    mManager->sendMessage(name, msgs);
    env->ReleaseStringUTFChars(path, path_utf);
}

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_sendBytes(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jobjectArray commands
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    int stringCount = env->GetArrayLength(commands);
    auto name = std::string(path_utf);
    for (int i = 0; i < stringCount; ++i) {
        auto message = static_cast<jbyteArray >(env->GetObjectArrayElement(commands, i));
        auto msg = ConvertJByteArrayToVectorOfChars(env, &message);
        mManager->sendBytesMessage(name, msg);
        env->DeleteLocalRef(message);
    }
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
    mManager->removeSerialPort(name);
    g_callback_map.erase(path_utf);
    env->ReleaseStringUTFChars(path, path_utf);
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
Java_com_castle_serialport_SerialPortManager_openSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate,
        jobject callback
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    if (mManager->hasSerialPort(path_utf)) {
        LOGE("请不要重复添加串口,路径%s", path_utf);
        return;
    }
    auto name = std::string(path_utf);
    if (callback != nullptr) {
        g_callback_map[path_utf] = env->NewGlobalRef(callback);
        mManager->addSerialPort(path_utf,
                                std::make_unique<SPReadWriteWorker>(name, baudRate, g_vm,
                                                                    &g_callback_map[name]));
        mManager->sendMessage(name, {START_READ});
    } else {
        mManager->addSerialPort(path_utf,
                                std::make_unique<SPReadWriteWorker>(name, baudRate,
                                                                    nullptr,
                                                                    nullptr));
    }
    env->ReleaseStringUTFChars(path, path_utf);
}
