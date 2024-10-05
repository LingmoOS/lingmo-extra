#ifndef KYSDK_BASE_UTILS_MARCOS_H__
#define KYSDK_BASE_UTILS_MARCOS_H__

/*
    GNU Attribute
*/
#define	DEPRECATED	__attribute__((deprecated))
#define	CONSTRUCTOR	__attribute__((constructor))
#define	DESTRUCTOR	__attribute__((destructor))
#define	FORMAT(type, fmtindex, argindex)	__attribute__((format(type, fmtindex, argindex)))
#define	WARNUNSEDRET	__attribute__((warn_unused_result))
#define	WEAK		__attribute__((weak))
#define ALWAYSINLINE    __attribute__((always_inline))
#define	CHECK_FMT(x,y)	__attribute__((format(printf,x,y)))
#define	NOINLINE	__attribute__((noinline))
#define	NOTNULL(...)	__attribute__((nonnull(__VA_ARGS__)))
#define	MUSTCHECKRESULT	__attribute__((warn_unused_result))

/*
    Customize
*/
#define ASSERT_NOT_NULL(x, ret)  if (!x) {return ret;}
#define SAFE_FREE(x) if (x) {free(x); x = NULL;}

#endif