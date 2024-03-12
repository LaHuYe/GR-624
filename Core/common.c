#include "common.h"


// 初始化循环队列
void initQueue(CircularQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
}

// 入队
void enqueue(CircularQueue *queue, uint16_t value) {
    queue->data[queue->rear] = value;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE; // 更新尾指针
    if (queue->count < QUEUE_SIZE) {
        queue->count++;
    } else {
        queue->front = (queue->front + 1) % QUEUE_SIZE; // 如果队列满了，更新头指针覆盖最旧的数据
    }
}

// 计算移动平均值
uint16_t movingAverage(CircularQueue *queue) {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < queue->count; i++) {
        sum += queue->data[(queue->front + i) % QUEUE_SIZE]; // 计算队列中所有元素的总和
    }
    return sum / queue->count; // 返回平均值
}

