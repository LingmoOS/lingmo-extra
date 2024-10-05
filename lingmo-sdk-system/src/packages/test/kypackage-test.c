#include "../libkypackages.h"
#include <stdio.h>

static void print_package(kdk_package_t *package)
{
    printf("包名：%s\t", package->name);
    printf("版本号：%s\t", package->version);
    printf("包类型：%s\t", package->section);
    printf("包状态：%s\t", package->status);
    printf("包大小：%lu\n", package->size_kb);
}

int main()
{
    kdk_package_list *list = kdk_package_get_packagelist();
    printf("系统中一共%u个软件包\n", list->nums);
    for (size_t i = 0; i < list->nums; i ++)
    {
        print_package(list->list[i]);
    }
    kdk_package_free_packagelist(list);

    printf("Evolution是否安装：%s\n", kdk_package_is_installed("evolution", NULL) == 1 ? "是" : "否");
    char *version = kdk_package_get_version("evolution");
    printf("Evolution版本号：%s\n", version);
    free(version);

    return 0;
}
