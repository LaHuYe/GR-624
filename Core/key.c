
/**
 * ***********************************************************
 * @file key.c
 * @author cyWU
 * @brief 按键应用框架
 * @version 0.2
 * @date 2024-01-29
 *
 * @copyright Copyright (c) 2024
 * ***********************************************************
 */

#include "key.h"
#include "adc.h"
#include <math.h>

uint32_t keyCountTime;
uint8_t keyNum = KEY_NUM;
keyCategory_t keyTable[KEY_NUM];


/**
 * @brief Judge whether the key is pressed
 * @param [in] key :Key state machine global structure pointer
 * @return :  true,Key status read successfully;
 *            false,The key is not enabled;
 */
static bool getKeyLevel(keyFSM_t *key)
{
    if (key->keyShield == DISABLE)
        return false;

    if (key->keyReadValue() == key->keyDownLevel)
    {
        key->keyLevel = Bit_SET;
    }
    else
    {
        key->keyLevel = Bit_RESET;
    }
    return true;
}

/**
 * @brief Judge whether the key is pressed once or several times
 * @param [in] click :Key state machine global structure pointer
 * @return : true,If the key interval time is exceeded, return to the event;
 *           false,The event is not returned if the key does not exceed the key interval;
 */
static bool keyClickFrequency(keyFSM_t *click)
{
    click->keyInterval++;
    switch (click->keyFrequency)
    {
    case CLICK:
        if (click->keyInterval >= (KEY_INTERVAL / (DEBOUNCE_TIME * KEY_TIMER_MS)))
        {
            click->eventType = SHORT_Event; // Click Event
            return true;
        }
        break;
    case DbLCLICK:
        if (click->keyInterval >= (KEY_INTERVAL / (DEBOUNCE_TIME * KEY_TIMER_MS)))
        {
            click->eventType = DBCL_Event; // Double click event
            return true;
        }
        break;
    /*
    More click events are added as required
    case XXXX:
        if (click->keyInterval >= (KEY_INTERVAL / (DEBOUNCE_TIME * KEY_TIMER_MS)))
        {
            click->eventType = XXXXXX;
            return true;
        }
        break;
    */
    default:
        return true;
    }
    return false;
}

/**
 * @brief Read the KEY value
 * @param [in] Key_Buf :Key state machine global structure pointer
 * @return : true,KEY value get succeeded;
 *           false,The key is not enabled;
 */
static bool readKeyStatus(keyFSM_t *Key_Buf)
{

    if (!getKeyLevel(Key_Buf))
        return false;

    switch (Key_Buf->keyStatus)
    {
    // Status 0: No key pressed
    case KEY_NULL:
        if (Key_Buf->keyLevel == Bit_SET) // Press the key down
        {
            Key_Buf->keyStatus = KEY_SURE;
        }
        break;
        // Status 1: Press to confirm
    case KEY_SURE:
        if (Key_Buf->keyLevel == Bit_SET) // Confirm to press the key down
        {
            Key_Buf->eventType = DOWN_Event; // Trigger key down Event
            Key_Buf->keyStatus = KEY_DOWN;
            Key_Buf->keyCount = 0; // The count value of the key is cleared to 0
            Key_Buf->keyLongFlag = true;
        }
        else
        {
            Key_Buf->keyStatus = KEY_NULL;
        }
        break;
    // Status 2: Press the key down
    case KEY_DOWN:
        if (Key_Buf->keyLevel != Bit_SET) // Key release
        {
            if (Key_Buf->keyShortFlag == false) // Press the key to press down the flag bit
            {
                Key_Buf->keyShortFlag = true;
                Key_Buf->keyFrequency++;  // Number of keystrokes plus 1
                Key_Buf->keyInterval = 0; // The interval between keys is 0
            }
            if (keyClickFrequency(Key_Buf)) // Multi click judgment
            {
                Key_Buf->keyFrequency = 0;
                Key_Buf->keyStatus = KEY_NULL;
            }
            if ((Key_Buf->keyCount >= Key_Buf->keyLongTime / DEBOUNCE_TIME) && // Press the key for 1s-3s for a long time
                (Key_Buf->keyCount < Key_Buf->keyLastTime / DEBOUNCE_TIME))
            {
                Key_Buf->keyFrequency = 0;
                Key_Buf->keyStatus = KEY_NULL;
                Key_Buf->eventType = RELEASE_Event; // Trigger long press event 
            }
            Key_Buf->keyCount = 0; // Reset the key counter
            Key_Buf->keyLongFlag = true;
        }
        else
        {
            if ((++Key_Buf->keyCount >= Key_Buf->keyLastTime / DEBOUNCE_TIME)) // Over KEY_ LONG_ DOWN_ DELAY is not released
            {
                Key_Buf->keyCount = 0; // Reset the key counter
                Key_Buf->keyFrequency = 0;
                Key_Buf->keyStatus = KEY_LONG;
                Key_Buf->eventType = LAST_Event; // Trigger continuous press event
                Key_Buf->keyLongFlag = true;
            }
            if ((Key_Buf->keyCount >= Key_Buf->keyLongTime / DEBOUNCE_TIME) && // Press the key for 1s-3s for a long time
                (Key_Buf->keyCount < Key_Buf->keyLastTime / DEBOUNCE_TIME))
            {
                Key_Buf->keyFrequency = 0;
                if (Key_Buf->keyLongFlag == true)
                {
                    Key_Buf->eventType = LONG_Event; // Trigger long press event
                    Key_Buf->keyLongFlag = false;
                }
            }
            Key_Buf->keyShortFlag = false; // Press the key to press down the flag bit
        }
        break;
    // State 3: Press and hold the key for a long time
    case KEY_LONG:
        if (Key_Buf->keyLevel != Bit_SET) // Key release
        {
            Key_Buf->keyStatus = KEY_NULL;
            Key_Buf->eventType = RELEASE_Event; // Trigger null event
        }
        else
        {
            // Key_Buf->eventType = LAST_Event; // Trigger continuous press event
        }
        break;
    default:
        break;
    }
    return true;
}

