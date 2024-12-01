#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stddef.h>

/* Define buffer size */
#define BUFFER_SIZE 10U

/* Error codes */
typedef enum {
    BUFFER_OK,
    BUFFER_OVERFLOW,
    BUFFER_UNDERFLOW
} BufferStatus_t;

/* Circular buffer structure */
typedef struct {
    uint8_t data[BUFFER_SIZE]; /* Data storage */
    uint8_t head;              /* Index for the next write */
    uint8_t tail;              /* Index for the next read */
    uint8_t count;             /* Number of elements in the buffer */
} CircularBuffer_t;

/* Function Prototypes */
void CircularBuffer_Init(CircularBuffer_t *buffer);
BufferStatus_t CircularBuffer_Add(CircularBuffer_t *buffer, uint8_t value);
BufferStatus_t CircularBuffer_Get(CircularBuffer_t *buffer, uint8_t *value);
uint8_t CircularBuffer_IsEmpty(const CircularBuffer_t *buffer);
uint8_t CircularBuffer_IsFull(const CircularBuffer_t *buffer);

#endif /* CIRCULAR_BUFFER_H */
