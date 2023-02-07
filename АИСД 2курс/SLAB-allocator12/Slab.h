#pragma once

struct MEMPART
{
  MEMPART *next;
  MEMPART *prev;
};

struct SLAB
{
  SLAB *next; //��������� slab
  SLAB *prev; //���������� slab
  size_t size; //������
  MEMPART *memPart; 
  bool isSlabMarked; //���� ���������� ������ ��� ����� 1 - true
};

struct cache
{
  SLAB *free; //list of free slabs
  SLAB *partlyFree; //list of partial slabs
  SLAB *booked; //list of full slabs
  size_t object_size; //size of one object
  int slab_order; //buddy allocat
  size_t slab_objects; //���������� �������� � ����� �����
  cache *next;
};

void cache_setup(cache *cache, size_t object_size); //�������������
void cache_release(cache *cache); //������������ cache
void *cache_alloc(cache *cache); // Allocate one object from cache
void cache_free(cache *cache, void *ptr); // Deallocate one object from cache
void cache_shrink(cache *cache); // Shrink cache
void cache_info(cache* cache); 
void cache_info_infile(cache* cache, int k);
void slab_info(cache* cache);
void cache_allInfo(cache* cache);