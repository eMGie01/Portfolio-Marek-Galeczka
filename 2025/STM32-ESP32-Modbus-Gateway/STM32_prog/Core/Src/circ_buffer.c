/*
 * circ_buffer.c
 *
 *  Created on: Oct 24, 2025
 *      Author: galeczka
 */
#include "circ_buffer.h"

#include <string.h>

// How much space is taken in the buffer
size_t cb_taken_space (circ_buffer_t *cb) {
	if (cb->full) return cb->max_bytes;
	return ( cb->head >= cb->tail ) ? (cb->head - cb->tail) : (cb->max_bytes + cb->head - cb->tail);
}

// How much space is free in the buffer
static size_t cb_free_space (circ_buffer_t *cb) {
	return ( cb->max_bytes - cb_taken_space(cb) );
}

// Returns the information if the buffer is full
//static bool cb_is_full (circ_buffer_t *cb) {
//	return cb->full;
//}

// Returns the information if the buffer is empty
static bool cb_is_empty (circ_buffer_t *cb) {
	return (!cb->full && (cb->head == cb->tail));
}

//Function to init Circular Buffer
void cb_buffer_init (circ_buffer_t *cb, uint8_t *buffer, size_t size) {
	cb->data = buffer;
	cb->head = 0;
	cb->tail = 0;
	cb->max_bytes = size;
	cb->full = false;
}

// Function for writing into the buffer
bool cb_buffer_push (circ_buffer_t *cb, uint8_t *data, size_t len) {
	if (len > cb_free_space(cb)) return false;

	size_t part = cb->max_bytes - cb->head;
	if ( part >= len ) {
		memcpy(&cb->data[cb->head], data, len);
	} else {
		memcpy(&cb->data[cb->head], data, part);
		memcpy(&cb->data[0], data + part, len - part);
	}
	cb->head = (cb->head + len) % cb->max_bytes;
	cb->full = (cb->tail == cb->head);
	return true;
}

size_t cb_buffer_pull (circ_buffer_t *cb, uint8_t *data, size_t len) {
	if (cb_is_empty(cb) || len == 0) return 0;

	size_t taken_space = cb_taken_space(cb);
	size_t to_read = (len < taken_space) ? len : taken_space;

	size_t part = cb->max_bytes - cb->tail;
	if (part >= to_read) {
		memcpy(data, &cb->data[cb->tail], to_read);
	} else {
		memcpy(data, &cb->data[cb->tail], part);
		memcpy(data + part, &cb->data[0], to_read - part);
	}
	cb->tail = (cb->tail + to_read) % cb->max_bytes;
	cb->full = false;
	return to_read;
}

// Reseting buffer
void cb_buffer_reset (circ_buffer_t *cb) {
	cb->head = 0;
	cb->tail = 0;
	cb->full = false;
}
