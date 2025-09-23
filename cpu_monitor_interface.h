#ifndef __CPU_MONITOR_INTERFACE_H__
#define __CPU_MONITOR_INTERFACE_H__

#include <stdint.h>

/**
 * @brief CPU监控获取系统滴答函数接口，需要用户实现，返回值单位为tick
 *
 * @return uint32_t
 */
extern uint32_t cpu_monitor_get_systick(void);

#endif /* __CPU_MONITOR_INTERFACE_H__ */

