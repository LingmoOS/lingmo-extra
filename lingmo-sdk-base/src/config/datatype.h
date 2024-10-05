#ifndef KYSDK_BASE_CONFIGURE_TYPE_H__
#define KYSDK_BASE_CONFIGURE_TYPE_H__

#include <stddef.h>

typedef union KconfigureValue
{
    int asInt;
    long long asLLong;
    double asDouble;
    char *asString;
}KconfigureValue;

typedef enum {
    KconfigureInt,
    KconfigureLLong,
    KconfigureDouble,
    KconfigureString
}KconfigureValueType;

typedef enum {
    KconfigureNodeRoot,
    KconfigureNodeGroup,
    KconfigureNodeNode
}KconfigureNodeType;

typedef struct KconfigureDataNode{
    char *group;
    char *key;
    size_t children_nums;
    size_t max_children_nums;
    char* value;
    // KconfigureValueType valType;
    KconfigureNodeType nodeType;
    struct KconfigureDataNode **children;
}KconfigureDataNode;

#endif