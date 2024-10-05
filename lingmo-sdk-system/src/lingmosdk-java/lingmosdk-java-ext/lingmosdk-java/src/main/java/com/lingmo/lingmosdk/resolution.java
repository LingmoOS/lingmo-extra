package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;
import java.util.List;

public interface resolution extends DBusInterface{
    public List<String> getSysLegalResolution();
}
