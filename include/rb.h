/*
 * All copyright and related rights waived via CC0.
 */


#ifndef __RB_H
#define __RB_H


struct ring {
	uint8_t *buffer;
	uint8_t *wptr;
	uint8_t *rptr;
	size_t size;
};


struct ring *rb_init(size_t len);
size_t rb_used(struct ring *rb);
size_t rb_left(struct ring *rb);
size_t rb_write(struct ring *rb, const uint8_t *data, size_t len);
size_t rb_read(struct ring *rb, uint8_t *data, size_t len);


#endif
