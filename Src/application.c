#include "application.h"

/*****************初始化层*******************/
// uint8_t high_pressure_pwm = HIGH_PRESSURE_LEVEL1_PWM_DUTY_CYCLE;
uint32_t high_time = 0, low_time = 0; // 记录反馈脚高电平和低电平时间
uint16_t last_bat_value;
uint16_t BAT_DischargeVol(void);
uint8_t charge_bat_vol(void);
state_machine_t state_machine =
    {
        .board_state = BOARD_IDLE,
        .pressure_value = 0.0f,
        .idle_state_start_time = 0,
        .change_interval = 0,
        .board_fiting_flag = 0,
        .pressure_gear = NOT_GEAR,
};

// 按键回调函数
uint8_t key_power_read(void);
uint8_t key_charge_read(void);
uint8_t key_chargeFull_read(void);
void key_power_ShortPress(void);
void key_charge_InsertPress(void);
void key_charge_ExtractPress(void);
void key_charge_FullPress(void);
void key_charge_NotFullPress(void);

// LED回调函数
uint8_t power_test_handle(LED_Config_t *led_config);
uint8_t not_charging_led_handle(LED_Config_t *led_config);
uint8_t charging_led_handle(LED_Config_t *led_config);
uint8_t bat_full_led_handle(LED_Config_t *led_config);
uint8_t bat_low_led_handle(LED_Config_t *led_config);
uint8_t current_high_handle(LED_Config_t *led_config);

void set_led_multi(uint16_t led, uint8_t state);
/**
 * Key init function
 * @param none
 * @return none
 */
void user_key_Init(void)
{
    // 初始化KEY GPIO
    CHARGE_GPIO_CLK_ENABLE();
    KEY_POWER_GPIO_CLK_ENABLE();
    FULL_CHARGE_GPIO_CLK_ENABLE();

    LL_GPIO_InitTypeDef GPIO_InitStruct[KEY_NUM];

    keyInit_t keyGpioInit[KEY_NUM] =
        {
            [KEY_POWER] = {
                .keyPort = KEY_POWER_GPIO_Port,
                .keyPin = KEY_POWER_Pin,
                .keyMode = LL_GPIO_MODE_INPUT,
                .keyPull = LL_GPIO_PULL_UP},

            [KEY_CHARGE] = {.keyPort = CHARGE_GPIO_PORT, .keyPin = CHARGE_PIN, .keyMode = LL_GPIO_MODE_INPUT, .keyPull = LL_GPIO_PULL_UP},

            [KEY_CHARGE_FULL] = {.keyPort = FULL_CHARGE_GPIO_PORT, .keyPin = FULL_CHARGE_PIN, .keyMode = LL_GPIO_MODE_INPUT, .keyPull = LL_GPIO_PULL_DOWN},
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
            .fsm.eventType = NULL_Event,
            .fsm.keyLongTime = 2000,
            .fsm.keyLastTime = 5000,
            .fsm.keyReadValue = key_power_read,
            // KEY_POWER Callback function init
            .func.ShortPressCb = key_power_ShortPress,
        },
        [KEY_CHARGE] = {
            // KEY_CHARGE State Machine Init
            .fsm.keyShield = ENABLE,
            .fsm.keyDownLevel = Bit_RESET,
            .fsm.eventType = RELEASE_Event,
            .fsm.keyLongTime = 500,
            .fsm.keyLastTime = 5000,
            .fsm.keyReadValue = key_charge_read,
            // KEY_CHARGE Callback function init
            .func.downPressCb = key_charge_InsertPress,
            .func.releasePressCb = key_charge_ExtractPress,
        },
        [KEY_CHARGE_FULL] = {
            // KEY_CHARGE_FULL State Machine Init
            .fsm.keyShield = ENABLE,
            .fsm.keyDownLevel = Bit_RESET,
            .fsm.eventType = RELEASE_Event,
            .fsm.keyLongTime = 500,
            .fsm.keyLastTime = 5000,
            .fsm.keyReadValue = key_chargeFull_read,
            // KEY_CHARGE_FULL Callback function init
            .func.downPressCb = key_charge_FullPress,
            .func.releasePressCb = key_charge_NotFullPress,
        },
    };

    keyParaInit(keys);
}

