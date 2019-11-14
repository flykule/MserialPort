#include <jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <Exception.hpp>
#include <SerialPort.hpp>
#include <PFBackgroundService.h>

using namespace mn::CppLinuxSerial;

static const char *TAG = "serial_port";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

const int BIT16 = 16;
const int BIT8 = 8;

void HexToBytes(const std::string &hex, char *result) {
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = (char) strtol(byteString.c_str(), nullptr, BIT16);
        *result = byte;
        result++;
    }
}

void sendTestData(SerialPort &serialPort, std::string &data) {
    //将string转换为hexBytes
    char command[data.size() / 2];
    HexToBytes(data, command);
    serialPort.Write(command, data.size() / 2);
    serialPort.Close();

    if (serialPort.currendState() == State::CLOSED) {
        LOGD("关闭串口成功");
    }
}

std::unique_ptr<IBackgroundService> pfService = std::make_unique<PFBackgroundService>(
        [](std::string msg) {
            LOGD("Received message %s", msg.c_str());
        });

extern "C" JNIEXPORT void JNICALL
Java_com_example_mserialport_MainActivity_testSendData(
        JNIEnv *env,
        jobject thiz,
        jstring data) {
//    LOGD("开始开启串口并测试发送数据");
    SerialPort serialPort;
    serialPort = SerialPort("/dev/ttysWK0", BaudRate::B_9600);
    serialPort.Open();
    if (serialPort.currendState() != State::OPEN) {
        LOGE("Open serial port failed");
    } else {
        LOGD("Open serial port success!");
    }
    const char *path_utf = env->GetStringUTFChars(data, nullptr);
    std::string s = std::string(path_utf);
    LOGD("发送命令 %s", path_utf);
    sendTestData(serialPort, s);
    env->ReleaseStringUTFChars(data, path_utf);
    //make an instance of the promise/future implementation
    pfService->processMessage(s);
}

