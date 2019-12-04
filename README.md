# MserialPort
旧的androidSerialPort已经过于陈旧, 不支持奇偶校验,停止位,超时等设置, 其次当用于多线程应用时表现不佳,
总体读写较单片机应用有较大差距, 经常会出现莫名其妙的延迟,因此写了一个新的串口读写库.在底层直接进行读写.
## 特点
* 支持调整时间间隔, 灵活调整最大一次性读取数据与串口响应时间
* 底层直接进行读写, 不再返回fileDescriptor指针
* 底层直接管理串口, 添加删除等
* 目前的线程通讯通过std::future进行, 未来考虑支持socket通信
## 如何使用
1. 使用AndroidStuido->Build Apk, 生成debug包
2. 修改生成的apk后缀为zip
3. 解压,复制其中的lib文件夹到你的应用
4. 复制SerialPortManager.kt这个文件,保持一样的包结构到你的src/main/java目录下
5. 参考dmeo进行使用
## 效果展示
![demo](https://github.com/flykule/MserialPort/blob/master/gif/demo.gif)

