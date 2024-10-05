package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;
import java.util.List;

public interface bios extends DBusInterface {
    public String getBiosVendor();
    public String getBiosVersion();
}
