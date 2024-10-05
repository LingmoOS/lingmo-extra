package lingmo.lingmosdk.java.client;

import org.freedesktop.dbus.connections.impl.DBusConnection;
import org.freedesktop.dbus.exceptions.DBusException;

/***
 * dbus连接器
 * @author lihao3
 */
public class DBusClient  {
    public static DBusConnection getConn(DBusConnection.DBusBusType dBusBusType) throws DBusException {
        DBusConnection  conn = DBusConnection.getConnection(dBusBusType);
        return  conn;
    }
}
