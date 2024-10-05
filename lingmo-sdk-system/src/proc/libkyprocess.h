#ifndef LIBKYPROCESS_H
#define LIBKYPROCESS_H

#endif

/**
 * @file libkyprocess.h
 * @author shaozhimin (shaozhimin@lingmoos.cn)
 * @brief 某一进程的瞬时详细信息
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * @defgroup 某一进程的瞬时详细信息
 * @{
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取某一进程的名称
 * 
 * @param proc_num 进程号
 * @return char* 进程的名称
 */
extern char* kdk_get_process_name(int proc_num);

/**
 * @brief 获取某一进程的id
 * 
 * @param proc_name 进程的名称
 * @return int** 进程号（id）
 */
extern int kdk_get_process_id(char *proc_name);

/**
 * @brief 获取某一进程的CPU使用率
 * 
 * @param proc_num 进程号
 * @return float CPU使用率
 */
extern float kdk_get_process_cpu_usage_percent(int proc_num);

/**
 * @brief 获取某一进程的内存占用率
 * 
 * @param proc_num 进程号
 * @return float 内存占用率
 */
extern float kdk_get_process_mem_usage_percent(int proc_num);

/**
 * @brief 获取某一进程的进程状态
 * 
 * @param proc_num 进程号
 * @return char* 进程状态
 */
extern char* kdk_get_process_status(int proc_num);

/**
 * @brief 获取某一进程的端口号占用
 * 
 * @param proc_num 进程号
 * @return char* 使用的端口号
 */
extern int kdk_get_process_port(int proc_num);

/**
 * @brief 获取某一进程的启动时间
 * 
 * @param proc_num 进程号
 * @return char* 启动时间
 */
extern char* kdk_get_process_start_time(int proc_num);

/**
 * @brief 获取某一进程的运行时间
 * 
 * @param proc_num 进程号
 * @return char* 运行时间
 */
extern char* kdk_get_process_running_time(int proc_num);

/**
 * @brief 获取某一进程的cpu时间
 * 
 * @param proc_num 进程号
 * @return char* cpu时间
 */
extern char* kdk_get_process_cpu_time(int proc_num);

/**
 * @brief 获取某一进程的Command
 * 
 * @param proc_num 进程号
 * @return char* Command
 */
extern char* kdk_get_process_command(int proc_num);

/**
 * @brief 获取某一进程的属主
 * 
 * @param proc_num 进程号
 * @return char* 属主
 */
extern char* kdk_get_process_user(int proc_num);


/**
 * @brief 获取某一进程的信息
 * 
 * @param proc_name 进程名
 * @return float 内存占用率
 */
extern char** kdk_procname_get_process_infomation(char *proc_name);

/**
 * @brief 获取进程所有信息
 *
 * @return char** 进程所有信息
 */
extern char** kdk_get_process_all_information();

/**
 * @brief 用于回收字符串列表
 * 
 * @param ptr 字符串列表
 */
extern inline void kdk_proc_freeall(char **ptr);

/**
 * @brief 用于回收进程id资源
 *
 * @param list 进程id列表
 */
extern inline void kdk_proc_freeid(int *list);

#ifdef __cplusplus
}

#endif
/**
  * @}
  */
