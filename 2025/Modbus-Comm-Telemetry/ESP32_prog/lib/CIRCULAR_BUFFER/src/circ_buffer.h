#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 128

typedef struct {
	uint8_t* data;
	uint8_t head;
	uint8_t tail;
	size_t max_bytes;
	bool full;
} circ_buffer_t;


size_t 	cb_taken_space (circ_buffer_t *cb);
void 	cb_buffer_init (circ_buffer_t *cb, uint8_t *buffer, size_t size);
bool 	cb_buffer_push (circ_buffer_t *cb, uint8_t *data, size_t len);
size_t 	cb_buffer_pull (circ_buffer_t *cb, uint8_t *data, size_t len);
void 	cb_buffer_reset (circ_buffer_t *cb);