/*
 * Copyright (c) 2016 Centropy <contact@centropy.info>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "rb.h"


/* Function: rb_init
 * -----------------
 *  Initiatialize ringbuffer. Increment length for a seperation byte.
 *
 *  len:	Size of the ring buffer.
 *
 *  Returns:	Pointer to a ringbuffer.
 */
struct ring *rb_init(size_t len)
{
	struct ring *rb = malloc(sizeof(struct ring));
	/* Make ring buffer 1 byte bigger, this is the free byte needed to differ full/empty. */
	len++;

	rb->buffer = calloc(len, (sizeof(uint8_t) * len));
	rb->wptr = rb->buffer;
	rb->rptr = rb->buffer;
	rb->size = len;
	return rb;
}


/* Function: rb_used
 * -----------------
 *  Calculate number of bytes in use.
 *
 *  *rb:	Pointer to a ringbuffer.
 *
 *  Returns:	Bytes used in a ringbuffer.
 */
size_t rb_used(struct ring *rb)
{
	int bytes = (int)(rb->wptr - rb->rptr);

	/* If the result is negative the buffer has wrapped around. */
	if (bytes < 0) {
		bytes += rb->size;
	}

	return bytes;
}


/* Function: rb_left
 * -----------------
 *  Calculate number of bytes free to use.
 *
 *  *rb:	Pointer to a ringbuffer.
 *
 *  Returns:	Bytes free in our ringbuffer.
 */
size_t rb_left(struct ring *rb)
{
	/* Free = free - the seperation byte. */
	int bytes = (int)(rb->rptr - rb->wptr - 1);

	if (bytes < 0) {
		bytes += rb->size;
	}

	return bytes;
}


/* Function: rb_write_byte
 * -----------------------
 *  Internal function to write a byte into a ringbuffer.
 *
 *  *rb:	Pointer to a ringbuffer.
 *  *data:	Data byte to write.
 *
 *  Returns:	Bytes wrote.
 */
static size_t rb_write_byte(struct ring *rb, const uint8_t *data)
{
	if (rb->wptr >= (rb->buffer + rb->size)) {
		rb->wptr = rb->buffer;
	}

	*rb->wptr++ = *data;

	return 1;
}


/* Function: rb_read_byte
 * ----------------------
 *  Internal function to read a byte from a ringbuffer.
 *
 *  *rb:	Pointer to a ringbuffer.
 *  *data:	Data byte to read into.
 *
 *  Returns:	Bytes read.
 */
static size_t rb_read_byte(struct ring *rb, uint8_t *data)
{
	if (rb->rptr >= (rb->buffer + rb->size)) {
		rb->rptr = rb->buffer;
	}

	*data = *rb->rptr++;

	return 1;
}


/* Function: rb_write
 * ------------------
 *  Check if there is enough free space, write data into a ringbuffer.
 *
 *  *rb:	Pointer to a ringbuffer.
 *  *data:	Data to write.
 *  len:	Data length.
 *
 *  Returns:	Bytes wrote on success.
 *		-1 and EIO errno on failure.
 */
size_t rb_write(struct ring *rb, const uint8_t *data, size_t len)
{
	if (len > rb_left(rb)) {
		errno = EIO;
		return -1;
	}

	size_t i;
	for (i = 0; i < len; i++) {
		rb_write_byte(rb, data++);
	}

	return i;
}


/* Function: rb_read
 * ------------------
 *  Check if there are enough used bytes, otherwise change length.
 *  Read data from a ringbuffer.
 *
 *  *rb:	Pointer to a ringbuffer.
 *  *data:	Data to read into.
 *  len:	Data length.
 *
 *  Returns:	Bytes read on success.
 *		-1 and EIO errno on failure. (i need to add more checks here!)
 */
size_t rb_read(struct ring *rb, uint8_t *data, size_t len)
{
	if (len > rb_used(rb)) {
		len = rb_used(rb);
	}

	size_t i;
	for (i = 0; i < len; i++) {
		rb_read_byte(rb, data++);
	}

	return i;
}


