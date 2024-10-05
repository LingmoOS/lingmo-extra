package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;
import org.freedesktop.dbus.types.UInt32;

import java.util.List;

public interface sysinfo extends DBusInterface {

    public String getSystemArchitecture();

    public String getSystemName();

    public String getSystemVersion(boolean verbose);

    public int getSystemActivationStatus();

    public String getSystemSerialNumber();

    public String getSystemKernelVersion();

    public String getSystemEffectUser();

    public String getSystemProjectName();

    public String getSystemProjectSubName();

    public UInt32 getSystemProductFeatures();

    public String getSystemHostVirtType();

    public String getSystemHostCloudPlatform();

    public String getSystemOSVersion();

    public String getSystemUpdateVersion();

    public boolean getSystemIsZYJ();
}
