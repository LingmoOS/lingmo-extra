package com.lingmo.lingmosdk;

import java.util.List;
import org.freedesktop.dbus.interfaces.DBusInterface;

public interface netcard extends DBusInterface {

    public List<String> getNetCardName();
    public int getNetCardUp(String nc);
    public List<String> getNetCardUpcards();
    public String getNetCardPhymac(String nc);
    public String getNetCardPrivateIPv4(String nc);
    public List<String> getNetCardIPv4(String nc);
    public String getNetCardPrivateIPv6(String nc);
    public  int getNetCardType(String nc);
    public  List<String> getNetCardProduct(String nc);
    public List<String> getNetCardIPv6(String nc);
}
