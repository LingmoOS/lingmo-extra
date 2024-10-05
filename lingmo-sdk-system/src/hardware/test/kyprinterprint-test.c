#include "../libkyprinter.h"
#include <stdio.h>

int main()
{
    int index = 0;
    //获取打印机列表
    char **printers = kdk_printer_get_available_list();
    while (printers[index])
    {
        printf("%zd: %s\n", index + 1, printers[index]);

        //取消当前打印机所有打印任务
        kdk_printer_cancel_all_jobs(printers[index]);

        //获取url对应的文件名
        char *url = "http://www.cztouch.com/upfiles/soft/testpdf.pdf";
        int res = -1;
        char *filename = kdk_printer_get_filename(url); //从完整路径名中解析出文件名称，例如：/home/test/abc.txt,解析完成后为abc.txt
        printf("filename = %s\n", filename);

        //设置打印参数
        kdk_printer_set_options(2, "A4", "lrtb", "two-sided-long-edge");

        //下载
        res = kdk_printer_print_download_remote_file(url, filename);
        printf("[%s] res = %d\n", __FUNCTION__, res);
        printf("[%s] filepath = %s\n", __FUNCTION__, filename);

        //打印
        int job_id = kdk_printer_print_local_file(printers[index], filename);
        if (job_id == 0)
        {   
            //打印失败
            printf("[%s] create print job fail\n", __FUNCTION__);
        }
        else
        {
            //打印成功，等待一会获取任务状态
            sleep(10);
            int status = kdk_printer_get_job_status(printers[index], job_id);
        }

        index++;
        free(filename);
    }
    kdk_printer_freeall(printers);
    return 0;
}
