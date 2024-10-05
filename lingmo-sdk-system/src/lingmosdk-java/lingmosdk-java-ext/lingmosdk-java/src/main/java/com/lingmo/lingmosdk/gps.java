package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;

public interface gps extends DBusInterface {
    public String getGPSInfo();
}
