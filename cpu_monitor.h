#ifndef __CPU_MONITOR_H__
#define __CPU_MONITOR_H__

#include <stdint.h>


/**
 * @brief CPU监控初始化函数，在main函数中，systick初始化且其他外设未初始化之前调用
 *
 */
void cpu_monitor_init(void);

/**
 * @brief CPU监控运行钩子函数，在while循环中调用
 *
 */
void cpu_monitor_run_hook(void);

/**
 * @brief CPU监控计算函数，由systick中断回调调用
 *
 */
void cpu_monitor_calculate(void);

/**
 * @brief 获取CPU使用率
 *
 * @param major 整数部分
 * @param minor 小数部分
 */
void cpu_monitor_get_loading(uint8_t *major, uint8_t *minor);

#endif /* __CPU_MONITOR_H__ */
