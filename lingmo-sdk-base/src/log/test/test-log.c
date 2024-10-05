#include <libkylog.h>
#include <assert.h>

int main(int argc , char** argv)
{
	assert(kdk_logger_init("./logtest.conf") == 0);

	klog_calltrace();
	klog_debug("Debug\n");
	klog_info("Info\n");
	klog_notice("Notice\n");
	klog_warning("Warning\n");
	klog_err("Error\n");
	klog_crit("Crit\n");
	klog_alert("Alert\n");
	klog_emerg("Emerg\n");

	return 0;
}
