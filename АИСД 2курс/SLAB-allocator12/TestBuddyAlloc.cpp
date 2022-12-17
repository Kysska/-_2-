#include <cstdlib>

//Аллоцирует участок размером 4096 * 2^order байт

void *alloc_slab(int order)
{
  return _aligned_malloc(1 << (order + 12), 1 << (order + 12));
}

void free_slab(void *slab)
{
  _aligned_free(slab);
}