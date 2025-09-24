# CPU monitor without thread



## 1. What's this​!​ ​​

`cpu_monitor_no_thread` 是一个针对裸机应用编写的 cpu loading 监控方案。使用 FreeRTOS、RT-Thread 这类操作系统时，往往 RTOS 内都自带了 cpu monitor 的组件，但是针对裸机应用场景，却缺乏此类组件，因此在进行逻辑开发时，对于 CPU loading 统计便成了问题，因此开发了 `cpu_monitor_no_thread`  此组件。

使用 `cpu_monitor_no_thread`  时，建议对应的裸机开发也需遵循一定的任务思想进行开发，这样统计 cpu loading 才具有一定的意义，如果将事情全部放在`while(1)` 循环内，不关心执行频率，cpu 能跑多少次就跑多少次，对于这类应用场景，相信你也无须考虑 cpu loading 的问题，因为你的 cpu 一直都在忙着干活，一直都是100%



## 2. Other

为避免使用此组件的开发者未能理解裸机任务开发思想，特作此补充说明，若已理解可直接跳过

任务开发思想：即将 cpu 需要做的事情拆分为多个任务，如 A、B、C，并根据需求确定相关任务的执行频率，如：

+ 任务A：10ms 执行一次
+ 任务B：20ms 执行一次
+ 任务C：30ms 执行一次

由系统定时器（Cortex M处理器通常使用 system tick）产生基础时间节拍 tick，如 1ms 产生一个 tick

下方展示一个简单的由 system tick 产生时间节拍的处理方式：

```c
uint32_t clock_array[5]={0};		/* 用于拆分任务处理 */
uint32_t system_tick = 0;

void SysTick_Handler(void)
{
	uint8_t i = 0;
	system_tick ++;
	for(i = 0; i < 5; i++)
	{
		if(clock_array[i] != 0)
			clock_array[i] --;
	}
}
```



有了 system tick 提供的时间片之后，之后在主循环内即可设计对应任务处理了，伪代码如下：
```c
int main(void)
{
    /* xxxx init */
    while (1) {
        if (clock_array[0] == 0) {
            clock_array[0] = 10;
            A();	/* 执行A任务*/
        }
        if (clock_array[1] == 0) {
            clock_array[1] = 20;
            B();	/* 执行A任务*/
        }
        if (clock_array[2] == 0) {
            clock_array[2] = 30;
            C();	/* 执行A任务*/
        }
    }
}
```



## 3. Usage
### 3.1 Steps

####  step1：

添加 `cpu_monitor_no_thread` 组件至对应工程

#### step2：

根据对应平台，适配系统时间戳接口，`uint32_t cpu_monitor_get_systick(void)`，接口声明位于`cpu_monitor_interface.h` 文件内

#### step3：

系统时钟初始化之后，且其他外设/中断未启动前，调用 `cpu_monitor_init();` ，务必注意在系统时钟初始化之后再调用，否则初始化将失败；

此函数运行固定时间用来统计全cpu计算的基（分母），因此要求在其他外设/中断未启用前调用的，这样更加精准

#### step4：

主循环内调用 `cpu_monitor_run_hook()` 统计cpu空闲时占比

#### step5：

系统时钟中断内调用 `cpu_monitor_calculate()` 定周期计算 cpu loading

可通过修改 `#define CPU_MONITOR_PERIOD_TICK     100` ，调整 cpu loading 计算周期

#### step6：

可调用 `cpu_monitor_get_loading()` 获取当前 cpu 使用率



### 3.2 Example

系统滴答处理：

```
uint32_t clock_array[5]={0};		/* 用于拆分任务处理 */
uint32_t system_tick = 0;

void SysTick_Handler(void)
{
	uint8_t i = 0;
	system_tick ++;
	for(i = 0; i < 5; i++)
	{
		if(clock_array[i] != 0)
			clock_array[i] --;
	}
	cpu_monitor_calculate();
}
```

主函数处理：

```c
#include "cpu_monitor.h"
#include "cpu_monitor_interface.h"

uint32_t cpu_monitor_get_systick(void)
{
    return get_systick();
}

int main(void)
{
	uint8_t major = 0, minor = 0;
	Systick_Init();
	cpu_monitor_init();
    
    /* xxxx init */
    while (1) {
        cpu_monitor_run_hook();
        
        if (clock_array[0] == 0) {
            clock_array[0] = 10;
            A();	/* 执行A任务*/
        }
        if (clock_array[1] == 0) {
            clock_array[1] = 20;
            B();	/* 执行A任务*/
        }
        if (clock_array[2] == 0) {
            clock_array[2] = 30;
            C();	/* 执行A任务*/
        }
        if (clock_array[3] == 0) {
            clock_array[3] = 1000;
            cpu_monitor_get_loading(&major, &minor);
            printf("cpu loading:%d.%02d\r\n", major, minor);
        }
    }
}
```



## 4. Author

+ jaffer <jaffer.work@foxmail.com>



## 5. License

MIT.



