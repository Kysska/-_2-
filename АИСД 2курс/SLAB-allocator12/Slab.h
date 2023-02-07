#pragma once

struct MEMPART
{
  MEMPART *next;
  MEMPART *prev;
};

struct SLAB
{
  SLAB *next; //следующая slab
  SLAB *prev; //предыдущий slab
  size_t size; //размер
  MEMPART *memPart; 
  bool isSlabMarked; //если количество меньше или равно 1 - true
};

struct cache
{
  SLAB *free; //list of free slabs
  SLAB *partlyFree; //list of partial slabs
  SLAB *booked; //list of full slabs
  size_t object_size; //size of one object
  int slab_order; //buddy allocat
  size_t slab_objects; //количество объектов в одной плите
  cache *next;
};

void cache_setup(cache *cache, size_t object_size); //инициализация
void cache_release(cache *cache); //освобождение cache
void *cache_alloc(cache *cache); // Allocate one object from cache
void cache_free(cache *cache, void *ptr); // Deallocate one object from cache
void cache_shrink(cache *cache); // Shrink cache
void cache_info(cache* cache); 
void cache_info_infile(cache* cache, int k);
void slab_info(cache* cache);
void cache_allInfo(cache* cache);