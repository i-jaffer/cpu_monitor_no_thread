#include "cpu_monitor.h"
#include "cpu_monitor_interface.h"

#ifndef CPU_MONITOR_PERIOD_TICK
#define CPU_MONITOR_PERIOD_TICK     100
#endif

#ifndef CPU_MONITOR_MAX_LOOP
#define CPU_MONITOR_MAX_LOOP        100
#endif

typedef struct {
    uint32_t total_count;
    volatile uint32_t total_loop;

    uint32_t run_count;
    volatile uint32_t run_loop;

    uint8_t loading_major;
    uint8_t loading_minor;
} cpu_data_t;

typedef struct {
    uint8_t init_flag;
} cpu_status_t;

typedef struct {
    cpu_data_t data;
    cpu_status_t status;
} cpu_monitor_t;

static cpu_monitor_t cpu_monitor = {0};

/**
 * @brief CPU监控初始化函数，在main函数中，systick初始化且其他外设未初始化之前调用
 *
 */
void cpu_monitor_init(void)
{
    uint32_t tick = cpu_monitor_get_systick();
    while (cpu_monitor_get_systick() - tick < CPU_MONITOR_PERIOD_TICK) {
        cpu_monitor.data.total_loop++;
        if (cpu_monitor.data.total_loop >= CPU_MONITOR_MAX_LOOP) {
            cpu_monitor.data.total_loop = 0;
            cpu_monitor.data.total_count ++;
        }
    }
    cpu_monitor.status.init_flag = 1;
}

/**
 * @brief CPU监控运行钩子函数，在while循环中调用
 *
 */
void cpu_monitor_run_hook(void)
{
    cpu_monitor.data.run_loop++;
    if (cpu_monitor.data.run_loop >= CPU_MONITOR_MAX_LOOP) {
        cpu_monitor.data.run_loop = 0;
        cpu_monitor.data.run_count ++;
    }
}

/**
 * @brief CPU监控计算函数，由systick中断回调调用
 *
 */
void cpu_monitor_calculate(void)
{
    static uint32_t last_tick = 0;
    volatile uint32_t count = 0;
    volatile uint32_t tick = cpu_monitor_get_systick();

    if (cpu_monitor.status.init_flag == 0) {
        last_tick = tick;
        return;
    }
    if (tick - last_tick < CPU_MONITOR_PERIOD_TICK) {
        return;
    }
    last_tick = tick;

    if (cpu_monitor.data.run_count < cpu_monitor.data.total_count) {
        count = cpu_monitor.data.total_count - cpu_monitor.data.run_count;
        cpu_monitor.data.loading_major = (count * 100) / cpu_monitor.data.total_count;
        cpu_monitor.data.loading_minor = (((count * 100) % cpu_monitor.data.total_count) * 100) / cpu_monitor.data.total_count;
    } else {
        cpu_monitor.data.total_count = cpu_monitor.data.run_count;
        cpu_monitor.data.loading_major = 0;
        cpu_monitor.data.loading_minor = 0;
    }

    cpu_monitor.data.run_count = 0;
}

/**
 * @brief 获取CPU使用率
 *
 * @param major 整数部分
 * @param minor 小数部分
 */
void cpu_monitor_get_loading(uint8_t *major, uint8_t *minor)
{
    if (major)
        *major = cpu_monitor.data.loading_major;
    if (minor)
        *minor = cpu_monitor.data.loading_minor;
}
