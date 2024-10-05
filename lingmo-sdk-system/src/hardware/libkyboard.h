#ifndef LIBKYBOARDINFO_H
#define LIBKYBOARDINFO_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern const char *kdk_board_get_name();

    extern const char *kdk_board_get_date();

    extern const char *kdk_board_get_serial();

    extern const char *kdk_board_get_vendor();

    extern void kdk_board_free(char *info);

#ifdef __cplusplus
}
#endif

#endif