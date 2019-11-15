# MserialPort
旧的androidSerialPort已经过于陈旧, 不支持奇偶校验,停止位,超时等设置, 其次当用于多线程应用时表现不佳,
总体读写较单片机应用有较大差距, 经常会出现莫名其妙的延迟,因此写了一个新的串口读写库.在底层直接进行读写.
## 特点
* 底层直接进行读写, 不再返回fileDescriptor指针
* 底层直接管理串口, 添加删除等
* 目前的线程通讯通过std::future进行, 未来考虑支持socket通信
## 效果展示
![demo](https://github.com/flykule/MserialPort/blob/master/gif/demo.mp4)

