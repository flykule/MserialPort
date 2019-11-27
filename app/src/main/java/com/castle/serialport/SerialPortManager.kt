package com.castle.serialport

import android.util.Log


object SerialPortManager {
    //写flag
    const val FLAG_WRITE = 1;
    //读flag
    const val FLAG_READ = 2;

    init {
        Log.d("SerialPortManager", "开始加载库")
        System.loadLibrary("mserialport")
    }


    /**
     * 关闭串口
     * @param path 串口路径,通常为/dev/tty*开头
     * @param 如果成功,那么返回0,否则为失败
     */
    external fun closeSerialPort(path: String)

    /**
     * 发送消息给指定串口, 底层已经为串口读写专门开启线程,上层可以直接调用,无需切换线程
     * @param path 串口路径,通常为/dev/tty*开头
     * @param msg 要发送给串口的消息, 直接传入hexString即可, 底层会将其转换成为16进制byte数组
     * @param flags 标记, 1->只写,2->只读,3->读写
     * @return 如果失败,底层会直接抛出异常
     */
    external fun sendBytes(path: String, msg: Array<String>, flags: Int = FLAG_WRITE)

    /**
     * 发送消息给指定串口, 底层已经为串口读写专门开启线程,上层可以直接调用,无需切换线程
     * @param path 串口路径,通常为/dev/tty*开头
     * @param msg 要发送给串口的消息, 传入byte数组即可
     * @param flags 标记, 1->只写,2->只读,3->读写
     * @return 如果失败,底层会直接抛出异常
     */
    external fun sendBytes(path: String, msg: Array<ByteArray>, flags: Int = FLAG_WRITE)

    /**
     * 打开一个读串口,用于监听数据
     * @param path 串口路径,通常为/dev/tty*开头
     * @param baudrate 串口拨特率,底层在打开串口时会检测一次默认波特率
     * @param listener 读数据监听,为空的话就为只写接口
     */
    external fun openSerialPort(path: String, baudrate: Int, listener: OnReadListener? = null)

    interface OnReadListener {
        fun onDataReceived(msg: ByteArray)
    }
}