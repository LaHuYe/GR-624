/**
 * ***********************************************************
 * @file key.h
 * @author cyWU
 * @brief key.c对应头文件
 * @version 0.2
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 * ***********************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "main.h"

#define KEY_TIMER_MS 1
#define KEY_MAX_NUMBER 12
#define DEBOUNCE_TIME 30
#define KEY_INTERVAL 160
// #define PRESS_LONG_TIME 1000
// #define PRESS_LAST_TIME 5000

#define CLICK 1    // Key click
#define DbLCLICK 2 // Key double click

typedef enum
{
    KEY_POWER,       // 电源按键
    KEY_NUM,         // Number of keys,Must be placed at the bottom of the registry
} keyList;

typedef enum
{
    Bit_RESET = 0,
    Bit_SET
} BitAction_t;

/*Key status enumeration*/
typedef enum
{
    KEY_NULL,    // Key no action
    KEY_RELEASE, // Key release
    KEY_SURE,    // Key quaking elimination
    KEY_UP,      // Key release
    KEY_DOWN,    // Key down
    KEY_LONG,    // Key long
} keyStatus_t;

/*Key event enumeration*/
typedef enum
{
    NULL_Event,    // Empty event
    DOWN_Event,    // Down event
    SHORT_Event,   // Short event
    LONG_Event,    // Long event
    LAST_Event,    // Continuous Events
    DBCL_Event,    // Double click event
    RELEASE_Event, // release event
} keyEvent_t;

__packed typedef struct
{
    uint32_t keyPin;
    uint32_t keyMode;
    uint32_t keyPull;
    GPIO_TypeDef *keyPort;
} keyInit_t;

/*Key state machine structure*/
__packed typedef struct
{
    bool keyShortFlag;         // key short flag
    bool keyLongFlag;          // key Long flag
    uint8_t keyInterval;       // Key interval time
    uint8_t keyFrequency;      // Number of key presses
    uint16_t keyLongTime;      // Button Long Time
    uint16_t keyLastTime;      // Button Last Time
    uint32_t keyCount;         // Long press key counting
    keyEvent_t keyEvent_Init;  // Key event init
    FunctionalState keyShield; // Key enabling position
    BitAction_t keyLevel;      // Judge whether the key is pressed, press: 1, lift: 0
    BitAction_t keyDownLevel;  // Level state of IO port when the key is pressed
    keyStatus_t keyStatus;     // Current state of the key
    keyEvent_t eventType;      // Key Event Type
    uint8_t (*keyReadValue)(void);
} keyFSM_t;

/*Callback functions corresponding to different events*/
__packed typedef struct
{
    void (*nullPressCb)(void);
    void (*releasePressCb)(void);
    void (*downPressCb)(void);
    void (*ShortPressCb)(void);
    void (*longPressCb)(void);
    void (*lastPressCb)(void);
    void (*dbclPressCb)(void);
} keyFunc_t;

/*Key class*/
__packed typedef struct
{
    keyFSM_t fsm;
    keyFunc_t func;
} keyCategory_t;

void keyParaInit(keyCategory_t *keys);
void keyCheckProcess(void);
void keyHandle(void);

void power_key_enable(FunctionalState key_state);
void charge_full_key_enable(FunctionalState key_state);

#endif
