package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.types.UInt32;
import org.freedesktop.dbus.types.UInt64;
import java.util.List;

public interface disk extends DBusInterface {
    public List<String> getDiskList();
    public UInt32 getDiskSectorSize(String name);
    public float getDiskTotalSizeMiB(String name);
    public String getDiskModel(String name);
    public String getDiskSerial(String name);
    public UInt32 getDiskPartitionNums(String name);
    public String getDiskType(String name);
    public String getDiskVersion(String name);
    public UInt64 getDiskSectorNum(String name);

}
