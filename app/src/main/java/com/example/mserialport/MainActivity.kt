package com.example.mserialport

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
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
    val SERIAL_PORT_QRCODE_SCAN = 2400//二维码扫码器波特率

    val SEARIAL_PORT_NAME_SCREEN_2 = "/dev/ttyHSL0"//屏幕2串口
    val SERIAL_PORT_SCREEN_2 = 9600//屏幕串口的波特率
    val SERIAL_PORT_NAME_CARDCODE_SCAN = "/dev/ttysWK1"//读卡器串口名
    val SERIAL_PORT_CARDCODE_SCAN = 57600//读卡器波特率

//    val mSerialPortManager by lazy {
//        SerialPortManager.apply {
//            openWriteSerialPort(
//                mScreenPath,
//                9600
//            )
//        }
//    }

    val PAGE_STATUS_INITING = "01" //初始化界面
    val PAGE_STATUS_STANDBY = "04" //待机界面
    var isOffline = false;

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
    private var mTestTimer: Timer? = null;

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initListener()
        initListener_2()
        checkPermissions()
        // Example of a call to a native method
    }

    private fun initListener_2() {
        read_qr_scanner.setOnClickListener {
            SerialPortManager.openSerialPort(
                SERIAL_PORT_NAME_QRCODE_SCAN,
                9600,
                object : SerialPortManager.OnReadListener {
                    override fun onDataReceived(msg: ByteArray) {
                        println("接受到扫码头消息${String(msg)}")
                    }
                })
            SerialPortManager.setReadTimeInterval(SERIAL_PORT_NAME_QRCODE_SCAN, 500000)
        }
        start_listen_screen_2.setOnClickListener {
            SerialPortManager.openSerialPort(
                SEARIAL_PORT_NAME_SCREEN_2,
                SERIAL_PORT_KEYBROAD
            )
        }
        crazy_test.setOnClickListener {
            SerialPortManager.openSerialPort(mScreenPath, SERIAL_PORT_SCREEN_2)
//            SerialPortManager.sendMessage(mScreenPath, arrayOf())
            mTestTimer = fixedRateTimer("Trump", true, 5 * 1000, 5) {
                val millis = System.currentTimeMillis()
                val version = Random(millis).nextInt(2000)
                val pageCmd = pageCmd("2900", "${version}${version}")
                SerialPortManager.sendMessage(
                    mScreenPath,
                    arrayOf(dateCommand, pageCmd),
                    SerialPortManager.FLAG_WRITE
                )
                isOffline = !isOffline;
                if (isOffline) {
                    SerialPortManager.sendBytes(mScreenPath, arrayOf(offlineCmd))
                } else {
                    SerialPortManager.sendBytes(mScreenPath, arrayOf(onlineCmd))
                }
            }
        }
        end_listen_screen_2.setOnClickListener {
            SerialPortManager.closeSerialPort(SEARIAL_PORT_NAME_SCREEN_2)
        }
        close_all_sp.setOnClickListener {
            mTestTimer?.cancel()
            SerialPortManager.closeSerialPort(mScreenPath)
            SerialPortManager.closeSerialPort(SERIAL_PORT_NAME_KEYBROAD)
            SerialPortManager.closeSerialPort(SERIAL_PORT_NAME_QRCODE_SCAN)
            SerialPortManager.closeSerialPort(SEARIAL_PORT_NAME_SCREEN_2)
        }
    }

    private fun initListener() {
        update_time.setOnClickListener {
            SerialPortManager.openSerialPort(mScreenPath, SERIAL_PORT_SCREEN_2)
            SerialPortManager.sendMessage(
                mScreenPath,
                arrayOf(pageJump(PageJump("04")), dateCommand),
                SerialPortManager.FLAG_WRITE
            )
        }

        test_bytes.setOnClickListener {
            SerialPortManager.openSerialPort(mScreenPath, SERIAL_PORT_SCREEN_2)
            isOffline = !isOffline;
            if (isOffline) {
                SerialPortManager.sendBytes(mScreenPath, arrayOf(offlineCmd))
            } else {
                SerialPortManager.sendBytes(mScreenPath, arrayOf(onlineCmd))
            }
        }

        update_version.setOnClickListener {
            SerialPortManager.openSerialPort(mScreenPath, SERIAL_PORT_SCREEN_2)
            val millis = System.currentTimeMillis()
            val version = Random(millis).nextInt(2000)
            val bytes = pageCmd("2900", "${version}${version}")
            SerialPortManager.sendMessage(mScreenPath, arrayOf(bytes))
        }
        start_listen_kb.setOnClickListener {
            SerialPortManager.openSerialPort(
                SERIAL_PORT_NAME_KEYBROAD,
                SERIAL_PORT_KEYBROAD,
                object : SerialPortManager.OnReadListener {
                    override fun onDataReceived(msg: ByteArray) {
                        println("接受到键盘消息${HexUtils.bytesToHexString(msg)}")
                    }
                });
            SerialPortManager.sendMessage(SERIAL_PORT_NAME_KEYBROAD, arrayOf("0000"))
            SerialPortManager.setReadTimeInterval(SERIAL_PORT_NAME_KEYBROAD, 200)
        }
        end_listen_kb.setOnClickListener {
            SerialPortManager.closeSerialPort(SERIAL_PORT_NAME_KEYBROAD);
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

    var permissions = arrayOf<String>(
        Manifest.permission.INTERNET,
        Manifest.permission.READ_PHONE_STATE,
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.VIBRATE,
        Manifest.permission.RECORD_AUDIO
    )

    private fun checkPermissions(): Boolean {
        var result: Int
        val listPermissionsNeeded: MutableList<String> = ArrayList()
        for (p in permissions) {
            result = ContextCompat.checkSelfPermission(this, p)
            if (result != PackageManager.PERMISSION_GRANTED) {
                listPermissionsNeeded.add(p)
            }
        }
        if (!listPermissionsNeeded.isEmpty()) {
            ActivityCompat.requestPermissions(
                this,
                listPermissionsNeeded.toTypedArray(),
                100
            )
            return false
        }
        return true
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        if (requestCode == 100) {
            if (grantResults.size > 0
                && grantResults[0] == PackageManager.PERMISSION_GRANTED
            ) { // do something
            }
            return
        }
    }

    /**
     * 显示离线
     */
    var offlineCmd = byteArrayOf(
        0x5A,
        0xA5.toByte(),
        0x45,
        0x82.toByte(),
        0x10,
        0x00,
        0x00,
        0x04,
        0x00,
        0x07,
        0x00,
        0xE8.toByte(),
        0x00,
        0x12,
        0x00,
        0xEC.toByte(),
        0x00,
        0x15,
        0xF8.toByte(),
        0x00,
        0x00,
        0xEE.toByte(),
        0x00,
        0x0f,
        0x00,
        0xF2.toByte(),
        0x00,
        0x15,
        0xF8.toByte(),
        0x00,
        0x00,
        0xF4.toByte(),
        0x00,
        0x0c,
        0x00,
        0xF8.toByte(),
        0x00,
        0x15,
        0xF8.toByte(),
        0x00,
        0x00,
        0xFA.toByte(),
        0x00,
        0x09,
        0x00,
        0xFE.toByte(),
        0x00,
        0x15,
        0xF8.toByte(),
        0x00,
        0x01,
        0x00,
        0x00,
        0x06,
        0x01,
        0x04,
        0x00,
        0x15,
        0xF8.toByte(),
        0x00,
        0x01,
        0x06,
        0x00,
        0x03,
        0x01,
        0x0a,
        0x00,
        0x15,
        0xF8.toByte(),
        0x00,
        0xFF.toByte(),
        0x00
    )

    /**
     * 显示满格信号
     */
    var onlineCmd = byteArrayOf(
        0x5A,
        0xA5.toByte(),
        0x45,
        0x82.toByte(),
        0x10,
        0x00,
        0x00,
        0x04,
        0x00,
        0x07,
        0x00,
        0xe8.toByte(),
        0x00,
        0x12,
        0x00,
        0xec.toByte(),
        0x00,
        0x15,
        0xff.toByte(),
        0xff.toByte(),
        0x00,
        0xee.toByte(),
        0x00,
        0x0f,
        0x00,
        0xf2.toByte(),
        0x00,
        0x15,
        0xff.toByte(),
        0xff.toByte(),
        0x00,
        0xf4.toByte(),
        0x00,
        0x0c,
        0x00,
        0xf8.toByte(),
        0x00,
        0x15,
        0xff.toByte(),
        0xff.toByte(),
        0x00,
        0xfa.toByte(),
        0x00,
        0x09,
        0x00,
        0xfe.toByte(),
        0x00,
        0x15,
        0xff.toByte(),
        0xff.toByte(),
        0x01,
        0x00,
        0x00,
        0x06,
        0x01,
        0x04,
        0x00,
        0x15,
        0xff.toByte(),
        0xff.toByte(),
        0x01,
        0x06,
        0x00,
        0x03,
        0x01,
        0x0a,
        0x00,
        0x15,
        0xff.toByte(),
        0xff.toByte(),
        0xff.toByte(),
        0x00
    )

}
