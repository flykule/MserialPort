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
    work_thread = new std::thread(&SPReadWorker::readLoop, this);
}

SPReadWorker::~SPReadWorker() {
    LOGD("开始销毁SPReadWorker");
    _serialPort->Close();
    _serialPort = nullptr;
    if (g_vm)
        g_vm->DetachCurrentThread();
    g_vm = nullptr;
//    释放你的全局引用的接口，生命周期自己把控
    if (jcallback)
        env->DeleteGlobalRef(*jcallback);
    jcallback = nullptr;
    env = nullptr;
    if (work_thread->joinable()) {
        work_thread->join();
    }
    work_thread = nullptr;
};

SPReadWorker::SPReadWorker(const char *c_name, const int *baudrate, JavaVM *vm,
                           jobject *callback) :
        jcallback(callback),
        work_thread(nullptr),
        g_vm(vm),
        env(nullptr) {
    _serialPort = new SerialPort(c_name, *baudrate);
    //non-blocking read
    _serialPort->SetTimeout(0);
    _serialPort->Open();
}

void SPReadWorker::readLoop() {
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
        _serialPort->Read(data);
        if (!data.empty()) {
            //执行回调
            if (*jcallback == nullptr) {
                LOGD("因为获取不到回调而停止");
                stop();
            }
            env->CallVoidMethod(*jcallback, javaCallbackId, StringToJByteArray(env, data));
        }
    }
    LOGD("读线程终止运行");
}
