#include <stdio.h>
#include <string.h>
#include <X11/extensions/Xrandr.h>
#include <dirent.h>
#include <errno.h>
#include <libkylog.h>
#include <fcntl.h>
#include <lingmosdk/lingmosdk-base/cstring-extension.h>
#include <lingmosdk/lingmosdk-base/sdkmarcos.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include "libkyedid.h"
#include "hd.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static int edid_lines = 0;

// static int claims_one_point_oh = 0;
static int claims_one_point_two = 0;
static int claims_one_point_three = 0;
static int claims_one_point_four = 0;

static char* get_val_from_file(FILE *fp, const char *key)
{
    if (! fp)
        return NULL;
    char *val = NULL;
    char buf[2048] = {0};
    while (fgets(buf, 2048, fp))
    {
        if (strncmp(buf, key, strlen(key)) == 0)
        {
            val = strdup((char*)buf + strlen(key) +1);
            break;
        }
    }

    return val;
}

int verify_file(char *pFileName)
{
    if(!strncmp(pFileName, "/sys", strlen("/sys")))
    {
        return 1;
    }
    else if(!strncmp(pFileName, "/tmp", strlen("/tmp")))
    {
        return 1;
    }
    else{
        return 0;
    }
}

char *cmd_exec(char *file)
{
    char *res = (char *)malloc(sizeof(char) * 128);
    memset(res, 0, 128);
    int fd[2];
    if (pipe(fd) == -1) {
        free(res);
        return NULL;
    }

    pid_t pid = fork();
    if (pid == -1) {
        free(res);
        return NULL;
    }

    if (pid == 0) {
        close(fd[0]); // 关闭读取端
        dup2(fd[1], STDOUT_FILENO); // 将管道写入端连接到标准输出

        // 使用execve执行所需的命令，例如：
        char* args[] = {"edid-decode", file, NULL};
        execve("/bin/edid-decode", args, NULL);

        // 如果execve执行失败，需要打印错误信息并退出
        perror("Failed to execute command");
        exit(1);
    } else {
        close(fd[1]); // 关闭写入端

        const char* targetLine1 = "Display Product Name"; // 要匹配的目标行
        const char* targetLine2 = "Alphanumeric Data String"; // 要匹配的目标行
        
        FILE* fp = fdopen(fd[0], "r"); // 将读取端转换为文件指针
        char *val = get_val_from_file(fp, targetLine1);
        if(!val)
        {
            char *value = get_val_from_file(fp, targetLine2);
            if(!value)
            {
                free(res);
                return NULL;
            }
            else{
                strcpy(res, value);
                free(value);
            }
        }
        else{
            strcpy(res, val);
            free(val);
        }

        fclose(fp); // 关闭文件指针

        // 等待子进程退出并获取退出状态
        int status;
        waitpid(pid, &status, 0);
    }
    return res;
}

void kdk_edid(char *name)
{
    char edid_text[2048] = "\0";
    char *edid_character = (char *)malloc(sizeof(char) * 1024);
    if(!edid_character)
    {
        return;
    }
    FILE *pipeLine = popen("xrandr --prop", "r");  // 建立流管道
    if (!pipeLine)
    {
        free(edid_character);
        return;
    }
    int i = 0;
    memset(edid_character, 0, 1024);
    while(fgets(edid_text, sizeof(edid_text), pipeLine))
    {
        if(i != 1)
        {
            if(strstr(edid_text, name))
            {
                i = 1;
                continue;
            }
            else{
                continue;
            }
        }
        else{
            if(strstr(edid_text, "EDID"))
            {
                continue;
            }
            if(strstr(edid_text, "TearFree") || strstr(edid_text, "GAMMA_LUT_SIZE:") || strstr(edid_text, "non-desktop:"))
            {
                break;
            }
        
            strstripspace(edid_text);
            strcat(edid_character, edid_text);
        }
    }
    pclose(pipeLine);

    FILE *fp = fopen("/tmp/sdk-edid", "w");
    if(!fp)
    {
        free(edid_character);
        return;
    }
    fprintf(fp, "%s", edid_character);
    
    fclose(fp);
    free(edid_character);
    return;

}