// led初始化
void user_led_init(void)
{
    USER_LED1_GPIO_CLK_ENABLE();
    USER_LED2_GPIO_CLK_ENABLE();
    USER_LED3_GPIO_CLK_ENABLE();
    USER_LED4_GPIO_CLK_ENABLE();
    USER_LED5_GPIO_CLK_ENABLE();

    LL_GPIO_SetPinMode(USER_LED1_GPIO_PORT, USER_LED1_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(USER_LED2_GPIO_PORT, USER_LED2_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(USER_LED3_GPIO_PORT, USER_LED3_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(USER_LED4_GPIO_PORT, USER_LED4_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(USER_LED5_GPIO_PORT, USER_LED5_PIN, LL_GPIO_MODE_OUTPUT);

    LED_SetStateInit(set_led_multi); // 设置LED灯状态的回调函数
    SET_TimeGetInit(LL_GetTick);     // 获取时间戳的回调函数

    // 流水灯模式初始化
    LED_EventTableItem_t led_event_category[EVENT_MAX] = {
        [EVENT_POWER_TEST] = {
            // 开机后LED13常亮
            .event = EVENT_POWER_TEST,
            .priority = PRIORITY_1,
            .config.ledMask = LED_CHANNEL(USER_LED1) | LED_CHANNEL(USER_LED2) | LED_CHANNEL(USER_LED3) | LED_CHANNEL(USER_LED4) | LED_CHANNEL(USER_LED5),
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_BLINKING,
            .config.blinkParams.offTime = 500,
            .config.blinkParams.onTime = 500,
            .config.runNum = 1, // 次数为0次数无限大，一直执行
            .ledEventHandler = power_test_handle,
        },
        [EVENT_NOT_CHARGING] = {
            // 不充电时LED显示电量。
            .event = EVENT_NOT_CHARGING,
            .priority = PRIORITY_4,
            // .config.ledMask = LED_CHANNEL(USER_LED2) | LED_CHANNEL(USER_LED3) | LED_CHANNEL(USER_LED4) | LED_CHANNEL(USER_LED5),
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_LIGHT,
            .config.runNum = 0, // 次数为0次数无限大，一直执行
            .ledEventHandler = not_charging_led_handle,
        },
        [EVENT_CHARGING] = {
            // 充电时LED流水灯工作。
            .event = EVENT_CHARGING,
            .priority = PRIORITY_4,
            // .config.ledMask = LED_CHANNEL(USER_LED2) | LED_CHANNEL(USER_LED3) | LED_CHANNEL(USER_LED4) | LED_CHANNEL(USER_LED5),
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_RUNNING,
            .config.runningParams.cycleTime = 250,
            .config.runningParams.intervalTime = 500,
            .config.runNum = 0, // 次数为0次数无限大，一直执行
            .ledEventHandler = charging_led_handle,
        },
        [EVENT_FULL_BATTERY] = {
            // 充满时LED指示灯变绿常亮。
            .event = EVENT_FULL_BATTERY,
            .priority = PRIORITY_4,
            .config.ledMask = LED_CHANNEL(USER_LED2) | LED_CHANNEL(USER_LED3) | LED_CHANNEL(USER_LED4) | LED_CHANNEL(USER_LED5),
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_LIGHT,
            .ledEventHandler = bat_full_led_handle,
        },
        [EVENT_LOW_BATTERY] = {
            /// 缺电时（电压低于 8.4V±0.1V）故障LED闪烁3次
            .event = EVENT_LOW_BATTERY,
            .priority = PRIORITY_4,
            .config.ledMask = LED_CHANNEL(USER_LED1),
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_BLINKING,
            .config.blinkParams.offTime = 500,
            .config.blinkParams.onTime = 500,
            .config.runNum = 3, // 次数为0次数无限大，一直执行
            .ledEventHandler = bat_low_led_handle,
        },
        [EVENT_CURRENT_HIGH] = {
            /// 电流过大红绿交替闪烁5次
            .event = EVENT_CURRENT_HIGH,
            .priority = PRIORITY_4,
            // 常亮1s为一个周期，没有次数，没有循环，即一直常亮下去
            .config.state = LED_ALTERNATING_BLINK,
            .config.alternatingBlinkParams.Cycle = 1000,
            .config.alternatingBlinkParams.Group[0] = LED_CHANNEL(USER_LED1),
            .config.alternatingBlinkParams.Group[1] = LED_CHANNEL(USER_LED2),
            .config.runNum = 5, // 次数为0次数无限大，一直执行
            .ledEventHandler = current_high_handle,
        },
    };
    LED_EventTableInit(led_event_category);
}

/*****************************************************/

/****************************应用层*********************************/

////////////////////////////////////////////////////////
/********************按键读取回调开始*****************/
uint8_t key_power_read(void)
{
    if (LL_GPIO_IsInputPinSet(KEY_POWER_GPIO_Port, KEY_POWER_Pin))
    {
        return 1;
    }
    return 0;
}
uint8_t key_charge_read(void)
{
    if (LL_GPIO_IsInputPinSet(CHARGE_GPIO_PORT, CHARGE_PIN))
    {
        
        return 1;
    }
    return 0;
}
uint8_t key_chargeFull_read(void)
{
    if (LL_GPIO_IsInputPinSet(FULL_CHARGE_GPIO_PORT, FULL_CHARGE_PIN))
    {
        return 1;
    }
    return 0;
}
/******************按键读取回调结束*********************/

/********************按键应用事件开始*****************/
// 电源单击模式按键回调函数
void key_power_ShortPress(void)
{
    keyPrintf(LOG_NOTIC, "key_power_ShortPress\r\n");
    if (state_machine.bat_low_flag)
    {
        state_machine.board_state = BOARD_IDLE;
        LED_EventAdd(EVENT_LOW_BATTERY); // 添加缺电事件
        return;
    }

    if (state_machine.board_state == BOARD_WORK && state_machine.board_fiting_flag) // 关机，进入休眠模式
    {
        appPrintf(LOG_NOTIC, "BOARD_STOP\r\n");
        state_machine.board_fiting_flag = 0;
        set_led_multi(0xFFFF, LED_OFF);      // 关闭所有LED
        LED_EventDelete(EVENT_NOT_CHARGING); // 删除不充电事件
        state_machine.board_state = BOARD_STOP;
    }
    else // 开机，运行电机
    {
        // if (!state_machine.board_fiting_flag)
        // {
        //     VOL_PowerGather();
        // }
        appPrintf(LOG_NOTIC, "BOARD_WORK\r\n");
        LED_EventAdd(EVENT_NOT_CHARGING); // 添加不充电LED事件
        state_machine.board_fiting_flag = 1;
        state_machine.board_state = BOARD_WORK; // 进入工作状态
    }
}

// 充电插入回调函数
void key_charge_InsertPress(void)
{
    logAssert();
    last_bat_value = BAT_MIN_VOL;
    state_machine.bat_low_flag = 0;
    charge_full_key_enable(ENABLE); // 充电的时候使能充满按钮
    if (state_machine.board_state != BOARD_CHARGE)
    {
        state_machine.board_state = BOARD_CHARGE; // 进入该状态关闭所有电机
        LED_EventAdd(EVENT_CHARGING);             // 添加充电LED事件
        power_key_enable(DISABLE);                // 不使能电源按键
    }
    keyPrintf(LOG_NOTIC, "key_charge_InsertPress\r\n");
}
// 充电拔出回调函数
void key_charge_ExtractPress(void)
{
    if (state_machine.bat_low_flag)
        return;
    last_bat_value = BAT_MAX_VOL;
    state_machine.bat_full.charge_full_valid_flag = 0;
    LED_EventAdd(EVENT_NOT_CHARGING); // 添加不充电LED事件
    charge_full_key_enable(DISABLE);  // 没充电的时候不使能充满按钮
    power_key_enable(ENABLE);         // 使能电源按键
    if (state_machine.board_state == BOARD_CHARGE)
    {
        state_machine.board_state = BOARD_STOP; // 进入休眠状态
    }

    keyPrintf(LOG_NOTIC, "key_charge_ExtractPress\r\n");
}

// 充满电回调函数
void key_charge_FullPress(void)
{
    if (state_machine.board_state == BOARD_CHARGE)
    {
        state_machine.bat_full.charge_full_valid_flag = 1;
        state_machine.bat_full.charge_full_start_time = LL_GetTick();
    }
    keyPrintf(LOG_NOTIC, "key_charge_FullPress\r\n");
}
// 没满电回调函数
void key_charge_NotFullPress(void)
{
    keyPrintf(LOG_NOTIC, "key_charge_NotFullPress\r\n");
}
/******************按键应用事件结束*********************/
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/********************LED应用事件开始**********************/
// 上电测试LED事件
uint8_t power_test_handle(LED_Config_t *led_config)
{
    while (LED_Update(led_config))
        ;
    // 返回0会继续执行下个优先级任务，返回1则不运行其他低优先级任务
    return 0;
}
// 不充电事件LED事件
uint8_t not_charging_led_handle(LED_Config_t *led_config)
{
    uint16_t batteryLevel;
    static uint32_t flow_interval = 0;

    if (LL_GetTickDiff(flow_interval) >= 1000 && state_machine.board_fiting_flag) // 1s采集一次电量
    {
        uint16_t led_channels[] = {USER_LED2, USER_LED3, USER_LED4, USER_LED5};
        uint16_t charge_levels[] = {charg_level_gear1,
                                    charg_level_gear2,
                                    charg_level_gear3,
                                    charg_level_gear4};
        size_t num_levels = sizeof(charge_levels) / sizeof(charge_levels[0]);

        led_config->ledMask = 0;
        flow_interval = LL_GetTick();
        batteryLevel = BAT_DischargeVol();

        if (batteryLevel > 0)
        {
            // 迭代充电等级，根据电量设置LED通道
            for (size_t i = 0; i < num_levels; ++i)
            {
                if (batteryLevel >= charge_levels[i])
                {
                    // 添加对应的LED通道
                    led_config->ledMask |= LED_CHANNEL(led_channels[i]);
                }
                else
                {
                    // 熄灭电量不足的LED灯
                    set_led_multi(LED_CHANNEL(led_channels[i]), LED_OFF);
                }
            }
        }
        else
        { // 缺电
            state_machine.bat_low_flag = 1;
        }
    }
    if (LED_Update(led_config))
    {
        return 1;
    }
    // 返回0会继续执行下个优先级任务，返回1则不运行其他低优先级任务
    return 0;
}
// 充电事件LED事件
uint8_t charging_led_handle(LED_Config_t *led_config)
{
    uint16_t batteryLevel;
    static uint32_t flow_interval = 0;

    if (LL_GetTickDiff(flow_interval) >= 1000) // 1s采集一次电量
    {
        uint16_t led_channels[] = {USER_LED2, USER_LED3, USER_LED4, USER_LED5};
        uint16_t charge_levels[] = {charg_level_gear2,
                                    charg_level_gear3,
                                    charg_level_gear4,
                                    charg_level_gear5};
        size_t num_levels = sizeof(charge_levels) / sizeof(charge_levels[0]);

        led_config->ledMask = 0;
        flow_interval = LL_GetTick();
        batteryLevel = charge_bat_vol();

        // 迭代充电等级，根据电量设置LED通道
        for (size_t i = 0; i < num_levels; ++i)
        {
            if (batteryLevel < charge_levels[i])
            {
                // 添加对应的LED通道
                led_config->ledMask |= LED_CHANNEL(led_channels[i]);
            }
            else
            {
                // 显示目前有几格电量
                set_led_multi(LED_CHANNEL(led_channels[i]), LED_ON);
            }
        }
    }
    if (LED_Update(led_config))
    {
        return 1;
    }
    // 返回0会继续执行下个优先级任务，返回1则不运行其他低优先级任务
    return 0;
}
// 充满电LED事件
uint8_t bat_full_led_handle(LED_Config_t *led_config)
{
    LED_Update(led_config);
    // 返回0会继续执行下个优先级任务，返回1则不运行其他低优先级任务
    return 0;
}
// 缺电LED事件
uint8_t bat_low_led_handle(LED_Config_t *led_config)
{
    printf("bat_low_led_handle\r\n");
    state_machine.bat_low_flag = 1;
    if (!LED_Update(led_config))
    {
        LED_EventDelete(EVENT_LOW_BATTERY);     // 删除缺电事件
        state_machine.board_state = BOARD_STOP; // 进入休眠状态
    }
    // 返回0会继续执行下个优先级任务，返回1则不运行其他低优先级任务
    return 0;
}

// 电机电流过大LED事件
uint8_t current_high_handle(LED_Config_t *led_config)
{
    if (!LED_Update(led_config))
    {
        LED_EventDelete(EVENT_CURRENT_HIGH);    // 删除电流过大事件
        state_machine.board_state = BOARD_STOP; // 进入休眠状态
        logAssert();
    }
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
    case USER_LED3:
        set_led(USER_LED3_GPIO_PORT, USER_LED3_PIN, state);
        break;
    case USER_LED4:
        set_led(USER_LED4_GPIO_PORT, USER_LED4_PIN, state);
        break;
    case USER_LED5:
        set_led(USER_LED5_GPIO_PORT, USER_LED5_PIN, state);
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
// 缺电处理函数
void low_bat_handle(void)
{
    static uint8_t lastLowBat;
    if (state_machine.board_state != BOARD_CHARGE)
    {
        if (lastLowBat != state_machine.bat_low_flag)
        {
            lastLowBat = state_machine.bat_low_flag;
            if (state_machine.bat_low_flag)
            {
                LL_GPIO_ResetOutputPin(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN); // 关闭电机电源
                set_led_multi(0xFFFF, LED_OFF); // 关闭所有LED
                state_machine.board_state = BOARD_IDLE;
                LED_EventAdd(EVENT_LOW_BATTERY); // 添加缺电事件
            }
            else
            {
                LED_EventDelete(EVENT_LOW_BATTERY); // 删除缺电事件
            }
        }
    }
    else
    {
        LED_EventDelete(EVENT_LOW_BATTERY); // 删除缺电事件
    }
}

void bat_full_handle(void)
{ // 该函数是用于充电拔出时，过滤掉充满的标志，也可以用于充满后再充一段时间
    if (LL_GetTickDiff(state_machine.bat_full.charge_full_start_time) > 1000 && state_machine.bat_full.charge_full_valid_flag)
    {
        last_bat_value = BAT_MAX_VOL;
        state_machine.bat_full.charge_full_valid_flag = 0;
        LED_EventAdd(EVENT_FULL_BATTERY); // 添加充满LED事件
        batPrintf(LOG_NOTIC, "charge_Full\r\n");
    }
}

// 电池补偿函数，用于电机运行时显示电压百分比
uint16_t BAT_DischargeVol(void)
{
    uint8_t bat_percentage;
    static uint16_t compensation_vol;
    static uint32_t compensation_tick;
    uint16_t bat_value = get_bat_vol();
    batPrintf(LOG_NOTIC, "bat_value:%d\r\n", bat_value);

    // 小于等于8.2V时，进入缺电状态，不然会触发电池保护板低压保护
    if (bat_value <= 8400)
    {
        state_machine.bat_low_flag = 1;
        batPrintf(LOG_NOTIC, "bat Vol < 8200V\r\n");
    }
    
    // 进行电池补偿
    if (state_machine.motor_board_state == MOTOR_RUNNING_STATUS)
    {
        compensation_tick = LL_GetTick();
        compensation_vol = 1870; // 补偿1.87V
    }else if (state_machine.motor_board_state == PUMP_RUNNING_STATUS)
    {
        compensation_tick = LL_GetTick();
        compensation_vol = 750; // 补偿0.75V
    }else
    {
        if (LL_GetTickDiff(compensation_tick) >= 5000)
        {
            compensation_vol = 0;
        }
    }

    if ((bat_value + compensation_vol) < last_bat_value)
    {
        last_bat_value = (bat_value + compensation_vol);
    }
    if (last_bat_value <= BAT_MIN_VOL)
    {
        return 0;
    }
    else
    {
        bat_percentage = ((last_bat_value - BAT_MIN_VOL) * 100) / (BAT_MAX_VOL - BAT_MIN_VOL);
        if (bat_percentage > 100)
        {
            bat_percentage = 100;
        }
        batPrintf(LOG_NOTIC, "bat_percentage %d\r\n", bat_percentage);
        return bat_percentage;
    }
}

// 电池电压显示，用于充电时显示电池电压
uint8_t charge_bat_vol(void)
{
    uint32_t bat_vol, adc_vol;
    uint8_t bat_percentage;
    batPrintf(LOG_DEBUG, "get_bat_adc_value:%d\r\n", get_bat_adc_value());
    adc_vol = (ADC_POWER_VOL * get_bat_adc_value()) / 4095;
    bat_vol = (adc_vol * (BAT_ADC_R1 + BAT_ADC_R2)) / BAT_ADC_R2;
    batPrintf(LOG_DEBUG, "bat_vol %d\r\n", bat_vol);

    //防止充电时上下波动
    if (bat_vol > last_bat_value)
    {
        last_bat_value = bat_vol;
    }

    if (last_bat_value <= BAT_MIN_VOL)
    {
        return 0;
    }
    else
    {
        bat_percentage = ((last_bat_value - BAT_MIN_VOL) * 100) / (BAT_MAX_VOL - BAT_MIN_VOL);
        if (bat_percentage > 100)
        {
            bat_percentage = 100;
        }
        batPrintf(LOG_NOTIC, "bat_percentage %d\r\n", bat_percentage);
        return bat_percentage;
    }
}

//检测反馈脚电平
uint8_t feedback_read(void)
{
    if (LL_GPIO_IsInputPinSet(PUMP_FG_GPIO_PORT, PUMP_FG_PIN))
    {
        return 1;
    }
    return 0;
}

void feedback_time_init(void)
{
    low_time = LL_GetTick();
    high_time = LL_GetTick();
}

// 检查电机板的状态
void detect_motor_pump_status()
{
    static uint8_t num = 0;
    
    if (state_machine.board_state == BOARD_CHARGE)
    {
        feedback_time_init();
        return;
    }

    uint8_t io_status = feedback_read();

    if (io_status)
    {
        low_time = LL_GetTick();
    }
    else
    {
        if (LL_GetTickDiff(high_time) < 600 && LL_GetTickDiff(high_time) >= 500)
        { // 500ms 欠压
            if (UNDER_VOL_STATUS != state_machine.motor_board_state)
            {
                if (num++ == 2)
                {
                    num = 0;
                    state_machine.motor_board_state = UNDER_VOL_STATUS;
                    appPrintf(LOG_NOTIC, "motor_board_state :UNDER_VOL_STATUS\r\n");
                }
            } 
        }
        else if (LL_GetTickDiff(high_time) >= 400)
        { // 400ms 过流
            if (OVER_CURRENT_STATUS != state_machine.motor_board_state)
            {
                if (num++ == 2)
                {
                    num = 0;
                    state_machine.motor_board_state = OVER_CURRENT_STATUS;
                    appPrintf(LOG_NOTIC, "motor_board_state :OVER_CURRENT_STATUS\r\n");
                }
            } 
        }
        else if (LL_GetTickDiff(high_time) >= 300)
        { // 300ms 工作完成
            if (JOB_COMPLETE_STATUS != state_machine.motor_board_state)
            {
                if (num++ == 2)
                {
                    num = 0;
                    state_machine.motor_board_state = JOB_COMPLETE_STATUS;
                    appPrintf(LOG_NOTIC, "motor_board_state :JOB_COMPLETE_STATUS\r\n");
                }
            } 
        }
        else if (LL_GetTickDiff(high_time) >= 200)
        { // 200ms的高电平 电机
            if (MOTOR_RUNNING_STATUS != state_machine.motor_board_state)
            {
                if (num++ == 2)
                {
                    num = 0;
                    state_machine.motor_board_state = MOTOR_RUNNING_STATUS;
                    appPrintf(LOG_NOTIC, "motor_board_state :MOTOR_RUNNING_STATUS\r\n");
                }
            } 
        }
        else if (LL_GetTickDiff(high_time) >= 100)
        { // 100ms的高电平 隔膜泵
            if (PUMP_RUNNING_STATUS != state_machine.motor_board_state)
            {
                if (num++ == 2)
                {
                    num = 0;
                    state_machine.motor_board_state = PUMP_RUNNING_STATUS;
                    appPrintf(LOG_NOTIC, "motor_board_state :PUMP_RUNNING_STATUS\r\n");

                }
            }
        }
        high_time = LL_GetTick();
    }

    if (LL_GetTickDiff(high_time) > 3000 || LL_GetTickDiff(low_time) > 3000)
    {
        state_machine.board_state = BOARD_STOP; // 进入休眠状态
        state_machine.motor_board_state = NO_STATUS;
        appPrintf(LOG_NOTIC, "motor_board_state :NO_STATUS\r\n");
        
    }
}



/***********************用户应用层结束*****************/
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/*******************状态机应用层开始********************/
uint8_t motorNumSamples = 0;

void app_state_machine_handle(void)
{
    static uint8_t state_list = 0;
    if (state_list != state_machine.board_state)
    {
        appPrintf(LOG_NOTIC, "state_machine.board_state %d\r\n", state_machine.board_state);
    }
    switch (state_machine.board_state)
    {
    case BOARD_IDLE:
        state_list = state_machine.board_state;
        break;
    case BOARD_WORK:
        state_list = state_machine.board_state;
        LL_GPIO_SetOutputPin(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN);
        if (state_machine.motor_board_state == OVER_CURRENT_STATUS) // 过流
        {
            state_machine.board_state = BOARD_IDLE;
            LL_GPIO_ResetOutputPin(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN); // 关闭电机电源
            set_led_multi(0xFFFF, LED_OFF);                           // 关闭所有LED
            LED_EventAdd(EVENT_CURRENT_HIGH);                         // 添加电流过大LED事件
        }
        else if (state_machine.motor_board_state == JOB_COMPLETE_STATUS) // 工作完成（气压达到设定值）
        {
            state_machine.board_state = BOARD_STOP; // 进入休眠状态
        }else if (state_machine.motor_board_state == UNDER_VOL_STATUS) // 欠压输出高电平，电池相当于没电，进入缺电模式
        {
            // state_machine.board_state = BOARD_STOP; // 进入休眠状态
        }
        break;
    case BOARD_STOP:
    {
        static uint32_t stop_interval_time;
        // 唤醒后让key有时间执行事件函数，不然key还没判断完成又进入休眠了
        if (((state_list == state_machine.board_state) && (LL_GetTickDiff(stop_interval_time) >= 5000)) ||
            ((state_list != state_machine.board_state) && (LL_GetTickDiff(stop_interval_time) >= 500)))
        {
            state_list = state_machine.board_state;
            state_machine.motor_board_state = NO_STATUS;
            state_machine.board_fiting_flag = 0;
            LL_GPIO_ResetOutputPin(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN); // 关闭电机电源
            set_led_multi(0xFFFF, LED_OFF);                           // 关闭所有LED
            LED_EventDelete(EVENT_NOT_CHARGING);                      // 删除不充电事件
            mcu_enter_sleep();
            stop_interval_time = LL_GetTick();
        }
    }
    break;
    case BOARD_CHARGE:
        state_list = state_machine.board_state;
        LL_GPIO_ResetOutputPin(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN); // 关闭电机电源
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
// 气泵电源控制脚和FG初始化
void pump_pwr_gpio_init(void)
{
    PUMP_PWR_GPIO_CLK_ENABLE();
    PUMP_FG_GPIO_CLK_ENABLE();

    LL_GPIO_SetPinMode(PUMP_FG_GPIO_PORT, PUMP_FG_PIN, LL_GPIO_MODE_INPUT);    // 反馈脚初始化
    LL_GPIO_SetPinPull(PUMP_FG_GPIO_PORT, PUMP_FG_PIN, LL_GPIO_PULL_DOWN);  // 反馈脚下拉

    LL_GPIO_SetPinMode(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN, LL_GPIO_MODE_OUTPUT); // 电源控制脚初始化
    LL_GPIO_SetPinPull(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN, LL_GPIO_PULL_DOWN);   // 电源控制脚下拉
    LL_GPIO_ResetOutputPin(PUMP_PWR_GPIO_PORT, PUMP_PWR_PIN); // 关闭电机电源
}

void app_Init(void)
{
    Tim14_Init();
    pump_pwr_gpio_init();
    adc_Init();
    BSP_USART_Config(); // 将串口配置成日志口
    appPrintf(LOG_NOTIC, "Start running\r\n");
    version_printf();
    user_key_Init();
    user_led_init();
    // 上电所有LED闪烁一次后进入睡眠
    LED_EventAdd(EVENT_POWER_TEST);
    LED_EventHandle();
    mcu_enter_sleep();
    BSP_USART_Config(); // 将串口配置成日志口(唤醒之后有时候看不到日志，重新初始化一下)
    LED_EventDelete(EVENT_POWER_TEST);
    iwdg_Init(); // 看门狗初始化
    appPrintf(LOG_NOTIC, "iwdg 1S Init\r\n");
}

void app_lication(void)
{
    while (1)
    {
        adc_gather_handle();        // ADC采集
        keyHandle();                // 按键处理函数
        low_bat_handle();           // 缺电处理函数
        bat_full_handle();          // 满电处理函数
        LED_EventHandle();          // LED处理函数
        app_state_machine_handle(); // 状态机处理函数
        detect_motor_pump_status(); // 主控板状态检测
        iwdg_FeedDog();          // 看门狗喂狗
    }
}
