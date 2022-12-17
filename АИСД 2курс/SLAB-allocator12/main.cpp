#include <cstdlib>
#include <algorithm>
#include "Slab.h"


int main(void)
{
  cache cache;
  void *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;

  cache_setup(&cache, 2000000);
  cache_info(&cache);
  ptr1 = cache_alloc(&cache);
  cache_info(&cache);
  ptr2 = cache_alloc(&cache);
  cache_info(&cache);
  ptr3 = cache_alloc(&cache);
  cache_info(&cache);
  cache_free(&cache, ptr1);
  cache_info(&cache);
  ptr1 = cache_alloc(&cache);
  cache_info(&cache);
  ptr4 = cache_alloc(&cache);  
  //cache_allInfo(&cache);
  //ptr5 = cache_alloc(&cache);
  cache_info(&cache);
  cache_free(&cache, ptr1);
  cache_free(&cache, ptr2);
  cache_free(&cache, ptr3);
  cache_info(&cache);
  cache_release(&cache);


  return 1;
}