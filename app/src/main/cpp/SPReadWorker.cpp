//
// Created by Administrator on 2019/11/18.
//

#include "includes/SPReadWorker.h"

static jbyteArray StringToJByteArray(JNIEnv *env, const std::string &nativeString) {
    jbyteArray arr = env->NewByteArray(nativeString.length());
    env->SetByteArrayRegion(arr, 0, nativeString.length(), (jbyte *) nativeString.c_str());
    return arr;
}

void SPReadWorker::doWork(std::string &msg) {
    int getEnvStat = g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        //如果没有， 主动附加到jvm环境中，获取到env
        if (g_vm->AttachCurrentThread(&env, nullptr) != 0) {
            std::__throw_runtime_error("获取java虚拟机实例失败!");
        }
    }
    //通过强转后的jcallback 获取到要回调的类
    jclass javaClass = env->GetObjectClass(*jcallback);

    if (javaClass == 0) {
        std::__throw_runtime_error("获取java回调类失败!");
    }

    //获取要回调的方法ID
    jmethodID javaCallbackId = env->GetMethodID(javaClass,
                                                 "onDataReceived", "([B)V");
    if (javaCallbackId == nullptr) {
        std::__throw_runtime_error("获取java回调方法失败!");
    }
    std::string data;

    while (!isInterrupted()) {
        _serialPort.Read(data);
        //执行回调
        env->CallVoidMethod(*jcallback, javaCallbackId, StringToJByteArray(env, data));
    }
}

SPReadWorker::~SPReadWorker() {
    g_vm->DetachCurrentThread();
    g_vm = nullptr;
    //释放你的全局引用的接口，生命周期自己把控
    env->DeleteGlobalRef(*jcallback);
    jcallback = nullptr;
    env = nullptr;
}

SPReadWorker::SPReadWorker(const char *c_name, const int *baudrate, JavaVM *vm, jobject *cal) :
        g_vm(vm),
        jcallback(cal),
        env(nullptr) {
    _serialPort = SerialPort(c_name, *baudrate);
    _serialPort.Open();
}
