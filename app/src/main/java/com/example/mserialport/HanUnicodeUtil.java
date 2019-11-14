package com.example.mserialport;

public class HanUnicodeUtil {
    /**
     * 将汉字转换车16进制字符串
     * @param str
     * @return st
     */
    public static String enUnicode(String str) {// 将汉字转换为16进制数
    String st = "";
    try {
        //这里要非常的注意,在将字符串转换成字节数组的时候一定要明确是什么格式的,这里使用的是gb2312格式的,还有utf-8,ISO-8859-1等格式
        byte[] by = str.getBytes("gbk");
        for (int i = 0; i < by.length; i++) {
            String strs = Integer.toHexString(by[i]);
            if (strs.length() > 2){
                strs = strs.substring(strs.length() - 2);
            }
            st += strs;
        }
    } catch (Exception e) {
        e.printStackTrace();
    }
    return st;
    }
}
