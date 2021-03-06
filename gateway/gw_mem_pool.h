/*
 * Copyright 2015 Naver Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _GW_MEM_POOL_H_
#define _GW_MEM_POOL_H_

#include <assert.h>

#include "zmalloc.h"

#define MEMPOOL_DEFAULT_POOL_SIZE   16384

typedef struct
{
  size_t buf_size;
  size_t alloc_count;
  size_t pool_size;
  size_t pool_count;
  void **pool;
} mempool_hdr_t;

static inline mempool_hdr_t *
mempool_create (size_t buf_size, size_t pool_size)
{
  mempool_hdr_t *hdr;

  hdr = zmalloc (sizeof (mempool_hdr_t));
  hdr->buf_size = buf_size;
  hdr->alloc_count = 0;
  hdr->pool_size = pool_size;
  hdr->pool_count = 0;
  hdr->pool = zmalloc (sizeof (void *) * pool_size);
  return hdr;
}

static inline void
mempool_destroy (mempool_hdr_t * hdr)
{
  assert (hdr->alloc_count == 0);
  while (hdr->pool_count-- > 0)
    {
      zfree (hdr->pool[hdr->pool_count]);
    }
  zfree (hdr->pool);
  zfree (hdr);
}

static inline void *
mempool_alloc (mempool_hdr_t * hdr)
{
  hdr->alloc_count++;
  if (hdr->pool_count > 0)
    {
      return hdr->pool[--hdr->pool_count];
    }
  return zmalloc (hdr->buf_size);
}

static inline void
mempool_free (mempool_hdr_t * hdr, void *buf)
{
  assert (hdr->alloc_count > 0);
  hdr->alloc_count--;
  if (hdr->pool_count < hdr->pool_size)
    {
      hdr->pool[hdr->pool_count++] = buf;
      return;
    }
  zfree (buf);
  return;
}
#endif
