#include "circular_buffer.h"

/* Initialize the circular buffer */
void CircularBuffer_Init(CircularBuffer_t *buffer) {
    if (buffer != NULL) {
        buffer->head = 0U;
        buffer->tail = 0U;
        buffer->count = 0U;
    }
}

/* Add an element to the buffer */
BufferStatus_t CircularBuffer_Add(CircularBuffer_t *buffer, uint8_t value) {
    if (buffer == NULL) {
        return BUFFER_OVERFLOW; /* Invalid buffer */
    }

    if (buffer->count == BUFFER_SIZE) {
        return BUFFER_OVERFLOW; /* Buffer is full */
    }

    buffer->data[buffer->head] = value;
    buffer->head = (buffer->head + 1U) % BUFFER_SIZE;
    buffer->count++;

    return BUFFER_OK;
}

/* Get an element from the buffer */
BufferStatus_t CircularBuffer_Get(CircularBuffer_t *buffer, uint8_t *value) {
    if ((buffer == NULL) || (value == NULL)) {
        return BUFFER_UNDERFLOW; /* Invalid parameters */
    }

    if (buffer->count == 0U) {
        return BUFFER_UNDERFLOW; /* Buffer is empty */
    }

    *value = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1U) % BUFFER_SIZE;
    buffer->count--;

    return BUFFER_OK;
}

/* Check if the buffer is empty */
uint8_t CircularBuffer_IsEmpty(const CircularBuffer_t *buffer) {
    if (buffer == NULL) {
        return 1U; /* Null buffer treated as empty */
    }

    return (buffer->count == 0U) ? 1U : 0U;
}

/* Check if the buffer is full */
uint8_t CircularBuffer_IsFull(const CircularBuffer_t *buffer) {
    if (buffer == NULL) {
        return 0U; /* Null buffer cannot be full */
    }

    return (buffer->count == BUFFER_SIZE) ? 1U : 0U;
}
