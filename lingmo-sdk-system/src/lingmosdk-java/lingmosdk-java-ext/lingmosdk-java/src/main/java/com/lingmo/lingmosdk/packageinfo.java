package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;
import java.util.List;

public interface packageinfo extends DBusInterface {
    public List<String> getPackageList();

    public String getPackageVersion(String name);

    public int getPackageInstalled(String name,String version);
}
