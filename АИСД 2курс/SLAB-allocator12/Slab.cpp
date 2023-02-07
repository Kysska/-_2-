#include <algorithm>
#include "Slab.h"
#include "BuddyAlloc.h"
#include <iostream>
#include <fstream>
#include <string>

/**
 * Функция инициализации
 *  - cache - структура, которую инициализируем
 *  - object_size - размер объектов, которые должен
 *    аллоцировать этот кеширующий аллокатор
 **/

void cache_setup(cache* cache, size_t object_size)
{
    cache->free = NULL; //свободный
    cache->partlyFree = NULL; //частично занятый
    cache->booked = NULL; //занятый
    cache->object_size = std::max(sizeof(MEMPART), object_size); //размаер объекта в кеше
    //cache->slab_order = 0;
   /* cache->slab_order = std::min(10, nearest_pow2(object_size) - 5);*/ //используемый размер slab
    int order = 0;
    int count = 0;
    for (order = 0; order <= 10; order++)
    {
        count = ((1 << (12 + order)) - sizeof(SLAB)) / (object_size);
        //std::cout << count << std::endl;
        if (count > 10)
        {
            cache->slab_order = order;
            cache->slab_objects = count;
            return;
        }
    }
    order--;
    if (count) {
        cache->slab_order = order;
        cache->slab_objects = count;
    }
    else {
        //std::cout << "Setup is failed" << std::endl;
        cache->slab_order = 0;
        cache->slab_objects = 0;
    }
    //cache->slab_objects = (((size_t)1 << (12 + cache->slab_order)) - sizeof(SLAB)) / cache->object_size; //количество объектов в одном slab 
}
void FreeSlabsFromList(SLAB* slab)
{
    while (slab != NULL)
    {
        SLAB* tmp = slab;
        slab = slab->next;
        free_slab((void*)tmp);
    }
}

// Функция освобождения
// Она должна освободить всю память занятую аллокатором.

void cache_release(cache* cache)
{
    FreeSlabsFromList(cache->free);
    cache->free = NULL;

    FreeSlabsFromList(cache->partlyFree);
    cache->partlyFree = NULL;

    FreeSlabsFromList(cache->booked);
    cache->booked = NULL;
}
bool CreateNewSlab(cache* cache)
{
    SLAB* tmp = (SLAB*)alloc_slab(cache->slab_order);
    if (tmp == NULL)
        return false;

    tmp->size = 0;
    if (cache->slab_objects > 1)
        tmp->isSlabMarked = false;
    else
        tmp->isSlabMarked = true;

    if (cache->slab_objects > 0)
    {
        tmp->memPart = (MEMPART*)(tmp + 1);
        tmp->memPart->next = NULL;
        tmp->memPart->prev = NULL;
    }
    else
    {
        tmp->memPart = NULL;
    }

    tmp->next = cache->free;
    tmp->prev = NULL;
    cache->free = tmp;

    return true;
}

void* AllocFromSlab(cache* cache, SLAB* slab)
{
    if (slab->memPart == NULL)
        return NULL;

    if (slab->memPart->next == NULL && slab->isSlabMarked == false)
    {
        slab->memPart->next = (MEMPART*)((size_t)(slab->memPart) + cache->object_size);
        slab->memPart->next->next = NULL;
        slab->memPart->next->prev = slab->memPart;
        if ((slab->size + 2) == cache->slab_objects)
            slab->isSlabMarked = true;
    }

    slab->size++;
    MEMPART* tmp = slab->memPart;
    slab->memPart = slab->memPart->next;
    if (slab->memPart != NULL)
        slab->memPart->prev = NULL;

    return (void*)tmp;
}

void MoveFirstElement(SLAB** from, SLAB** to)
{
    SLAB* tmp = *from;
    if (tmp->next != NULL)
        tmp->next->prev = NULL;
    *from = tmp->next;
    tmp->next = *to;
    if (*to != NULL)
        (*to)->prev = tmp;
    *to = tmp;
}
//Функция аллокации памяти из кеширующего аллокатора.
void* cache_alloc(cache* cache)
{
    if (cache->partlyFree != NULL)
    {
        void* ptr = AllocFromSlab(cache, cache->partlyFree);
        if (ptr == NULL)
            return NULL;

        if (cache->partlyFree->size == cache->slab_objects)
            MoveFirstElement(&cache->partlyFree, &cache->booked);

        return ptr;
    }
    else if (cache->free != NULL)
    {
        void* ptr = AllocFromSlab(cache, cache->free);
        if (ptr == NULL)
            return NULL;

        if (cache->free->size == cache->slab_objects)
            MoveFirstElement(&cache->free, &cache->booked);
        else
            MoveFirstElement(&cache->free, &cache->partlyFree);

        return ptr;
    }
    else
    {
        if (CreateNewSlab(cache) == false)
            return NULL;

        void* ptr = AllocFromSlab(cache, cache->free);
        if (ptr == NULL)
            return NULL;

        if (cache->free->size == cache->slab_objects)
            MoveFirstElement(&cache->free, &cache->booked);
        else
            MoveFirstElement(&cache->free, &cache->partlyFree);

        return ptr;
    }
}

