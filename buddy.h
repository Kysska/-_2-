#ifndef _BUDDY_H_
#define _BUDDY_H_

void buddy_initialization(void *space, int block_num);	

void* buddy_alloc(int sz);	

void buddy_free(void *space, int sz);	

void buddy_print();		


#endif