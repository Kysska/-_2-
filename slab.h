#ifndef _SLAB_H_
#define _SLAB_H_


#include <stdlib.h>

typedef struct kmem_cache_s kmem_cache_t;
#define BLOCK_SIZE (4096)
#define CACHE_LINE_SIZE (64)

void kmem_initialization(void *space, int block_num);

kmem_cache_t *kmem_cache_create(const char *name, size_t size); 

void *kmem_cache_alloc(kmem_cache_t *cachone); 

void kmem_cache_free(kmem_cache_t *cachone, void *object1); 

void *kmalloc(size_t size);

void kfree(const void *object1);

void kmem_cache_destroy(kmem_cache_t *cachep); 

void kmem_cache_info(kmem_cache_t *cachep);



#endif