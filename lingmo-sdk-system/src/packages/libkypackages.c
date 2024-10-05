#include "libkypackages.h"
#include <libkylog.h>
#include <cstring-extension.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __linux__
#define DPKG_DB     "/var/lib/dpkg/status"
#endif

#define PACKAGE_INCREASE_STEP   50

static inline char* lookup(char *buffer, const char *key)
{
    if (strstartswith(buffer, key) == 0)
    {
        buffer += strlen(key) + 1;
        strstripspace(buffer);
        return buffer;
    }
    return NULL;
}

kdk_package_list* kdk_package_get_packagelist()
{
    FILE *fp = fopen(DPKG_DB, "rt");
    if (fp <= 0)
        return NULL;
    
    kdk_package_list *packagelist = (kdk_package_list*)calloc(1, sizeof(packagelist));
    if (!packagelist)
        goto err_out;
    
    char buffer[4097];
    kdk_package_t *curpackage = NULL;
    size_t maxpackage = 0;
    do
    {
        if (! fgets(buffer, 4096, fp))
            continue;
        char *val;
        if ((val = lookup(buffer, "Package")))
        {
            curpackage = (kdk_package_t *)calloc(1, sizeof(kdk_package_t));
            if (!curpackage)
                goto err_out;
            curpackage->name = strdup(val);
            if (packagelist->nums == maxpackage)
            {
                maxpackage += PACKAGE_INCREASE_STEP;
                kdk_package_t **tmp = (kdk_package_t **)realloc(packagelist->list, maxpackage * sizeof(kdk_package_t *));
                if (!tmp)
                {
                    free(curpackage);
                    goto err_out;
                }
                packagelist->list = tmp;
            }
            packagelist->list[packagelist->nums] = curpackage;
            packagelist->nums ++;
        }
        else if ((val = lookup(buffer, "Status")))
        {
            if (strcmp(val, "install ok installed") == 0)
            {
                curpackage->status = strdup("ii");
            }
            else if (strcmp(val, "deinstall ok config-files") == 0)
            {
                curpackage->status = strdup("rc");
            }
        }
        else if ((val = lookup(buffer, "Section")))
        {
            curpackage->section = strdup(val);
        }
        else if ((val = lookup(buffer, "Version")))
        {
            curpackage->version = strdup(val);
        }
        else if ((val = lookup(buffer, "Installed-Size")))
        {
            curpackage->size_kb = atol(val);
        }
        else if ((val = lookup(buffer, "Depends")))
        {

        }
    }while (feof(fp) == 0);

    fclose(fp);
    return packagelist;

err_out:
#ifdef __linux__
    fclose(fp);
#endif
    kdk_package_free_packagelist(packagelist);
    return NULL;
}

static void _free_package_t(kdk_package_t *package)
{
    if (package->name)
        free(package->name);
    if (package->section)
        free(package->section);
    if (package->version)
        free(package->version);
    if (package->status)
        free(package->status);
    if (package->depends)
        free(package->depends);
}

void kdk_package_free_packagelist(kdk_package_list *pl)
{
    if (!pl)
        return;
    
    if (pl->list)
    {
        for (int i = 0; i < pl->nums; i ++)
        {
            _free_package_t(pl->list[i]);
            free(pl->list[i]);
        }
        free(pl->list);
    }
    free(pl);
}

int kdk_package_install_package(const char *packagePath)
{

}

int kdk_package_remove_package(const char *name, const char *version)
{

}

char* kdk_package_get_version(const char *name)
{
    FILE *fp = fopen(DPKG_DB, "rt");
    if (fp <= 0)
        return NULL;
    
    char buffer[4097];
    char *val = NULL;
    int flag = 0;
    while (fgets(buffer, 4096, fp))
    {
        if ((val = lookup(buffer, "Package")))
        {
            if (strcmp(val, name) == 0)
                flag = 1;
        }
        else if (flag && (val = lookup(buffer, "Version")))
            break;
    }
    fclose(fp);
    return val ? strdup(val) : NULL;
}

int kdk_package_is_installed(const char *name, const char *version)
{
    FILE *fp = fopen(DPKG_DB, "rt");
    if (!fp)
        return -1;
    
    char buffer[4097];
    char *val;
    int flag = 0;
    while (fgets(buffer, 4096, fp))
    {
        if ((val = lookup(buffer, "Package")))
        {
            if (strcmp(val, name) == 0)
            {
                flag = 1;
                break;
            }
        }
    }
    fclose(fp);
    return flag;
}
