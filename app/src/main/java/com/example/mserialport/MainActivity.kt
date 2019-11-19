package com.example.mserialport

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.castle.serialport.SerialPortManager
import kotlinx.android.synthetic.main.activity_main.*
import java.util.*
import kotlin.concurrent.fixedRateTimer

class MainActivity : AppCompatActivity() {

    fun pageJump(jumper: PageJump): String {
        return "5aa5078200845a0100${jumper.pageIndex}"
    }

    private val mScreenPath = "/dev/ttysWK0"
    val SERIAL_PORT_NAME_KEYBROAD = "/dev/ttysWK2"//键盘对应的串口名
    val SERIAL_PORT_KEYBROAD = 9600//键盘串口的波特率
    val SERIAL_PORT_NAME_QRCODE_SCAN = "/dev/ttysWK3"//扫码头串口名
    val SERIAL_PORT_QRCODE_SCAN = 9600//二维码扫码器波特率

    val SEARIAL_PORT_NAME_SCREEN_2 = "/dev/ttyHSL0"//屏幕2串口
    val SERIAL_PORT_SCREEN_2 = 9600//屏幕串口的波特率
    val SERIAL_PORT_NAME_CARDCODE_SCAN = "/dev/ttysWK1"//读卡器串口名
    val SERIAL_PORT_CARDCODE_SCAN = 57600//读卡器波特率

    val mSerialPortManager by lazy {
        SerialPortManager.apply {
            openWriteSerialPort(
                mScreenPath,
                9600
            )
        }
    }

    val PAGE_STATUS_INITING = "01" //初始化界面
    val PAGE_STATUS_STANDBY = "04" //待机界面

    private fun nowDate(time: Long): String {
        //获取默认选中的日期的年月日星期的值，并赋值
        val calendar = Calendar.getInstance()//日历对象
        calendar.time = Date(time)//设置当前日期
        val year = HexUtils.intToHex8(
            calendar.get(Calendar.YEAR).toString().substring(
                2,
                4
            ).toInt()
        ) //年份后两位
        val month = HexUtils.intToHex8((calendar.get(Calendar.MONTH) + 1).toString().toInt()) //设置月份
        val day = HexUtils.intToHex8(calendar.get(Calendar.DAY_OF_MONTH).toString().toInt()) //设置日
        val hour = HexUtils.intToHex8(calendar.get(Calendar.HOUR_OF_DAY).toString().toInt()) //设置小时
        val minute = HexUtils.intToHex8(calendar.get(Calendar.MINUTE).toString().toInt()) //设置分钟
        val sec = HexUtils.intToHex8(calendar.get(Calendar.SECOND).toString().toInt()) //设置分钟
        return "5AA50A820010" + year + month + day + "01" + hour + minute + sec
    }

    val dateCommand: String get() = nowDate(System.currentTimeMillis())

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initListener()
        initListener_2()
        // Example of a call to a native method
    }

    private fun initListener_2() {
        read_qr_scanner.setOnClickListener {
            mSerialPortManager.openReadSerialPort(
                SERIAL_PORT_NAME_QRCODE_SCAN,
                SERIAL_PORT_QRCODE_SCAN,
                object : SerialPortManager.OnReadListener {
                    override fun onDataReceived(msg: ByteArray) {
                        println("接受到扫码头消息${HexUtils.bytesToHexString(msg)}")
                    }
                })
        }
        start_listen_screen_2.setOnClickListener {
            mSerialPortManager.openWriteSerialPort(
                SEARIAL_PORT_NAME_SCREEN_2,
                SERIAL_PORT_KEYBROAD
            )
        }
        crazy_test.setOnClickListener {
            fixedRateTimer("Trump", true, 5 * 1000, 5) {
                mSerialPortManager.sendMessage(
                    mScreenPath,
                    dateCommand,
                    SerialPortManager.FLAG_WRITE
                )
                val millis = System.currentTimeMillis()
                val version = Random(millis).nextInt(2000)
                mSerialPortManager.sendMessage(
                    mScreenPath,
                    pageCmd("2900", "${version}${version}"),
                    2
                );
            }
        }
        end_listen_screen_2.setOnClickListener {
            mSerialPortManager.closeSerialPort(SEARIAL_PORT_NAME_SCREEN_2)
        }
        close_all_sp.setOnClickListener {
            mSerialPortManager.closeSerialPort(mScreenPath)
            mSerialPortManager.closeSerialPort(SERIAL_PORT_NAME_KEYBROAD)
            mSerialPortManager.closeSerialPort(SERIAL_PORT_NAME_QRCODE_SCAN)
            mSerialPortManager.closeSerialPort(SEARIAL_PORT_NAME_SCREEN_2)
        }
    }

    private fun initListener() {
        update_time.setOnClickListener {
            mSerialPortManager.sendMessage(mScreenPath, dateCommand, SerialPortManager.FLAG_WRITE)
        }

        update_version.setOnClickListener {
            val millis = System.currentTimeMillis()
            val version = Random(millis).nextInt(2000)
            mSerialPortManager.sendMessage(mScreenPath, pageCmd("2900", "${version}${version}"), 2);
        }
        start_listen_kb.setOnClickListener {
            mSerialPortManager.openReadSerialPort(
                SERIAL_PORT_NAME_KEYBROAD,
                9600,
                object : SerialPortManager.OnReadListener {
                    override fun onDataReceived(msg: ByteArray) {
                        println("接受到键盘消息${HexUtils.bytesToHexString(msg)}")
                    }
                });
        }
        end_listen_kb.setOnClickListener {
            mSerialPortManager.closeSerialPort(SERIAL_PORT_NAME_KEYBROAD);
        }
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    fun enUnicode(str: String): String {// 将汉字转换为16进制数

        var st = ""
        try {
            //这里要非常的注意,在将字符串转换成字节数组的时候一定要明确是什么格式的,这里使用的是gb2312格式的,还有utf-8,ISO-8859-1等格式
            val by = str.toByteArray(charset("gbk"))
            for (i in by.indices) {
                var strs = Integer.toHexString(by[i].toInt())
                if (strs.length > 2) {
                    strs = strs.substring(strs.length - 2)
                }
                st += strs
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }

        return st
    }

    fun pageMenu(menuPage: MenuPage): String {
        val contentGBK = menuPage.content.let { enUnicode(it) }
        val lenth = contentGBK.length / 2 + 3
        return "5aa5" + HexUtils.intToHex8(lenth) + "82" + menuPage.position.substring(
            0,
            2
        ) + menuPage.position.substring(2, 4) + contentGBK

    }

    private fun pageCmd(position: String, content: String): String {
        val contentGBK = HanUnicodeUtil.enUnicode(content)
        val lenth = contentGBK.length / 2 + 3
        return "5aa5" + HexUtils.intToHex8(lenth) + "82" + position.substring(
            0,
            2
        ) + position.substring(2, 4) + contentGBK
    }
}
