/*
 *  Copyright (C) 2003 Paul Davis
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  $Id: ringbuffer.h,v 1.3 2003/11/19 15:28:17 theno23 Exp $
 */

#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include <sys/types.h>

typedef struct  
{
  char *buf;
  size_t len;
} 
ringbuffer_data_t ;

typedef struct
{
  char *buf;
  volatile size_t write_ptr;
  volatile size_t read_ptr;
  size_t size;
  size_t size_mask;
  int mlocked;
} 
ringbuffer_t ;

ringbuffer_t *ringbuffer_create(int sz);
void ringbuffer_free(ringbuffer_t *rb);

int ringbuffer_mlock(ringbuffer_t *rb);
void ringbuffer_reset(ringbuffer_t *rb);

void ringbuffer_write_advance(ringbuffer_t *rb, size_t cnt);
void ringbuffer_read_advance(ringbuffer_t *rb, size_t cnt);

size_t ringbuffer_write_space(ringbuffer_t *rb);
size_t ringbuffer_read_space(ringbuffer_t *rb);

size_t ringbuffer_read(ringbuffer_t *rb, char *dest, size_t cnt);
size_t ringbuffer_write(ringbuffer_t *rb, char *src, size_t cnt);

void ringbuffer_get_read_vector(ringbuffer_t *rb, ringbuffer_data_t *vec);
void ringbuffer_get_write_vector(ringbuffer_t *rb, ringbuffer_data_t *vec);

#endif