char **kdk_edid_get_interface()
{
    Display *disp;
    XRRScreenResources *screen;
    XRROutputInfo *info;
    int iscres;
    int icrtc;
    char tmp[128] = "\0";
    char **res = NULL;
    int count = 0;
    char test[100] = {0};

    disp = XOpenDisplay(0);
    screen = XRRGetScreenResources(disp, DefaultRootWindow(disp));
    for (iscres = screen->noutput; iscres > 0;)
    {
        --iscres;

        info = XRRGetOutputInfo(disp, screen, screen->outputs[iscres]);
        if (info->connection == RR_Connected)
        {
            for (icrtc = info->ncrtc; icrtc > 0;)
            {
                memset(tmp, 0, sizeof(tmp));

                if (strstr(test, info->name))
                {
                    break;
                }
                else
                {
                    strcpy(tmp, info->name);
                    strcpy(test, tmp);
                    --icrtc;
                }

                res = realloc(res, (count + 1) * sizeof(char *));
                if (!res)
                {
                    klog_err("内存申请失败：%s\n", strerror(errno));
                    goto err_out;
                }
                res[count] = malloc(512 * sizeof(char) + 1);
                if (!res[count])
                {
                    klog_err("内存申请失败：%s\n", strerror(errno));
                    goto err_out;
                }
                strcpy(res[count], tmp);
                count++;
            }
        }
        XRRFreeOutputInfo(info);
    }
    XRRFreeScreenResources(screen);
    res[count] = NULL;
    goto out;

err_out:
    while (count)
    {
        free(res[count - 1]);
        count--;
    }
    SAFE_FREE(res);
    return NULL;
out:
    return res;
}

static unsigned char *extract_edid(int fd)
{
    char *ret = NULL;
    char *start, *c;
    unsigned char *out = NULL;
    int state = 0;
    int lines = 0;
    int i;
    int out_index = 0;
    int len, size;

    size = 1 << 10;
    ret = malloc(size);
    len = 0;

    if (ret == NULL)
        return NULL;

    for (;;)
    {
        i = read(fd, ret + len, size - len);
        if (i < 0)
        {
            free(ret);
            return NULL;
        }
        if (i == 0)
            break;
        len += i;
        if (len == size)
        {
            char *t;
            size <<= 1;
            t = realloc(ret, size);
            if (t == NULL)
            {
                free(ret);
                return NULL;
            }
            ret = t;
        }
    }

    start = strstr(ret, "EDID_DATA:");
    if (start == NULL)
        start = strstr(ret, "EDID:");
    /* Look for xrandr --verbose output (lines of 16 hex bytes) */
    if (start != NULL)
    {
        const char indentation1[] = "                ";
        const char indentation2[] = "\t\t";
        /* Used to detect that we've gone past the EDID property */
        const char half_indentation1[] = "        ";
        const char half_indentation2[] = "\t";
        const char *indentation;
        char *s;

        lines = 0;
        for (i = 0;; i++)
        {
            int j;

            /* Get the next start of the line of EDID hex, assuming spaces for indentation */
            s = strstr(start, indentation = indentation1);
            /* Did we skip the start of another property? */
            if (s && s > strstr(start, half_indentation1))
                break;

            /* If we failed, retry assuming tabs for indentation */
            if (!s)
            {
                s = strstr(start, indentation = indentation2);
                /* Did we skip the start of another property? */
                if (s && s > strstr(start, half_indentation2))
                    break;
            }

            if (!s)
                break;

            lines++;
            start = s + strlen(indentation);

            s = realloc(out, lines * 16);
            if (!s)
            {
                free(ret);
                free(out);
                return NULL;
            }
            out = (unsigned char *)s;
            c = start;
            for (j = 0; j < 16; j++)
            {
                char buf[3];
                /* Read a %02x from the log */
                if (!isxdigit(c[0]) || !isxdigit(c[1]))
                {
                    if (j != 0)
                    {
                        lines--;
                        break;
                    }
                    free(ret);
                    free(out);
                    return NULL;
                }
                buf[0] = c[0];
                buf[1] = c[1];
                buf[2] = 0;
                out[out_index++] = strtol(buf, NULL, 16);
                c += 2;
            }
        }

        free(ret);
        edid_lines = lines;
        return out;
    }

    /* Is the EDID provided in hex? */
    for (i = 0; i < 32 && isxdigit(ret[i]); i++)
        ;
    if (i == 32)
    {
        out = malloc(size >> 1);
        if (out == NULL)
        {
            free(ret);
            return NULL;
        }

        for (c = ret; *c; c++)
        {
            char buf[3];

            if (*c == '\n')
                continue;

            /* Read a %02x from the log */
            if (!isxdigit(c[0]) || !isxdigit(c[1]))
            {
                free(ret);
                free(out);
                return NULL;
            }

            buf[0] = c[0];
            buf[1] = c[1];
            buf[2] = 0;

            out[out_index++] = strtol(buf, NULL, 16);
            c++;
        }

        free(ret);
        edid_lines = out_index >> 4;
        return out;
    }

    /* wait, is this a log file? */
    for (i = 0; i < 8; i++)
    {
        if (!isascii(ret[i]))
        {
            edid_lines = len / 16;
            return (unsigned char *)ret;
        }
    }

    /* I think it is, let's go scanning */
    if (!(start = strstr(ret, "EDID (in hex):")))
        return (unsigned char *)ret;
    if (!(start = strstr(start, "(II)")))
        return (unsigned char *)ret;

    for (c = start; *c; c++)
    {
        if (state == 0)
        {
            char *s;
            /* skip ahead to the : */
            s = strstr(c, ": \t");
            if (!s)
                s = strstr(c, ":     ");
            if (!s)
                break;
            c = s;
            /* and find the first number */
            while (!isxdigit(c[1]))
                c++;
            state = 1;
            lines++;
            s = realloc(out, lines * 16);
            if (!s)
            {
                free(ret);
                free(out);
                return NULL;
            }
            out = (unsigned char *)s;
        }
        else if (state == 1)
        {
            char buf[3];
            /* Read a %02x from the log */
            if (!isxdigit(*c))
            {
                state = 0;
                continue;
            }
            buf[0] = c[0];
            buf[1] = c[1];
            buf[2] = 0;
            out[out_index++] = strtol(buf, NULL, 16);
            c++;
        }
    }

    edid_lines = lines;

    free(ret);

    return out;
}

