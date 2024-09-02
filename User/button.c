/************************************************************
  * @brief   按键驱动
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    button.c
  ***********************************************************/
#include "button.h"

/*******************************************************************
 *                          变量声明
 *******************************************************************/

static struct button* Head_Button = NULL;


/*******************************************************************
 *                         函数声明
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n);
static void Print_Btn_Info(Button_t* btn);
static void Add_Button(Button_t* btn);


/************************************************************
  * @brief   按键创建
    * @param   name : 按键名称
    * @param   btn : 按键结构体
  * @param   read_btn_level : 按键电平读取函数，需要用户自己实现返回uint8_t类型的电平
  * @param   btn_trigger_level : 按键触发电平
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Create(const char *name,
                  Button_t *btn, 
                  uint8_t(*read_btn_level)(void),
                  uint8_t btn_trigger_level)
{
  if( btn == NULL)
  {
    printf("struct button is null!");
  }

  memset(btn, 0, sizeof(struct button));  //清除结构体信息，建议用户在之前清除

  StrnCopy(btn->Name, name, BTN_NAME_MAX); /* 创建按键名称 */

  btn->Button_State = NONE_TRIGGER;           //按键状态
  btn->Button_Last_State = NONE_TRIGGER;      //按键上一次状态
  btn->Button_Trigger_Event = NONE_TRIGGER;   //按键触发事件
  btn->Read_Button_Level = read_btn_level;    //按键读电平函数
  btn->Button_Trigger_Level = btn_trigger_level;  //按键触发电平
  btn->Button_Last_Level = btn->Read_Button_Level();              //按键当前电平
  btn->Debounce_Time = 0;

  printf("button create success!");

  Add_Button(btn);          //创建的时候添加到单链表中
}

/************************************************************
  * @brief   按键触发事件与回调函数映射链接起来
    * @param   btn : 按键结构体
    * @param   btn_event : 按键触发事件
  * @param   btn_callback : 按键触发之后的回调处理函数。需要用户实现
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
  if( btn == NULL)
  {
    printf("struct button is null!");
  }
  
  if(BUTTON_ALL_RIGGER == btn_event)
  {
    for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
      btn->CallBack_Function[i] = btn_callback; //按键事件触发的回调函数，用于处理按键事件
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //按键事件触发的回调函数，用于处理按键事件
  }
}

/************************************************************
  * @brief   删除一个已经创建的按键
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Delete(Button_t *btn)
{
  struct button** curr;
  for(curr = &Head_Button; *curr;) 
  {
    struct button* entry = *curr;
    if (entry == btn) 
    {
      *curr = entry->Next;
    } 
    else
    {
      curr = &entry->Next;
    }
  }
}

/************************************************************
  * @brief   获取按键触发的事件
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Get_Button_EventInfo(Button_t *btn)
{
  //按键事件触发的回调函数，用于处理按键事件
  for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
  {
    if(btn->CallBack_Function[i] != 0)
    {
      printf("Button_Event:%d",i);
    }
  }
}

uint8_t Get_Button_Event(Button_t *btn)
{
  return (uint8_t)(btn->Button_Trigger_Event);
}

/************************************************************
  * @brief   获取按键触发的事件
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
uint8_t Get_Button_State(Button_t *btn)
{
  return (uint8_t)(btn->Button_State);
}

/************************************************************
  * @brief   按键周期处理函数
  * @param   btn:处理的按键
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    必须以一定周期调用此函数，建议周期为20~50ms
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
    uint8_t current_level = (uint8_t)btn->Read_Button_Level();//获取当前按键电平
    if((current_level != btn->Button_Last_Level)&&(++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME)) //按键电平发生变化，消抖
    {
        btn->Button_Last_Level = current_level; //更新当前按键电平
        btn->Debounce_Time = 0;                 //确定了是按下

        if(current_level == btn->Button_Trigger_Level)
        {
            btn->Button_State = BUTTON_DOWN;
            TRIGGER_CB(BUTTON_DOWN);    // 触发释放
        }
        else if(btn->Button_State == BUTTON_DOWN)
        {
            btn->Button_State = BUTTON_UP;
            TRIGGER_CB(BUTTON_UP);    // 触发释放
        }
    }

    if(btn->Button_State == NONE_TRIGGER && current_level == btn->Button_Trigger_Level &&(++(btn->Timer_Count) >= BUTTON_DEBOUNCE_TIME))
    {
        btn->Timer_Count = 0;
        btn->Button_State = BUTTON_DOWN;
        TRIGGER_CB(BUTTON_DOWN);    // 触发释放
    }
}

/************************************************************
  * @brief   遍历的方式扫描按键，不会丢失每个按键
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    此函数要周期调用，建议20-50ms调用一次
  ***********************************************************/
void button_handle(void)
{
    struct button* pass_btn;
    for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
    {
      Button_Cycle_Process(pass_btn);
    }
}

/************************************************************
  * @brief   遍历按键
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Search_Button(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
    printf("button node have %s",pass_btn->Name);
  }
}

/************************************************************
  * @brief   处理所有按键回调函数
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    暂不实现
  ***********************************************************/
void Button_Process_CallBack(void *btn)
{
  uint8_t btn_event = Get_Button_Event(btn);

  switch(btn_event)
  {
    case BUTTON_DOWN:
    {
      printf("添加你的按下触发的处理逻辑");
      break;
    }

    case BUTTON_UP:
    {
      printf("添加你的释放触发的处理逻辑");
      break;
    }

    case BUTTON_DOUBLE:
    {
      printf("添加你的双击触发的处理逻辑");
      break;
    }

    case BUTTON_LONG:
    {
      printf("添加你的长按触发的处理逻辑");
      break;
    }

    case BUTTON_LONG_FREE:
    {
      printf("添加你的长按释放触发的处理逻辑");
      break;
    }

    case BUTTON_CONTINUOS:
    {
      printf("添加你的连续触发的处理逻辑");
      break;
    }

    case BUTTON_CONTINUOS_FREE:
    {
      printf("添加你的连续触发释放的处理逻辑");
      break;
    }

  }
}


/**************************** 以下是内部调用函数 ********************/

/************************************************************
  * @brief   拷贝指定长度字符串
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n)
{
  if (n != 0)
  {
    char *d = dst;
    const char *s = src;
    do
    {
        if ((*d++ = *s++) == 0)
        {
            while (--n != 0)
                *d++ = 0;
            break;
        }
    } while (--n != 0);
  }
  return (dst);
}

/************************************************************
  * @brief   打印按键相关信息
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static void Print_Btn_Info(Button_t* btn)
{

  printf("button struct information:\n\
              btn->Name:%s \n\
              btn->Button_State:%d \n\
              btn->Button_Trigger_Event:%d \n\
              btn->Button_Trigger_Level:%d \n\
              btn->Button_Last_Level:%d \n\
              ",
              btn->Name,
              btn->Button_State,
              btn->Button_Trigger_Event,
              btn->Button_Trigger_Level,
              btn->Button_Last_Level);
  Search_Button();
}
/************************************************************
  * @brief   使用单链表将按键连接起来
    * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static void Add_Button(Button_t* btn)
{
  btn->Next = Head_Button;
  Head_Button = btn;
}
