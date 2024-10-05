package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.types.UInt32;

public interface cpuinfo extends DBusInterface {
    public String getCpuArch();

    public String getCpuVendor();

    public String getCpuModel();

    public String getCpuFreqMHz();

    public UInt32 getCpuCorenums();

    public String getCpuVirt();

    public UInt32 getCpuProcess();
}