const float kdk_edid_get_gamma(char *name)
{
    if(!name)
    {
        return 0.00;
    }
    int fd;
    unsigned char *edid;
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    float ga = 0.00;
    char canonical_filename[100] = "\0";
    char filename[100] = "\0";
    char str_name[32] = "\0";
    strcpy(str_name, name);
    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        return 0.00;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        closedir(dirfd);
        return 0.00;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename)
            || (fd = open(canonical_filename, O_RDONLY)) == -1)
    {
	    // close(fd);
        kdk_edid(name);
        if (!realpath("/tmp/sdk-edid", filename) || !verify_file(filename))
        {
            closedir(dirfd);
            return 0.00;
        }
        FILE *fs = fopen(filename, "r");
        if(!fs)
        {
            closedir(dirfd);
            return 0.00;
        }
	    fclose(fs);
        FILE *fp = popen("edid-decode /tmp/sdk-edid", "r");
        if(!fp)
        {
            closedir(dirfd);
            return 0.00;
        }
        char *gamma = get_val_from_file(fp, "Gamma");
        if(gamma)
        {
            strstripspace(gamma);
            ga = atof(gamma);
        }
        free(gamma);
        closedir(dirfd);
        pclose(fp);
        return ga;

    }
    else{
        edid = extract_edid(fd);
        if (!edid)
        {
            goto err_out;
        }

        if (!edid || memcmp(edid, "\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00", 8))
        {
            goto err_out;
        }

        switch (edid[0x13])
        {
        case 4:
            claims_one_point_four = 1;
        case 3:
            claims_one_point_three = 1;
        case 2:
            claims_one_point_two = 1;
        default:
            break;
        }
        close(fd);
        closedir(dirfd);

        if (edid[0x17] == 0xff)
        {
            if (claims_one_point_four)
                ;
            else{
                free(edid);
                return 1.0;
            }
        }
        else
        {
            ga = ((edid[0x17] + 100.0) / 100.0);
            free(edid);
            return ga;
        }
    }

err_out:
    close(fd);
    closedir(dirfd);
    if(edid)
    {
        free(edid);
    }
    return 0.00;
}

float kdk_edid_get_size(char *name)
{
    if(!name)
    {
        return 0.00;
    }
    int fd;
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    char canonical_filename[100] = "\0";
    float size = 0.00;
    char filename[100] = "\0";
    char str_name[32] = "\0";
    strcpy(str_name, name);

    FILE *fp = NULL;
    int num1 = 0;
    int num2 = 0;

    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        return 0.00;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        closedir(dirfd);
        return 0.00;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename) \
        || (fd = open(canonical_filename, O_RDONLY)) == -1)
    {
		// close(fd);
        kdk_edid(name);
        if (!realpath("/tmp/sdk-edid", filename) || !verify_file(filename))
        {
            closedir(dirfd);
            return 0.00;
        }
        FILE *fs = fopen(filename, "r");
        if(!fs)
        {
            closedir(dirfd);
            return 0.00;
        }
		fclose(fs);
        fp = popen("edid-decode /tmp/sdk-edid", "r");
        if(!fp)
        {
            closedir(dirfd);
            return 0.00;
        }
        
    }
    else{
        char repath[527] = "\0";
        sprintf(repath, "edid-decode %s", path);
        fp = popen(repath, "r");
        if(!fp)
        {
            closedir(dirfd);
            return 0.00;
        }
    }
    char xsize[16] = "\0";
    char ysize[16] = "\0";
    char *tmp = get_val_from_file(fp, "Maximum image size");
    if(!tmp)
    {
        printf("test\n");
        closedir(dirfd);
        pclose(fp);
        return 0.00;
    }
    char *msize = get_val_from_file(fp, "Detailed mode");
    if(msize)
    {
        sscanf(msize, "%*s %*s %*s %s %*s %*s %s", xsize, ysize);
        num1 = atoi(xsize);
        num2 = atoi(ysize);
        size = sqrt(pow(num1, 2) + pow(num2, 2)) / 25.4;
        free(msize);
        free(tmp);
    }
    else if(tmp)
    {
        sscanf(tmp, "%s %*s %*s %s", xsize, ysize);
        num1 = atoi(xsize);
        num2 = atoi(ysize);
        size = sqrt(pow(num1, 2) + pow(num2, 2)) / 2.54;
        free(tmp);
    }
    closedir(dirfd);
    pclose(fp);

    // greatwall
    if(!strcmp("24AL60", kdk_edid_get_model(name)))
    {
        size = 23.8;
    }

    // X100
    if(!strcmp("LM133LF-8L02", kdk_edid_get_model(name)))
    {
        size = 13.3;
    }
    
    return size;
}

