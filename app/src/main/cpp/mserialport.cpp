#include <jni.h>
#include <SerialPortManager.h>
#include <androidLog.h>
#include <random>

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

static JavaVM *g_vm;

struct ReadStruct {
    jobject jcallback;
    std::string name;

    void operator()(std::string msg) const {
        JNIEnv *jenv;
        if (msg == PFBackgroundService::DESTROY) {
            //释放当前线程
            g_vm->DetachCurrentThread();
            //释放你的全局引用的接口，生命周期自己把控
            jenv->DeleteGlobalRef(jcallback);
            jenv = nullptr;

        } else {
            //获取当前native线程是否有没有被附加到jvm环境中
            int getEnvStat = g_vm->GetEnv(reinterpret_cast<void **>(&jenv), JNI_VERSION_1_6);
            if (getEnvStat == JNI_EDETACHED) {
                //如果没有， 主动附加到jvm环境中，获取到env
                if (g_vm->AttachCurrentThread(&jenv, nullptr) != 0) {
                    mManager.sendMessage(name, PFBackgroundService::DESTROY);
                    return;
                }
            }

            //通过强转后的jcallback 获取到要回调的类
            jclass javaClass = jenv->GetObjectClass(jcallback);

            if (javaClass == 0) {
                LOGE("Unable to find class");
                mManager.sendMessage(name, PFBackgroundService::DESTROY);
                return;
            }

            //获取要回调的方法ID
            jmethodID javaCallbackId = jenv->GetMethodID(javaClass,
                                                         "onDataReceived", "([B)V");
            if (javaCallbackId == nullptr) {
                LOGE("Unable to find method:onProgressCallBack");
                mManager.sendMessage(name, PFBackgroundService::DESTROY);
                return;
            }
            std::string data;
            while (true) {
                mManager.getSerialPort(name).Read(data);
                //执行回调
                jenv->CallVoidMethod(jcallback, javaCallbackId, StringToJByteArray(jenv, data));
            }
        }
    }
} mReadFun;

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_testRead(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate,
        jobject callback
) {
    ReadStruct mStruct;
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    auto name = std::string(path_utf);
    mStruct.name = name;
    env->GetJavaVM(&g_vm);
    mStruct.jcallback = env->NewGlobalRef(callback);

    std::string start = "start";
    mManager.addSerialPort(mStruct, name, (int) baudRate);
    mManager.sendMessage(name, start);
    env->ReleaseStringUTFChars(path, path_utf);
}
