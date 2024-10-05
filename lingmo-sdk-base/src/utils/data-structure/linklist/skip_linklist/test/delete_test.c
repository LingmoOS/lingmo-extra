#include <skip_linklist.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#define ARRAY_SIZE    20

int array[ARRAY_SIZE];
int del_flag[ARRAY_SIZE] = {0};

void print_list(lingmosdk_skiplist *list)
{
    for (int i = list->max_levels - 1; i >= 0; i --)
    {
        lingmosdk_skiplist_node *curnode = list->children[i];
        printf("Level[%d]: ", i);
        while (curnode)
        {
            printf("%d->", curnode->key);
            curnode = curnode->next[i];
        }
        printf("NULL\n");
    }
}

int test_list_delete(lingmosdk_skiplist *list)
{
    for (int i = 0; i < ARRAY_SIZE; i ++)
    {
        if (random() % 2)
        {
            del_flag[i] = 1;
            lingmosdk_skiplist_delete(list, array[i]);
            printf("Deleted %d\n", array[i]);
        }
    }

    for (int i = 0; i < ARRAY_SIZE; i ++)
    {
        if (del_flag[i] && lingmosdk_skiplist_search(list, array[i]).num != -1)
        {
            printf("Skiplist delete test failed.\n");
            return -1;
        }
    }

    printf("Skiplist order test pass.\n");

    return 0;
}

int main()
{
    lingmosdk_skiplist *list = lingmosdk_create_skiplist();

    lingmosdk_skiplist_setmaxlevels(list, 5);
    
    srand(time(NULL));

    for (int i = 0; i < ARRAY_SIZE; i ++)
    {
        array[i] = i;
        lingmosdk_skiplist_insert(list, i, (lingmosdk_listdata)i);
        printf("%d has been insert.\n", i);
        sleep(1);
    }
    
    print_list(list);

    int res = test_list_delete(list);

    print_list(list);

    lingmosdk_destroy_skiplist(list);

    return res;
}