char *kdk_edid_get_max_resolution(char *name)
{
    if(!name)
    {
        return NULL;
    }
    char edid_text[2048] = "\0";
    char tmp[64] = "\0";
    char *res = (char *)malloc(sizeof(char) * 64);
    if(!res)
    {
        return NULL;
    }
    memset(res, 0, 64);
    FILE *pipeLine = popen("xrandr --prop", "r");  // 建立流管道
    if (!pipeLine)
    {
        free(res);
        return NULL;
    }
    int i = 0;
    while(fgets(edid_text, sizeof(edid_text), pipeLine))
    {
        if(i != 1)
        {
            if(strstr(edid_text, name))
            {
                i = 1;
            }
            continue;
        }
        if(strstr(edid_text, "+"))
        {
            strncpy(tmp, edid_text, 63);
        }

    }
    pclose(pipeLine);
    sscanf(tmp, "%s", res);
    if(strlen(res) == 0)
    {
        free(res);
        return NULL;
    }
    
    return res;
}

/* extract a string from a detailed subblock, checking for termination */
char *extract_string(const unsigned char *x, unsigned len)
{
    static char s[50];
    int seen_newline = 0;
    unsigned i;

    memset(s, 0, sizeof(s));

    for (i = 0; i < len; i++)
    {
        if (isgraph(x[i]))
        {
            s[i] = x[i];
        }
        else if (!seen_newline)
        {
            if (x[i] == 0x0a)
            {
                seen_newline = 1;
                if (!i)
                    ;
                else if (s[i - 1] == 0x20)
                    ;
            }
            else if (x[i] == 0x20)
            {
                s[i] = x[i];
            }
            else
            {
                return s;
            }
        }
        else if (x[i] != 0x20)
        {
            return s;
        }
    }
    /* Does the string end with a space? */
    if (!seen_newline && s[len - 1] == 0x20)
        ;

    return s;
}

char *detailed_block(const unsigned char *x)
{
    static const unsigned char zero_descr[18] = {0};

    if (!memcmp(x, zero_descr, sizeof(zero_descr)))
    {
        return NULL;
    }

    switch (x[3])
    {
    case 0xfc:
        return extract_string(x + 5, 13);
    }
    return NULL;
}

char *kdk_edid_get_model(char *name)
{
    if(!name)
    {
        return NULL;
    }
    int fd;
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    char canonical_filename[100] = "\0";
    char filename[100] = "\0";
    char *edid_text = NULL;
    char *model = (char *) malloc(128 *sizeof(char));
    if(!model)
    {
        return NULL;
    }
    memset(model, 0, 128);
    char str_name[32] = "\0";
    strcpy(str_name, name);
    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        free(model);
        return NULL;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        free(model);
        closedir(dirfd);
        return NULL;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    closedir(dirfd);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename) \
                        || (fd = open(canonical_filename, O_RDONLY)) == -1)
    {
        kdk_edid(name);
        if (!realpath("/tmp/sdk-edid", filename) || !verify_file(filename))
        {
            free(model);
            return NULL;
        }
        FILE *fs = fopen(filename, "r");
        if(!fs)
        {
            free(model);
            return NULL;
        }
		fclose(fs);
        FILE *fp = popen("edid-decode /tmp/sdk-edid", "r");
        if(!fp)
        {
            free(model);
            return NULL;
        }
        edid_text = get_val_from_file(fp, "Display Product Name");
        if(!edid_text)
        {
            char *str_model = get_val_from_file(fp, "Alphanumeric Data String");
            if(!str_model)
            {
                free(edid_text);
                goto err_out;
            }
            else{
                strcpy(model, str_model);
            }
            free(str_model);
        }
        else{
            strcpy(model, edid_text);
        }
        free(edid_text);

        if(strlen(model) == 0)
        {
            goto err_out;
        }
        strstripspace(model);
        pclose(fp);
        return model;
    }

    char *cmd_val = cmd_exec(path);
    if(!cmd_val)
    {
        hd_data_t *hd_data;
        hd_data = (hd_data_t *)calloc(1, sizeof *hd_data);
        if(!hd_data)
        {
            goto err_out;
        }
        char src_model[128] = "\0";
        memset(src_model, 0, 128);

        hd_data->progress = NULL;
        hd_data->debug = ~(HD_DEB_DRIVER_INFO | HD_DEB_HDDB);

        hd_t *hd, *hd0;
        hd0 = hd_list(hd_data, hw_monitor, 1, NULL);
        for (hd = hd0; hd; hd = hd->next)
        {
            if(hd->model)
            {
                strcpy(src_model, hd->model);
            }
        }
        free(hd_data);
        if(strlen(src_model) == 0)
        {
            goto err_out;
        }
        else{
            strcpy(model, src_model);
        }
    }
    else{
        strcpy(model, cmd_val);
        free(cmd_val);
    }
    if(strlen(model) == 0)
    {
        goto err_out;
    }
    strstripspace(model);
    return model;
