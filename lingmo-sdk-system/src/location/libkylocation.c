#include <gio/gio.h>

#define BUS_NAME "com.lingmosdk.location"
#define BUS_PATH "/com/lingmosdk/location"
#define BUS_FACE "com.lingmosdk.location"
#define METHOD "getAddress"

static GDBusConnection *conn;
static GMainLoop *loop;
static void SearchDeviceCallback(GDBusConnection *connection, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;
    GVariant **ret = (GVariant **)user_data;

    *ret = g_dbus_connection_call_finish (connection, res, &error);
    g_main_loop_quit(loop);
}

char *kdk_loaction_get()
{
	// return pwd->pw_name;
	GError *error = NULL;
    GVariant *result;
	char *user_name;

    conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);


    loop = g_main_loop_new(NULL, FALSE);
    /* 方法调用 */
    g_dbus_connection_call(conn,
                           BUS_NAME,
                           BUS_PATH,
                           BUS_FACE,
                           METHOD,
                           NULL,
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           (GAsyncReadyCallback)SearchDeviceCallback,
                           &result);
    g_main_loop_run(loop);
	if (!result){
		g_variant_unref(result);
		return "";
	}
	g_variant_get(result, "(s)", &user_name);
    g_variant_unref(result);
	return user_name;

}
