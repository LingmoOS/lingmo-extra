package com.lingmo.lingmosdk;

import java.util.List;
import org.freedesktop.dbus.interfaces.DBusInterface;

public interface net extends DBusInterface {
    public int getPortState(int port);
    public List<String>  getMultiplePortStat(int begin,int end);
    public List<String> getGatewayInfo();
    public List<String> getFirewallState();
}