err_out:
    free(model);
    return NULL;
}

char *kdk_edid_get_visible_area(char *name)
{
    if(!name)
    {
        return NULL;
    }
    int fd;
    unsigned char *edid;
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    char canonical_filename[100] = "\0";
    char tmp[20] = "\0";
    char filename[100] = "\0";
    // char edid_text[24] = "\0";
    char str_name[32] = "\0";
    strcpy(str_name, name);

    char *area = (char *)malloc(sizeof(char) * 32);
    if (!area)
    {
        klog_err("内存申请失败：%s\n", strerror(errno));
        return NULL;
    }

    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        free(area);
        return NULL;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        free(area);
        closedir(dirfd);
        return NULL;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename) \
                    || (fd = open(canonical_filename, O_RDONLY)) == -1)
    {
		// close(fd);
        kdk_edid(name);
        if (!realpath("/tmp/sdk-edid", filename) || !verify_file(filename))
        {
            closedir(dirfd);
            free(area);
            return NULL;
        }
        FILE *fs = fopen(filename, "r");
        if(!fs)
        {
            closedir(dirfd);
            free(area);
            return NULL;
        }
		fclose(fs);
        FILE *fp = popen("edid-decode /tmp/sdk-edid", "r");
        if(!fp)
        {
            closedir(dirfd);
            free(area);
            return NULL;
        }
        char *msize = get_val_from_file(fp, "Maximum image size");
        if(!msize)
        {
            closedir(dirfd);
            pclose(fp);
            free(area);
            return NULL;
        }
        char *buf[3];
        int i = 0;
        char *p = strtok(msize, "x");
        while (p)
        {
            buf[i] = p;
            i++;
            p = strtok(NULL, "x");
        }
        char fs1[12] = "\0";
        char fs2[12] = "\0";
        strcpy(fs1, buf[0]);
        strcpy(fs2, buf[1]);
        strskipspace(fs1);
        strskipspace(fs2);
        int num1 = 0;
        int num2 = 0;
        sscanf(fs1, "%d", &num1);
        sscanf(fs2, "%d", &num2);
        sprintf(tmp, "%u X %u cm", num1, num2);
        strcpy(area, tmp);

        closedir(dirfd);
        pclose(fp);
        free(msize);
        return area;
    }

    edid = extract_edid(fd);
    if (!edid)
    {
        goto err_out;
    }

    if (!edid || memcmp(edid, "\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00", 8))
    {
        goto err_out;
    }
    close(fd);
    closedir(dirfd);

    if (edid[0x15] && edid[0x16])
    {
        sprintf(tmp, "%u X %u cm", edid[0x15], edid[0x16]);
    }
    strcpy(area, tmp);
    free(edid);
    return area;

err_out:
    free(area);
    close(fd);
    closedir(dirfd);
    if(edid)
    {
        free(edid);
    }
    return NULL;
}

static char *manufacturer_name(const unsigned char *x)
{
    static char name[4];

    name[0] = ((x[0] & 0x7c) >> 2) + '@';
    name[1] = ((x[0] & 0x03) << 3) + ((x[1] & 0xe0) >> 5) + '@';
    name[2] = (x[1] & 0x1f) + '@';
    name[3] = 0;

    if (!isupper(name[0]) || !isupper(name[1]) || !isupper(name[2]))
        ;

    return name;
}

