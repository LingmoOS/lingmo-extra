package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;

public interface mainboard extends DBusInterface{

    //输出：主板型号
    public String getMainboardName();
    //输出：发布日期
    public String getMainboardDate();
    //输出：主板序列号
    public String getMainboardSerial();
    //输出：主板厂商
    public String getMainboardVendor();

}
