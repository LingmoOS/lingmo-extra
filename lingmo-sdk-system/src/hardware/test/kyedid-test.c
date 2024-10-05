#include "../libkyedid.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char** name = kdk_edid_get_interface();
    size_t count = 0;
    while (name[count])
    {
        printf("当前接口 = %s\n", name[count]);
        float gamma = kdk_edid_get_gamma(name[count]);
        printf("gamma = %0.2f\n", gamma);
        float size = kdk_edid_get_size(name[count]);
        printf("size = %0.1f\n", size);
        char *max_resolution = kdk_edid_get_max_resolution(name[count]);
        printf("max_resolution = %s\n", max_resolution);
        free(max_resolution);
        char *model = kdk_edid_get_model(name[count]);
        printf("model = %s\n", model);
        free(model);
        char *area = kdk_edid_get_visible_area(name[count]);
        printf("area = %s\n", area);
        free(area);
        char *manufacturer = kdk_edid_get_manufacturer(name[count]);
        printf("manufacturer = %s\n", manufacturer);
        free(manufacturer);
        int week = kdk_edid_get_week(name[count]);
        printf("week = %d\n", week);
        int year = kdk_edid_get_year(name[count]);
        printf("year = %d\n", year);
        int primary = kdk_edid_get_primary(name[count]);
        printf("%s\n", primary == 1 ? "是" : "否");

        char *resolution = kdk_edid_get_resolution(name[count]);
        printf("resolution = %s\n", resolution);
        free(resolution);

        char *ratio = kdk_edid_get_ratio(name[count]);
        printf("ratio = %s\n", ratio);
        free(ratio);

        char *character = kdk_edid_get_character(name[count]);
        printf("character = %s\n", character);
        free(character);

        printf("---------------------------------------\n");
        count ++;
    }
    kdk_edid_freeall(name);
    return 0;
} 