char *kdk_edid_get_manufacturer(char *name)
{
    if(!name)
    {
        return NULL;
    }
    int fd;
    unsigned char *edid;
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    char canonical_filename[100] = "\0";
    char filename[100] = "\0";
    char str_name[32] = "\0";
    strcpy(str_name, name);

    char *manufacturer = (char *)malloc(sizeof(char) * 128);
    if(!manufacturer)
    {
        return NULL;
    }
    memset(manufacturer, 0, 128);

    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        free(manufacturer);
        return NULL;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        free(manufacturer);
        closedir(dirfd);
        return NULL;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename) \
                            || (fd = open(canonical_filename, O_RDONLY)) == -1)
    {
		// close(fd);
        kdk_edid(name);
        if (!realpath("/tmp/sdk-edid", filename) || !verify_file(filename))
        {
            closedir(dirfd);
            free(manufacturer);
            return NULL;
        }
        FILE *fs = fopen(filename, "r");
        if(!fs)
        {
            closedir(dirfd);
            free(manufacturer);
            return NULL;
        }
		fclose(fs);
        FILE *fp = popen("edid-decode /tmp/sdk-edid", "r");
        if(!fp)
        {
            closedir(dirfd);
            free(manufacturer);
            return NULL;
        }
        char *msize = get_val_from_file(fp, "Manufacturer");
        if(!msize)
        {
            closedir(dirfd);
            pclose(fp);
            free(manufacturer);
            return NULL;
        }
        char tmp[128] = "\0";
        sscanf(msize, "%s", tmp);
        if(tmp[0] == '\0')
        {
            free(msize);
            closedir(dirfd);
            pclose(fp);
            free(manufacturer);
            return NULL;
        }
        strcpy(manufacturer, tmp);
        free(msize);
        closedir(dirfd);
        pclose(fp);
        return manufacturer;
    }

    edid = extract_edid(fd);
    if (!edid)
    {
        goto err_out;
    }

    if (!edid || memcmp(edid, "\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00", 8))
    {
        goto err_out;
    }
    close(fd);
    closedir(dirfd);
    char *mn = manufacturer_name(edid + 0x08);
    free(edid);
    if(mn)
    {
        strcpy(manufacturer, mn);
        return manufacturer;
    }
    else
    {
        free(manufacturer);
        return NULL;
    }

err_out:
    close(fd);
    closedir(dirfd);
    free(manufacturer);
    if(edid)
    {
        free(edid);
    }
    return NULL;
}

int kdk_edid_get_week(char *name)
{
    if(!name)
    {
        return -1;
    }
    int fd;
    unsigned char *edid;
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    char canonical_filename[100] = "\0";
    char tmp[20] = {0};
    unsigned edid_minor;
    int wek;
    char filename[100] = "\0";
    char edid_text[11520] = "\0";
    char edid_week[32] = "\0";
    char str_name[32] = "\0";
    strcpy(str_name, name);


    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        return -1;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        closedir(dirfd);
        return -1;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename) \
                        || (fd = open(canonical_filename, O_RDONLY)) == -1)
    {
		// close(fd);
        kdk_edid(name);
        if (!realpath("/tmp/sdk-edid", filename) || !verify_file(filename))
        {
            closedir(dirfd);
            return -1;
        }
        FILE *fs = fopen(filename, "r");
        if(!fs)
        {
            closedir(dirfd);
            return -1;
        }
		fclose(fs);
        FILE *fp = popen("edid-decode /tmp/sdk-edid", "r");
        if(!fp)
        {
            closedir(dirfd);
            return -1;
        }
        while(fgets(edid_text, sizeof(edid_text), fp))
        {
            if(strstr(edid_text, "Made in week"))
            {
                strcpy(edid_week, edid_text);
            }
        }
        int year = 0;
        sscanf(edid_week, "%*s %*s %*s %d %*s %d", &wek ,&year);

        closedir(dirfd);
        pclose(fp);
        return wek;
    }

    edid = extract_edid(fd);
    if (!edid)
    {
        goto err_out;
    }

    if (!edid || memcmp(edid, "\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00", 8))
    {
        goto err_out;
    }
    close(fd);
    closedir(dirfd);

    if (edid[0x12] == 1)
    {
        edid_minor = edid[0x13];
        if (edid_minor > 4)
            ;
        if (edid_minor < 3)
            ;
    }
    else
    {
        ;
    }

    unsigned char week = edid[0x10];

    if (week)
    {
        if (edid_minor <= 3 && week == 0xff)
            ;
        if (edid_minor <= 3 && week == 54)
            ;
        if (week != 0xff && week > 54)
            ;
        if (week != 0xff)
        {
            sprintf(tmp, "%hhu", week);
            wek = strtol(tmp, NULL, 10);
        }
    }
    free(edid);
    return wek;

err_out:
    close(fd);
    closedir(dirfd);
    if(edid)
    {
        free(edid);
    }
    return -1;
}

