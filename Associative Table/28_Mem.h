#pragma once
#include "MemoryManager.h"
#include <stdio.h>
#include <string.h>
#define LIST_MEMORY
//#define MARKER_MEMORY

#ifdef LIST_MEMORY
typedef struct head // заголовок свободного блока
{
	size_t size;
	head *next, *prev;
} head;

#define HEADSIZE sizeof(head) // за еденицу памяти будет выступать размер заголовка


class Mem: public MemoryManager 
{
	head *lastBlock; // последний найденый блок 
	char *buf;
	int lastFreePiece; // номер последнего выделенного куска
	int numPiece; // количество кусков
	int ifNewPiece;
public:
	struct memoryError
    {
        char msg[256];
        memoryError(const char* err_msg) { strcpy(msg,err_msg); }
    };

	Mem(size_t sz, int numOfPiece) : MemoryManager(sz) 
	{
		if (sz <= 0 || numOfPiece <= 0)
		{ 
			throw memoryError("Error: size of memory manager and number of piece, must be more then 0\n"); 
		}
		ifNewPiece = 1;
	//	if (sz < MIN_SIZE) {sz = MIN_SIZE;}
		numPiece = numOfPiece;
		lastFreePiece = 1;
		buf = new char[sz]; // первый кусок
		lastBlock = (head *)buf;
		lastBlock->size = sz - HEADSIZE;
		lastBlock->next = lastBlock->prev = lastBlock;
	}

	void *allocMem(size_t sz);

	void freeMem(void *ptr);

private:
	void *addMem();
};
#else if
#define FREE_MARKER '0'
#define BUSY_MARKER '1'
#define MARKER_SIZE sizeof(unsigned char)
#define END_SIZE sizeof(size_t)

typedef struct head // заголовок свободного блока
{
	size_t size;
	head *next, *prev;
} head;

#define HEADSIZE sizeof(head) // за еденицу памяти будет выступать размер заголовка



class Mem: public MemoryManager 
{
	head *lastBlock; // последний найденый блок 
	int lastFreePiece; // индекс последнего куска
	int numPiece; // количество кусков
	int ifNewPiece;
public:
	struct memoryError
    {
        char msg[256];
        memoryError(const char* err_msg) { strcpy(msg,err_msg); }
    };
	unsigned char *buf;
	Mem(size_t sz, int numOfPiece) : MemoryManager(sz) 
	{
		ifNewPiece = 1;
	//	if (sz < MIN_SIZE) {sz = MIN_SIZE;}
		numPiece = numOfPiece;
		lastFreePiece = 1;
		buf = new unsigned char[sz]; // первый кусок

		unsigned char *tmp;
		tmp = buf;
		*tmp = BUSY_MARKER;
		lastBlock = (head *)(tmp + MARKER_SIZE);
		lastBlock->size = sz - HEADSIZE - END_SIZE - 2*MARKER_SIZE;
		lastBlock->next = lastBlock->prev = lastBlock;
		tmp += (MARKER_SIZE + HEADSIZE + lastBlock->size);
		*((size_t *)tmp) = lastBlock->size;
		*(tmp + END_SIZE) = BUSY_MARKER;
	}

	void *allocMem(size_t sz);

	void freeMem(void *ptr);

private:
	void *addMem();
};
#endif