#include <libkyconf.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char **keylist = NULL;
    int id = kdk_conf_init("struct.conf");
    ASSERT_NOT_NULL(id, -1);

    char **grouplist = kdk_conf_list_group(id);
    ASSERT_NOT_NULL(grouplist, -1);

    char *tmpgroup;
    int index = 0;
    while ((tmpgroup = grouplist[index]))
    {
        printf("Group: %s\n", tmpgroup);
        keylist = kdk_conf_list_key(id, tmpgroup);
        ASSERT_NOT_NULL(keylist, -1);
        char *tmpkey;
        int k_index = 0;
        while ((tmpkey = keylist[k_index]))
        {
            const char *tmpval = kdk_conf_get_value(id, tmpgroup, tmpkey);
            printf("%s = %s\n", tmpkey , tmpval);
            k_index ++;
        }
        kdk_config_freeall(keylist);

        index ++;
    }

    kdk_conf_destroy(id);
    kdk_config_freeall(grouplist);

    return 0;
}