int kdk_edid_get_year(char *name)
{
    if(!name)
    {
        return -1;
    }
    int fd;
    unsigned char *edid;
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    char canonical_filename[100] = "\0";
    unsigned edid_minor;
    char filename[100] = "\0";
    char edid_text[11520] = "\0";
    char edid_week[32] = "\0";
    int year = 0;
    char str_name[32] = "\0";
    strcpy(str_name, name);


    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        return -1;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        closedir(dirfd);
        return -1;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename) \
                        || (fd = open(canonical_filename, O_RDONLY)) == -1)
    {
		// close(fd);
        kdk_edid(name);
        if (!realpath("/tmp/sdk-edid", filename) || !verify_file(filename))
        {
            closedir(dirfd);
            return -1;
        }
        FILE *fs = fopen(filename, "r");
        if(!fs)
        {
            closedir(dirfd);
            return -1;
        }
		fclose(fs);
        FILE *fp = popen("edid-decode /tmp/sdk-edid", "r");
        if(!fp)
        {
            closedir(dirfd);
            return -1;
        }
        while(fgets(edid_text, sizeof(edid_text), fp))
        {
            if(strstr(edid_text, "Made in week"))
            {
                strcpy(edid_week, edid_text);
            }
        }
        int week = 0;
        sscanf(edid_week, "%*s %*s %*s %d %*s %d", &week ,&year);

        closedir(dirfd);
        pclose(fp);
        return year;
    }

    edid = extract_edid(fd);
    if (!edid)
    {
        goto err_out;
    }

    if (!edid || memcmp(edid, "\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00", 8))
    {
        goto err_out;
    }
    close(fd);
    closedir(dirfd);

    if (edid[0x12] == 1)
    {
        edid_minor = edid[0x13];
        if (edid_minor > 4)
            ;
        if (edid_minor < 3)
            ;
    }
    else
    {
        ;
    }
    year = 1990 + edid[0x11];
    free(edid);
    return year;

err_out:
    close(fd);
    closedir(dirfd);
    if(edid)
    {
        free(edid);
    }
    return -1;
}

int kdk_edid_get_primary(char *name)
{
    if(!name)
    {
        return -1;
    }
    int primary = 0;
    Display *display = XOpenDisplay(NULL);
    if (!display)
    {
        return -1;
    }

    Window root = DefaultRootWindow(display);
    RROutput primary_output = XRRGetOutputPrimary(display, root);

    XRRScreenResources *screen = XRRGetScreenResources(display, root);
    assert(screen);

    for (int i = 0; i < screen->noutput; i++)
    {
        XRROutputInfo *output_info = XRRGetOutputInfo(
            display, screen, screen->outputs[i]);
        assert(output_info);

        if (output_info->connection == RR_Connected)
        {
            XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(
                display, screen, output_info->crtc);
            assert(crtc_info);

            for (int j = 0; j < crtc_info->noutput; j++)
                if (crtc_info->outputs[j] == primary_output)
                {
                    if (strstr(output_info->name, name))
                        primary = 1;
                }
            XRRFreeCrtcInfo(crtc_info);
        }
        XRRFreeOutputInfo(output_info);
    }
    XRRFreeScreenResources(screen);
    XCloseDisplay(display);

    return primary;
}

char *kdk_edid_get_resolution(char *name)
{
    if(!name)
    {
        return NULL;
    }
    Display *disp;
    XRRScreenResources *screen;
    XRROutputInfo *info;
    XRRCrtcInfo *crtc_info;
    int iscres;
    int icrtc;
    char tmp[20];
    char *resolution = (char *)malloc(sizeof(char) * 20);
    if(!resolution)
    {
        return NULL;
    }
    memset(resolution, 0, 20);

    disp = XOpenDisplay(0);
    screen = XRRGetScreenResources(disp, DefaultRootWindow(disp));
    for (iscres = screen->noutput; iscres > 0;)
    {
        --iscres;

        info = XRRGetOutputInfo(disp, screen, screen->outputs[iscres]);
        if (info->connection == RR_Connected)
        {
            for (icrtc = info->ncrtc; icrtc > 0;)
            {
                --icrtc;

                crtc_info = XRRGetCrtcInfo(disp, screen, screen->crtcs[icrtc]);
                if (crtc_info->width != 0 || crtc_info->height != 0)
                {
                    if (strstr(info->name, name))
                    {
                        sprintf(tmp, "%dx%d", crtc_info->width, crtc_info->height);
                        strcpy(resolution, tmp);
                    }
                    XRRFreeCrtcInfo(crtc_info);
                }
            }
        }
        XRRFreeOutputInfo(info);
    }
    XRRFreeScreenResources(screen);
    if(strlen(resolution) == 0)
    {
        free(resolution);
        return NULL;
    }
    return resolution;
}

