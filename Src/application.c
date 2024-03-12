#include "application.h"

/*****************初始化层*******************/
// uint8_t high_pressure_pwm = HIGH_PRESSURE_LEVEL1_PWM_DUTY_CYCLE;
uint8_t pressureFull = 0;
uint16_t last_bat_value = BAT_MAX_VOL;
uint16_t BAT_DischargeVol(void);
state_machine_t state_machine =
    {
        .board_state = BOARD_LOW_PRESSURE,
        .pressure_value = 0.0f,
        .idle_state_start_time = 0,
        .change_interval = 0,
        .board_fiting_flag = 0,
        .pressure_gear = NOT_GEAR,
};

// 按键回调函数
uint8_t key_unit_read(void);
void key_unit_ShortPress(void);

// LED回调函数
uint8_t LED_KAP_Handle(LED_Config_t *led_config);
uint8_t LED_PSI_Handle(LED_Config_t *led_config);

void set_led_multi(uint16_t led, uint8_t state);
/**
 * Key init function
 * @param none
 * @return none
 */
void user_key_Init(void)
{
    // 初始化KEY GPIO
    KEY_UNIT_GPIO_CLK_ENABLE();

    LL_GPIO_InitTypeDef GPIO_InitStruct[KEY_NUM];

    keyInit_t keyGpioInit[KEY_NUM] =
        {
            [KEY_POWER] = {
                .keyPort = KEY_UNIT_GPIO_Port,
                .keyPin = KEY_UNIT_Pin,
                .keyMode = LL_GPIO_MODE_INPUT,
                .keyPull = LL_GPIO_PULL_UP},
        };

    for (size_t i = 0; i < KEY_NUM; i++)
    {
        // gpio init
        GPIO_InitStruct[i].Pin = keyGpioInit[i].keyPin;
        GPIO_InitStruct[i].Mode = keyGpioInit[i].keyMode;
        GPIO_InitStruct[i].Pull = keyGpioInit[i].keyPull;
        LL_GPIO_Init(keyGpioInit[i].keyPort, &GPIO_InitStruct[i]);
    }

    // 初始化KEY EVENT
    keyCategory_t keys[KEY_NUM] = {
        [KEY_POWER] = {
            // KEY_POWER State Machine Init
            .fsm.keyShield = ENABLE,
            .fsm.keyDownLevel = Bit_RESET,
            .fsm.keyEvent_Init = NULL_Event,
            .fsm.keyLongTime = 2000,
            .fsm.keyLastTime = 5000,
            .fsm.keyReadValue = key_unit_read,
            // KEY_POWER Callback function init
            .func.ShortPressCb = key_unit_ShortPress,
        },
    };

    keyParaInit(keys);
}

