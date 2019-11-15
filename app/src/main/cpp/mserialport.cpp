#include <jni.h>
#include <android/log.h>
#include <SerialPortManager.h>

using namespace mn::CppLinuxSerial;

static const char *TAG = "serial_port";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

const int BIT16 = 16;
const int BIT8 = 8;
const SerialPortManager mManager;

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

extern "C" JNIEXPORT void JNICALL
Java_com_castle_serialport_SerialPortManager_openSerialPort(
        JNIEnv *env,
        jobject thiz,
        jstring path,
        jint baudRate
) {
    const char *path_utf = env->GetStringUTFChars(path, nullptr);
    mManager.addSerialPort(std::string(path_utf),);
    env->ReleaseStringUTFChars(path, path_utf);

}

