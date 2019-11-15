package com.castle.serialport

object SerialPortManager {
    init {
        System.loadLibrary("mserialport")
    }

    /**
     * 打开一个新串口
     * @param path 串口路径,通常为/dev/tty*开头
     * @param baudrate 波特率 拨特率越高, 发送消息越快
     * @param 如果成功,那么返回0,否则为失败
     */
    external fun openSerialPort(path: String, baudrate: Int)

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
     * @return 如果失败,底层会直接抛出异常
     */
    external fun sendMessage(path: String, msg: String)

    /**
     * 从指定串口读一次消息,阻塞
     * @param path 串口路径,通常为/dev/tty*开头
     */
    external fun testRead(path: String, baudrate: Int, listener: OnReadListener)

    interface OnReadListener {
        fun onDataReceived(msg: String)
    }
}