// led初始化
void user_led_init(void)
{
    USER_LED1_GPIO_CLK_ENABLE();
    USER_LED2_GPIO_CLK_ENABLE();

    LL_GPIO_SetPinMode(USER_LED1_GPIO_PORT, USER_LED1_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(USER_LED2_GPIO_PORT, USER_LED2_PIN, LL_GPIO_MODE_OUTPUT);

    LED_SetStateInit(set_led_multi); // 设置LED灯状态的回调函数
    SET_TimeGetInit(LL_GetTick);     // 获取时间戳的回调函数

    LED_EventTableItem_t led_event_category[EVENT_MAX] = {
        [EVENT_KPA] = {
            // 开机默认为KPA
            .event = EVENT_KPA,
            .priority = PRIORITY_1,
            .config.ledMask = LED_CHANNEL(USER_LED2),
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_LIGHT,
            .config.runNum = 0, // 次数为0次数无限大，一直执行
            .ledEventHandler = LED_KAP_Handle,
        },
        [EVENT_PSI] = {
            // 按键按下时切换为PSI
            .event = EVENT_PSI,
            .priority = PRIORITY_1,
            .config.ledMask = LED_CHANNEL(USER_LED1),
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_LIGHT,
            .config.runNum = 0, // 次数为0次数无限大，一直执行
            .ledEventHandler = LED_PSI_Handle,
        },
    };
    LED_EventTableInit(led_event_category);
}
/*****************************************************/

/****************************应用层*********************************/
////////////////////////////////////////////////////////
/********************按键读取回调开始*****************/
uint8_t key_unit_read(void)
{
    if (LL_GPIO_IsInputPinSet(KEY_UNIT_GPIO_Port, KEY_UNIT_Pin))
    {
        return 1;
    }
    return 0;
}
/******************按键读取回调结束*********************/

/********************按键应用事件开始*****************/
// 电源单击模式按键回调函数
void key_unit_ShortPress(void)
{
    static uint8_t key_state;
    if (!key_state)
    {
        state_machine.pressure_unit = UNIT_KAP;
        LED_EventAdd(EVENT_KPA);
        key_state = 1;
    }
    else
    {
        state_machine.pressure_unit = UNIT_PSI;
        LED_EventAdd(EVENT_PSI);
        key_state = 0;
    }
}
/******************按键应用事件结束*********************/
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/********************LED应用事件开始**********************/

// KAP单位显示事件
uint8_t LED_KAP_Handle(LED_Config_t *led_config)
{
    LED_Update(led_config);
    // 返回0会继续执行下个优先级任务，返回1则不运行其他低优先级任务
    return 0;
}
// PSI单位显示事件
uint8_t LED_PSI_Handle(LED_Config_t *led_config)
{
    LED_Update(led_config);
    // 返回0会继续执行下个优先级任务，返回1则不运行其他低优先级任务
    return 0;
}

/*******************LED应用事件结束*********************/
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/***********************用户应用层开始**********************/

/************LED状态设置*************/
void set_led(GPIO_TypeDef *gpio_port, uint32_t pin, uint8_t state)
{
    if (state)
    {
        LL_GPIO_SetOutputPin(gpio_port, pin);
    }
    else
    {
        LL_GPIO_ResetOutputPin(gpio_port, pin);
    }
}

void set_led_state(uint16_t led_site, uint8_t state)
{
    switch (led_site)
    {
    case USER_LED1:
        set_led(USER_LED1_GPIO_PORT, USER_LED1_PIN, state);
        break;
    case USER_LED2:
        set_led(USER_LED2_GPIO_PORT, USER_LED2_PIN, state);
        break;
    default:
        break;
    }
}
// LED状态设置回调函数
void set_led_multi(uint16_t led, uint8_t state)
{
    for (size_t led_site = USER_LED1; led_site < USER_LED_MAX; led_site++)
    {
        if (led & LED_CHANNEL(led_site))
        {
            set_led_state(led_site, state);
        }
    }
}
/***********************************/
// 输出指定频率脉冲
void Output_frequency_handle(uint16_t frequency)
{
    static uint32_t tick = 0;
    if (LL_GetTickDiff(tick) >= frequency)
    {
        tick = LL_GetTick();
        appPrintf(LOG_NOTIC, "tick:%d \r\n",tick);
        if (LL_GPIO_IsOutputPinSet(BOARD_STATE_GPIO_PORT, BOARD_STATE_PIN)) // 检测到高电平
        {
            LL_GPIO_ResetOutputPin(BOARD_STATE_GPIO_PORT, BOARD_STATE_PIN); // 拉低
        }
        else // 检测到低电平
        {
            LL_GPIO_SetOutputPin(BOARD_STATE_GPIO_PORT, BOARD_STATE_PIN); // 拉高
        }
    }
}

// 电机保护处理函数
void pump_protect_handle(void)
{
    static uint32_t high_tick = 0;
    if (LL_GPIO_IsInputPinSet(PUMP_FG_GPIO_PORT, PUMP_FG_PIN)) // FG输出高电平，则电机堵转
    {
        if (state_machine.board_state == BOARD_LOW_PRESSURE)
        {
            if(LL_GetTickDiff(high_tick) >= 1000)     
            {
                // 欠压输出高电平，电池相当于没电，进入缺电模式
                state_machine.board_state = BOARD_IDLE; 
                motor_state(MOTOR_OFF, MOTOR_START_TIME, MOTOR_END_TIME);
                Output_frequency_handle(UNDERVOL_STATE_FREQUENCY);
                appPrintf(LOG_NOTIC, "pump protect!!!!! \r\n");
            }
        }else
        {
            high_tick = LL_GetTick();
        }    
    }
    else
    {
        high_tick = LL_GetTick();
    }
}

// 获取气压传感器的值
void read_pressureData_handle(void)
{
    read_pressureData(&state_machine.pressure_value);
    pressurePrintf(LOG_DEBUG, "pressure_value:%f Pa\r\n", state_machine.pressure_value);

    if (LL_GetTickDiff(state_machine.change_interval) >= HIGH_LOW_PRESSURE_CHANGE_INTERVAL)
    {
        if (state_machine.pressure_value >= HIGH_PRESSURE_THRESHOLD)
        {
            motor_state(MOTOR_OFF, MOTOR_START_TIME, MOTOR_END_TIME);
            state_machine.board_state = BOARD_HIGH_PRESSURE;
        }
        else if (state_machine.pressure_value <= LOW_PRESSURE_THRESHOLD)
        {
            pump_state(MOTOR_OFF, PUMP_START_TIME, PUMP_END_TIME);
            state_machine.board_state = BOARD_LOW_PRESSURE;
        }
        state_machine.change_interval = LL_GetTick();
    }
}


// 数码管显示处理函数
void seg_show_handle(void)
{
    static uint32_t last_tick = 0;
    float psi_value = 0.0f;
    if (LL_GetTickDiff(last_tick) >= 1000)
    {
        if (state_machine.pressure_unit == UNIT_KAP)
        { // state_machine.pressure_value为Pa，需显示Kpa
            seg_show_pressure(state_machine.pressure_value / 1000);
        }
        else
        {
            if (state_machine.pressure_value >= 0)
            { // 1PSI = 6.894757Kpa = 6894.757Pa
                psi_value = state_machine.pressure_value / 6894.757;
            }
            else
            {
                psi_value = 0;
            }
            seg_show_pressure(psi_value);
        }
        last_tick = LL_GetTick();
    }
}

/***********************用户应用层结束*****************/
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/*******************状态机应用层开始********************/

void app_state_machine_handle(void)
{
    switch (state_machine.board_state)
    {
    case BOARD_IDLE:
        break;
    case BOARD_LOW_PRESSURE:
        Output_frequency_handle(MOTOR_STATE_FREQUENCY);
        motor_state(LOW_PRESSURE_LEVEL_PWM_DUTY_CYCLE, MOTOR_START_TIME, MOTOR_END_TIME); // 开启电机
        break;
    case BOARD_HIGH_PRESSURE:
        if (pressureFull == 1)
        {
            Output_frequency_handle(DONE_STATE_FREQUENCY);
            motor_state(MOTOR_OFF, MOTOR_START_TIME, MOTOR_END_TIME);
            pump_state(MOTOR_OFF, PUMP_START_TIME, PUMP_END_TIME);
        }else
        {
            if (state_machine.pressure_value >= HIGH_PRESSURE_LEVEL_STOP_PRESSURE)
            {
                pressureFull = 1;
            }
            else
            {
                pump_state(HIGH_PRESSURE_LEVEL_PWM_DUTY_CYCLE, PUMP_START_TIME, PUMP_END_TIME); // 开启气泵
                Output_frequency_handle(PUMP_STATE_FREQUENCY);
            }
        }
        
        
        break;
    }
}

/***************状态机应用层结束************************/
////////////////////////////////////////////////////////

/*******************************************************************************/

void version_printf(void)
{
    appPrintf(LOG_NOTIC, "**************Versioning*****************\r\n");
    appPrintf(LOG_NOTIC, "Developer:WUCHUYUAN\r\n");
    appPrintf(LOG_NOTIC, "Download Date:%s\r\n", __DATE__ " at " __TIME__);
    appPrintf(LOG_NOTIC, "Version:%s\r\n", VERSION);
    appPrintf(LOG_NOTIC, "Commit Messages:%s\r\n", COMMIT_HASH);
    appPrintf(LOG_NOTIC, "*****************************************\r\n");
}

// 气泵FG反馈脚和板机状态脚初始化
void pump_pwr_gpio_init(void)
{
    PUMP_FG_GPIO_CLK_ENABLE();
    BOARD_STATE_GPIO_CLK_ENABLE();

    LL_GPIO_SetPinMode(PUMP_FG_GPIO_PORT, PUMP_FG_PIN, LL_GPIO_MODE_INPUT); // 反馈脚初始化
    LL_GPIO_SetPinPull(PUMP_FG_GPIO_PORT, PUMP_FG_PIN, LL_GPIO_PULL_DOWN);  // 配置为下拉

    LL_GPIO_SetPinMode(BOARD_STATE_GPIO_PORT, BOARD_STATE_PIN, LL_GPIO_MODE_OUTPUT); // 板机状态脚初始化
}

void app_Init(void)
{
    Tim14_Init();
    Tim1_Pwm_Init();
    pump_pwr_gpio_init();
    adc_Init();
    tm1652_Init();
    // BSP_USART_Config(); // 将串口配置成日志口
    appPrintf(LOG_NOTIC, "Start running\r\n");
    version_printf();
    user_key_Init();
    user_led_init();
    gzp6859d_init();
    iwdg_Init(); // 看门狗初始化
    appPrintf(LOG_NOTIC, "iwdg 1S Init\r\n");
    LED_EventAdd(EVENT_KPA);
}

void app_lication(void)
{
    while (1)
    {
        adc_gather_handle();        // ADC采集
        keyHandle();                // 按键处理函数
        LED_EventHandle();          // LED处理函数
        read_pressureData_handle(); // 获取气压传感器
        seg_show_handle();          // 数码管显示处理函数
        app_state_machine_handle(); // 状态机处理函数
        pump_protect_handle();      // 电机FG反馈处理函数
        motor_pump_handle();        // 电机处理函数
        iwdg_FeedDog();             // 看门狗喂狗
    }
}