char* kdk_edid_get_ratio(char *name)
{
    if(!name)
    {
        return NULL;
    }
    Display *disp;
    XRRScreenResources *screen;
    XRROutputInfo *info;
    XRRCrtcInfo *crtc_info;
    int iscres;
    int icrtc;
    char tmp[32] = "\0";
    char *ratio = (char *)malloc(sizeof(char) * 32);
    if(!ratio)
    {
        return NULL;
    }

    disp = XOpenDisplay(0);
    screen = XRRGetScreenResources(disp, DefaultRootWindow(disp));
    for (iscres = screen->noutput; iscres > 0;)
    {
        --iscres;

        info = XRRGetOutputInfo(disp, screen, screen->outputs[iscres]);
        if (info->connection == RR_Connected)
        {
            for (icrtc = info->ncrtc; icrtc > 0;)
            {
                --icrtc;

                crtc_info = XRRGetCrtcInfo(disp, screen, screen->crtcs[icrtc]);
                if (crtc_info->width != 0 || crtc_info->height != 0)
                {
                    if (strstr(info->name, name))
                    {
                        int ret = (crtc_info->width*100 / crtc_info->height);
                        if (ret == 125)
                            strcpy(tmp, "5:4");
                        else if (ret == 133)
                            strcpy(tmp, "4:3");
                        else if (ret == 160)
                            strcpy(tmp, "16:10");
                        else if (ret == 177)
                            strcpy(tmp, "16:9");
                    }
                    XRRFreeCrtcInfo(crtc_info);
                }
            }
        }
        XRRFreeOutputInfo(info);
    }
    XRRFreeScreenResources(screen);
    if(tmp[0] == "\0")
    {
        free(ratio);
        return NULL;
    }
    strcpy(ratio, tmp);
    return ratio;
}

char* kdk_edid_get_character(char *name)
{
    if(!name)
    {
        return NULL;
    }
    FILE *fd = NULL;
    char *chater = (char *)malloc(sizeof(char) * 2048);
    if(!chater)
    {
        return NULL;
    }
    char path[512] = "\0";
    DIR *dirfd = NULL;
    struct dirent *direfd = NULL;
    char canonical_filename[100] = "\0";
    unsigned char buf[2048] = "\0";
    char hex[2048] = "\0";
    char str_name[32] = "\0";
    strcpy(str_name, name);


    dirfd = opendir("/sys/class/drm");
    if (!dirfd)
    {
        free(chater);
        return NULL;
    }
    char **fg = strsplit(str_name, '-');
    if (!fg)
    {
        free(chater);
        closedir(dirfd);
        return NULL;
    }

    while ((direfd = readdir(dirfd)) != NULL)
    {
        if (strstr(direfd->d_name, fg[0]))
        {
            sprintf(path, "/sys/class/drm/%s/edid", direfd->d_name);
        }
    }
    free(fg);
    if (!realpath(path, canonical_filename) || !verify_file(canonical_filename) \
                                     || !(fd = fopen(canonical_filename, "rb")))
    {
        closedir(dirfd);
        free(chater);
        char edid_text[2048] = "\0";
        char *edid_character = (char *)malloc(sizeof(char) * 2048);
        if(!edid_character)
        {
            return NULL;
        }
        FILE *pipeLine = popen("xrandr --prop", "r");  // 建立流管道
        if (!pipeLine)
        {
            free(edid_character);
            return NULL;
        }
        int i = 0;
        memset(edid_character, 0, 2048);
        while(fgets(edid_text, sizeof(edid_text), pipeLine))
        {
            if(i != 1)
            {
                if(strstr(edid_text, name))
                {
                    i = 1;
                    continue;
                }
                else{
                    continue;
                }
            }
	        else{
                if(strstr(edid_text, "EDID"))
                {
                    continue;
                }
                if(strstr(edid_text, "TearFree") || strstr(edid_text, "GAMMA_LUT_SIZE:") || strstr(edid_text, "non-desktop:"))
                {
                        break;
                }

                if(edid_text[0] != '\0')
                {
                    strstripspace(edid_text);
                    strcat(edid_character, edid_text);
                }
                else{
                    free(edid_character);
                    return NULL;
                }
	        }
        }
        return edid_character;
    }

    fread(buf, sizeof(buf), 1, fd);
    long size = ftell(fd);
    fclose(fd);
    closedir(dirfd);

    for (int i = 0; i < size; i++) {
        sprintf(hex + i * 3, "%02X ", buf[i]);
    }
    strcpy(chater, hex);
    return chater;
}

inline void kdk_edid_freeall(char **list)
{
    if (!list)
        return;
    size_t index = 0;
    while (list[index])
    {
        free(list[index]);
        index++;
    }
    free(list);
}
