package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;

public interface resource extends DBusInterface {
    public double getMemTotalKiB();

    public double getMemUsagePercent();

    public double getMemUsageKiB();

    public double getMemAvailableKiB();

    public double getMemFreeKiB();

    public double getMemVirtAllocKiB();

    public double getMemSwapTotalKiB();

    public double getMemSwapUsagePercent();

    public double getMemSwapUsageKiB();

    public double getMemSwapFreeKiB();

    public double getCpuCurrentUsage();

    public String getUpTime();
}