/**
 * @brief Handle key event function
 * @param  None
 * @retval None
 */
void keyEventProcess(void)
{
    for (size_t i = 0; i < keyNum; i++)
    {
        if (!readKeyStatus(&keyTable[i].fsm))
            continue;
    }
}

/**
 * @brief Key processing function
 * @param  None
 * @retval None
 */
void keyCheckProcess(void)
{
    keyCountTime++;
    if (keyCountTime >= (DEBOUNCE_TIME / KEY_TIMER_MS))
    {
        keyCountTime = 0;
        keyEventProcess();
    }
}

/**
 * @brief key parameter init function
 * @param [in] keys :Key global structure pointer
 * @return none
 */
void keyParaInit(keyCategory_t *keys)
{
    if (NULL == keys)
    {
        return;
    }
    if (KEY_NUM >= KEY_MAX_NUMBER)
    {
        keyNum = KEY_MAX_NUMBER;
    }
    
    memcpy(keyTable, keys, sizeof(keyCategory_t) * keyNum);
}

void keyHandle(void)
{
    for (size_t i = 0; i < keyNum; i++)
    {
        if (keyTable[i].fsm.eventType == NULL_Event)
            continue;
        switch (keyTable[i].fsm.eventType)
        {
        case RELEASE_Event:
            if (keyTable[i].func.releasePressCb == NULL)
                break;
            keyTable[i].func.releasePressCb();
            break;
        case SHORT_Event:
            if (keyTable[i].func.ShortPressCb == NULL)
                break;
            keyTable[i].func.ShortPressCb();
            break;
        case DOWN_Event:
            if (keyTable[i].func.downPressCb == NULL)
                break;
            keyTable[i].func.downPressCb();
            break;
        case LONG_Event:
            if (keyTable[i].func.longPressCb == NULL)
                break;
            keyTable[i].func.longPressCb();
            break;
        case LAST_Event:
            if (keyTable[i].func.lastPressCb == NULL)
                break;
            keyTable[i].func.lastPressCb();
            break;
        case DBCL_Event:
            if (keyTable[i].func.dbclPressCb == NULL)
                break;
            keyTable[i].func.dbclPressCb();
            break;
        default:
            break;
        }
        if (keyTable[i].fsm.eventType == RELEASE_Event ||
            keyTable[i].fsm.eventType == DOWN_Event ||
            keyTable[i].fsm.eventType == SHORT_Event ||
            keyTable[i].fsm.eventType == LONG_Event)
        {
            keyTable[i].fsm.eventType = NULL_Event;
        }
    }
}

// 用户应用层
void power_key_enable(FunctionalState key_state)
{
    keyTable[KEY_POWER].fsm.keyShield = key_state;
}
void charge_full_key_enable(FunctionalState key_state)
{
    keyTable[KEY_CHARGE_FULL].fsm.keyShield = key_state;
}