void MoveSlab(SLAB* slab, SLAB** from, SLAB** to)
{
    if (slab->prev != NULL)
        slab->prev->next = slab->next;
    if (slab->next != NULL)
        slab->next->prev = slab->prev;

    if (*from == slab)
        *from = slab->next;

    slab->prev = NULL;
    slab->next = *to;
    if (*to != NULL)
        (*to)->prev = slab;
    *to = slab;
}

size_t GetNLowBits(size_t x, size_t n)
{
    return x & (~((1 << n) - 1));
}
//Функция освобождения памяти назад в кеширующий аллокатор.
void cache_free(cache* cache, void* ptr)
{
    SLAB* slab = (SLAB*)GetNLowBits((size_t)ptr, (size_t)(cache->slab_order + 12));
    MEMPART* memPart = (MEMPART*)ptr;
    memPart->prev = NULL;
    memPart->next = slab->memPart;
    if (slab->memPart != NULL)
        slab->memPart->prev = memPart;
    slab->memPart = memPart;

    if (slab->size == 1)
    {
        if (cache->slab_objects == 1)
            MoveSlab(slab, &cache->booked, &cache->free);
        else
            MoveSlab(slab, &cache->partlyFree, &cache->free);
    }
    else if (slab->size == cache->slab_objects)
    {
        if (cache->slab_objects == 1)
            MoveSlab(slab, &cache->booked, &cache->free);
        else
            MoveSlab(slab, &cache->booked, &cache->partlyFree);
    }

    slab->size--;
}
//Функция должна освободить все SLAB, которые не содержат занятых объектов.
void cache_shrink(cache* cache)
{
    FreeSlabsFromList(cache->free);
    cache->free = NULL;
}
static cache* allCaches = nullptr;
void slab_print(SLAB* slab)
{
    std::cout << "\t\t->Info slab:" << std::endl;
    std::cout << "adress:\t" << slab << std::endl;
    std::cout << "next slab:\t" << slab->next << std::endl;
    std::cout << "prev slab:\t" << slab->prev << std::endl;
    std::cout << "size:\t\t" << slab->size << std::endl;
}

void cache_info(cache* cache) {
    if (cache == nullptr)
    {
        std::cout << "NullPointer passed as argument" << std::endl;
        return;
    }
    int i = 0;

    SLAB* s = cache->free;
    while (s != nullptr)
    {
        i++;
        s = s->next;
    }

    s = cache->partlyFree;
    while (s != nullptr)
    {
        i++;
        s = s->next;
    }

    s = cache->booked;
    while (s != nullptr)
    {
        i++;
        s = s->next;

    }

    unsigned int cacheSize = i * (1 << cache->slab_order);

    std::cout << "*** CACHE INFO: ***" << std::endl
        << "adress:\t" << cache << std::endl
        << "booked:\t" << cache->booked << std::endl
        << "partlyfree:\t" << cache->partlyFree << std::endl
        << "free:\t" << cache->free << std::endl
        << "Size of one object (in bytes):\t" << cache->object_size << std::endl
        << "Size of cache (in blocks):\t" << cacheSize << std::endl
        << "Number of slabs:\t\t" << i << std::endl
        << "Number of objects in one slab:\t" << cache->slab_objects << std::endl
        << "Slab order\t" << cache->slab_order << std::endl;


}

void cache_info_infile(cache* cache, int k) {
    std::ofstream fout("./tests/" + std::to_string(k) + ".out");
    if (cache == nullptr)
    {
        fout << "NullPointer passed as argument" << std::endl;
        return;
    }
    int i = 0;

    SLAB* s = cache->free;
    while (s != nullptr)
    {
        i++;
        s = s->next;
    }

    s = cache->partlyFree;
    while (s != nullptr)
    {
        i++;
        s = s->next;
    }

    s = cache->booked;
    while (s != nullptr)
    {
        i++;
        s = s->next;

    }

    unsigned int cacheSize = i * (1 << cache->slab_order);

    fout << "*** CACHE INFO: ***" << std::endl
        << "adress:\t" << cache << std::endl
        << "booked:\t" << cache->booked << std::endl
        << "partlyfree:\t" << cache->partlyFree << std::endl
        << "free:\t" << cache->free << std::endl
        << "Size of one object (in bytes):\t" << cache->object_size << std::endl
        << "Size of cache (in blocks):\t" << cacheSize << std::endl
        << "Number of slabs:\t\t" << i << std::endl
        << "Number of objects in one slab:\t" << cache->slab_objects << std::endl
        << "Slab order\t" << cache->slab_order << std::endl;

    fout.close();
}


void slab_info(cache* cache) {
    if (cache == nullptr)
    {
        std::cout << "NullPointer passed as argument" << std::endl;
        return;
    }
    SLAB* s = cache->free;
    while (s != nullptr)
    {
        slab_print(s);
        s = s->next;
    }

    s = cache->partlyFree;
    while (s != nullptr)
    {
        slab_print(s);
        s = s->next;
    }

    s = cache->booked;
    while (s != nullptr)
    {
        slab_print(s);
        s = s->next;

    }

}


void cache_allInfo(cache* cache)
{
    while (cache != nullptr)
    {
        cache_info(cache);
        std::cout << std::endl;
        cache = cache->next;
    }
}