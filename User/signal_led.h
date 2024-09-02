#ifndef __SIGNAL_LED_H__
#define __SIGNAL_LED_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus 
extern "C" { 
#endif

#define LED_TICK_TIME 10    //心跳函数调用的时间间隔（单位:ms）
#define LOOP_PERMANENT 0XFF //永久循环

#define HANDLE_EXIST 1

#define LED_OFF 0 //灯灭状态
#define LED_ON 1  //灯亮状态


typedef struct _led
{
    uint8_t active_state; //信号灯亮时的引脚状态 (PIN_HIGH/PIN_LOW)
    uint8_t state;        //信号灯当前的状态
    uint8_t mode_pointer; //用于指明当前参照闪烁数组中的第几个成员
    uint16_t tick_cnt;
    uint16_t mode_cnt;    //一个周期内的闪烁次数
    uint16_t handle_cnt;  //handle函数计数
    uint16_t loop;        //周期
    uint16_t loop_temp;   //存储周期的临时变量，当重启led时会被重置
    uint32_t blink_point; //闪烁节点
    uint32_t blink_arr[64];
    char blink_mode[64];
    void (*switch_on)(void *args);
    void (*switch_off)(void *args);
    void (*led_blink_over_callback)(struct _led *led_handle); //led闪烁结束回调函数
    void *args;
    struct _led *next;
} led_t;

typedef void (*led_blink_over_callback)(led_t *led_handler);

void    led_ticks(void);
void    led_create(led_t *led_ptr,void (*switch_on)(void *args), void (*switch_off)(void *args), void *args);
void    led_set_blink_over_callback(led_t *led_handler, led_blink_over_callback callback);
void    led_set_mode(led_t *handle, uint8_t loop, char *blinkMode);
uint8_t led_start(led_t *led_handle);
void    led_stop(led_t *led_handle);
void    led_toggle(led_t *led_handle);

#ifdef __cplusplus 
} 
#endif

#endif /* __SIGNAL_LED_H__ */
