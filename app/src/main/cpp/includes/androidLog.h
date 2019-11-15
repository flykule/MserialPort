//
// Created by Administrator on 2019/11/15.
//

#ifndef MSERIALPORT_ANDROIDLOG_H
#define MSERIALPORT_ANDROIDLOG_H

#include <android/log.h>
static const char *TAG = "castle_serial_port";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#endif //MSERIALPORT_ANDROIDLOG_H
