#ifndef LIBKYBIOSINFO_H
#define LIBKYBIOSINFO_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern const char *kdk_bios_get_vendor();

    extern const char *kdk_bios_get_version();

    extern void kdk_bios_free(char *info);

#ifdef __cplusplus
}
#endif

#endif