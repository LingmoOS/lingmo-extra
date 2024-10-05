#include <libkylog.h>
#include <assert.h>

int main(int argc , char** argv)
{
	assert(kdk_logger_init("./logtest.conf") == 0);
    kdk_logger_set_autowrap(1);
	klog_calltrace();
	klog_debug("Debug");
	klog_info("Info");
	klog_notice("Notice");
	klog_warning("Warning");
	klog_err("Error");
	klog_crit("Crit");
	klog_alert("Alert");
	klog_emerg("Emerg");

	return 0;
}
