package com.lingmo.lingmosdk;

import org.freedesktop.dbus.interfaces.DBusInterface;
import java.util.List;

public interface print extends DBusInterface {
    public List<String> getPrinterList();

    public List<String> getPrinterAvailableList();

    public void setPrinterOptions(int page,String paperType,String cups,String sided);

    public int getPrinterPrintLocalFile(String printer,String file);

    public int getPrinterCancelAllJobs(String printer);

    public int getPrinterStatus(String printer);

    public String getPrinterFilename(String url);

    public int getPrinterJobStatus(String job, int id);

    public int getPrinterDownloadRemoteFile(String remoteFile,String localFile);

}
