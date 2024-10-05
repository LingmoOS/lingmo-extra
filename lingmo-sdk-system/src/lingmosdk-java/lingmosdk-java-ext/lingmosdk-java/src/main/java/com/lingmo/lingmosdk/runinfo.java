package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;

public interface runinfo extends DBusInterface {
    public double getNetSpeed(String nc);
    public int getDiskRate(String disk);
    public double getCpuTemperature();
    public double getDiskTemperature(String name);
}
