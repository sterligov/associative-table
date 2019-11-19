#include "28_mem.h"
#include <stdlib.h>

#ifdef LIST_MEMORY
// обычное выделение с помощью списка
void *Mem::allocMem(size_t sz)
{
	if (lastBlock == NULL)
	{
		if (addMem() == NULL) { throw Mem::memoryError("Not enough memory\n"); }
	}
	head *cur, *find;
	size_t nbytes = sz; // требуемая память
	size_t difference = this->size(); // разность между требуемым блоком и размером текущего блока
	find = NULL;
	cur = lastBlock->next;
	do 
	{
		if ( (cur->size >= nbytes) && (cur->size - nbytes) < difference )
		{
			difference = cur->size - nbytes;
			find = cur;
		}
		cur = cur->next;
	} while (cur != lastBlock->next);
	
	if (!find) 
	{
		if ( Mem::addMem() == NULL ) { throw Mem::memoryError("Not enough memory\n"); } // lastBlock изменился
		else {return allocMem(sz);}
	} 

	lastBlock = find->prev;
	if (difference <= HEADSIZE) // нельзя поделить на два блока
	{
		if (find->next == find) 
		{ 
			lastBlock = NULL; 
		}
		else 
		{
			find->prev->next = find->next;
			find->next->prev = find->prev;
		}
	} 
	else // отрежем конец 
	{
		find->size -= (nbytes + HEADSIZE);
		char *tmpFind;
		tmpFind = (char *)find;
		tmpFind += (find->size + HEADSIZE);
		find = (head *)tmpFind;
		find->size = nbytes;
	}
	return (void *)(find + 1);
}

void Mem::freeMem(void *ptr)
{
	if (!ptr) return;
	head *del, *cur;
	del = (head *)ptr - 1; // указатель на заголовок
	if (lastBlock == NULL) { lastBlock = del; lastBlock->next = lastBlock->prev = lastBlock; return; }

	for (cur = lastBlock; !( (del >= cur) && (del <= cur->next) ); cur = cur->next) // ищем место удаляемого блока
	{
		if ( (cur >= cur->next) && (del >= cur || del <= cur->next) ) // блок в начале или в конце
			break;
 	}

	if (cur == cur->next && cur->prev == cur)
	{
		if (del > cur)
		{
			if ((char *)cur + cur->size + HEADSIZE == (char *)del)
			{
				cur->size += (HEADSIZE + del->size);
			}
			else
			{
				cur->next = del;
				del->next = cur;
				del->prev = cur;
				cur->prev = del;
			}
			lastBlock = cur;
		}
		else
		{
			if ((char *)del + del->size + HEADSIZE == (char *)cur)
			{
				del->size += (HEADSIZE + cur->size);
				del->next = del->prev = del;
			}
			else
			{
				del->next = cur;
				cur->prev = del;
				cur->next = del;
				del->prev = cur;
			}
			lastBlock = del;
		}
	}
	else
	{
		if ( ((char *)del + del->size + HEADSIZE) == (char *)cur->next ) // сольем с нижним блоком
		{
			del->size += (cur->next->size + HEADSIZE);
			cur->next->next->prev = del;
			del->next = cur->next->next; 
		}
 		else {del->next = cur->next; cur->next->prev = del;}

		if ( ((char *)cur + cur->size + HEADSIZE) == (char *)del ) // с верхним
		{
			cur->size += (del->size + HEADSIZE);
			cur->next = del->next;
			del->next->prev = cur;
			/*del->next->prev = cur->next;
			cur->next = del->next;*/
		}
		else {cur->next = del; del->prev = cur;}
		
		lastBlock = cur;
	}

	if (cur->size == size() - HEADSIZE && ifNewPiece == 0 && (char *)cur != buf)
	{
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
		if (cur == cur->next && cur->prev == cur) { lastBlock = NULL; }
		else {lastBlock = cur->prev;}
		delete cur;
		--lastFreePiece;
	}
	else if (del->size == size() - HEADSIZE && ifNewPiece == 0 && (char *)del != buf)
	{
		del->prev->next = del->next;
		del->next->prev = del->prev;
		if (del == del->next && del->prev == del) {lastBlock = NULL;}
		else lastBlock = del->prev;
		delete del;
		--lastFreePiece;
	}
}

void *Mem::addMem() 
{
	if (lastFreePiece >= numPiece) { throw Mem::memoryError("Not enough memory\n"); }
	char *tmpBuf;
	tmpBuf = new char[size()];
	head *tmp = (head *)tmpBuf;
	tmp->size = size() - HEADSIZE;
	tmp->next = tmp->prev = tmp;
	++lastFreePiece;
	ifNewPiece = 1;
	freeMem(tmp + 1);
	ifNewPiece = 0;
	return lastBlock;
}
#else if
// выделение алгоритмом маркировки
void *Mem::allocMem(size_t sz)
{
	if (lastBlock == NULL)
	{
		if (addMem() == NULL) { return NULL; }
	}
	head *cur, *find;
	size_t nbytes = sz + END_SIZE + 2*MARKER_SIZE; // требуемая память
	size_t difference = this->size(); // разность между требуемым блоком и размером текущего блока
	find = NULL;
	cur = lastBlock;
	do 
	{
		if ( (cur->size >= nbytes) && (cur->size - nbytes) < difference )
		{
			difference = cur->size - nbytes;
			find = cur;
		}
		cur = cur->next;
	} while (cur != lastBlock);
	
	if (!find) 
	{
		if ( Mem::addMem() == NULL ) {return NULL;} // lastBlock изменился
		else {return allocMem(sz);}
	} 

	if (difference <= HEADSIZE) // нельзя поделить на два блока
	{
		if (find->next == find) 
		{ 
			lastBlock = NULL; 
		}
		else 
		{
			find->prev->next = lastBlock = find->next;
			find->next->prev = find->prev;
		}
		
		unsigned char * tmp;
		tmp = (unsigned char *)find - MARKER_SIZE;
		*tmp = BUSY_MARKER;
		tmp += (HEADSIZE + find->size + MARKER_SIZE);
		*((size_t *)tmp) = find->size;
		*(tmp + END_SIZE) = BUSY_MARKER;
	} 
	else // отрежем конец 
	{
		lastBlock = find;
		unsigned char *tmpFind;
		find->size -= (nbytes + HEADSIZE);
		tmpFind = (unsigned char *)find;
		tmpFind += (HEADSIZE + find->size);
		*((size_t *)tmpFind) = find->size;
		*(tmpFind + END_SIZE) = FREE_MARKER;
		
		tmpFind += (END_SIZE + MARKER_SIZE);
		*tmpFind = BUSY_MARKER;
		find = (head *)(tmpFind + 1);
		tmpFind += (MARKER_SIZE + HEADSIZE + sz);
		*((size_t *)tmpFind) = sz;
		*(tmpFind + END_SIZE) = BUSY_MARKER;
		find->size = sz;
	}
	return (void *)(find + 1);
}

void Mem::freeMem(void *ptr)
{
	if (!ptr) return;
	head *del, *cur, *prev;
	head *left, *right;
	del = (head *)ptr - 1; // указатель на заголовок
//	if (lastBlock == NULL) { lastBlock = del; lastBlock->next = lastBlock->prev = lastBlock; return; 



	unsigned char *mark;
	mark = (unsigned char *)del;
	mark += (HEADSIZE + del->size + END_SIZE);
	*mark = FREE_MARKER;
	if (*(mark + MARKER_SIZE) == FREE_MARKER)
	{
		right = (head *)(mark + 2*MARKER_SIZE);
	}
	else { right = NULL; }

	mark = (unsigned char * )del - 1;
	*mark = FREE_MARKER;
	if (*(mark - 1) == FREE_MARKER)
	{
		mark -= (END_SIZE + MARKER_SIZE);
		size_t *leftSz;
		leftSz = (size_t *)mark;
		mark -= (HEADSIZE + *leftSz);
		left = (head *)mark;
	}
	else { left = NULL; }
	if (lastBlock == NULL) { lastBlock = del; lastBlock->next = lastBlock->prev = lastBlock; return; }

	if (left != NULL && right != NULL)
	{
		mark = (unsigned char *)left;
		left->size += (END_SIZE + 2*MARKER_SIZE + HEADSIZE + del->size + END_SIZE + 2*MARKER_SIZE + HEADSIZE + right->size);
		mark += (HEADSIZE + left->size);
		*((size_t *)mark) = left->size;

		if (right->prev != right || right->next != right)
		{
			right->prev->next = right->next;
			right->next->prev = right->prev;
		}

		if (left->size == size() - HEADSIZE - 2*MARKER_SIZE - END_SIZE && ifNewPiece == 0 && (unsigned char *)left != buf)
		{
			left->prev->next = left->next;
			left->next->prev = left->prev;
			if (left->next == left && left == left->prev) { lastBlock = NULL; }
			else lastBlock = left->prev;
			delete ((char *)left - 1);
			--lastFreePiece;
		}

	}
	else if (left != NULL)
	{
		mark = (unsigned char *)left;
		left->size += (END_SIZE + 2*MARKER_SIZE + HEADSIZE + del->size);
		mark += (HEADSIZE + left->size);
		*((size_t *)mark) = left->size;

		
		if (left->size == size() - HEADSIZE - 2*MARKER_SIZE - END_SIZE && ifNewPiece == 0 && (unsigned char *)left != buf)
		{
			left->prev->next = left->next;
			left->next->prev = left->prev;
			if (left->next == left && left == left->prev) { lastBlock = NULL; }
			else lastBlock = cur->prev;
			delete ((char *)left - 1);
			--lastFreePiece;
		}
	}
	else if (right != NULL)
	{
		mark = (unsigned char *)del;
		del->size += (END_SIZE + 2*MARKER_SIZE + HEADSIZE + right->size);
		mark += (HEADSIZE + del->size);
		*((size_t *)mark) = del->size;
		
		if (right->next == right && right->prev == right)
		{
			del->next = del->prev = del;
		}
		else 
		{
			right->prev->next = del;
			right->next->prev = del;
			del->prev = right->prev;
			del->next = right->next;
		}


		if (del->size == size() - HEADSIZE - 2*MARKER_SIZE - END_SIZE && ifNewPiece == 0 && ((unsigned char * )del - 1) != buf)
		{
			del->prev->next = del->next;
			del->next->prev = del->prev;
			if (del->next == del && del->prev == del) {lastBlock = NULL;}
			else lastBlock = del->prev;
			delete ((unsigned char *)del - 1);
			--lastFreePiece;
		}
	}
	else
	{
		if (ifNewPiece == 1) 
		{ 
			lastBlock = del; lastBlock->prev = lastBlock->next = lastBlock; 
		}
		else
		{
			head *prev;
			prev = lastBlock->prev;
			del->prev = prev;
			del->next = lastBlock;
			lastBlock->prev = del;
			prev->next = del;
		}

		if (del->size == size() - HEADSIZE - 2*MARKER_SIZE - END_SIZE && ifNewPiece == 0 && ((unsigned char * )del - 1) != buf)
		{
			del->prev->next = del->next;
			del->next->prev = del->prev;
			if (del->next == del && del->prev == del) {lastBlock = NULL;}
			else lastBlock = del->prev;
			delete ((unsigned char *)del - 1);
			--lastFreePiece;
		}
	}

}

void *Mem::addMem() 
{
	if (lastFreePiece >= numPiece) { return NULL; }
	unsigned char *tmpBuf;
	tmpBuf = new unsigned char[size()];
	*tmpBuf = BUSY_MARKER;
	head *tmp = (head *)(tmpBuf + 1);
	tmp->size = size() - HEADSIZE - 2*MARKER_SIZE - END_SIZE;
	tmp->next = tmp->prev = tmp;
	tmpBuf += (MARKER_SIZE + HEADSIZE + tmp->size);
	*((size_t *)tmpBuf) = tmp->size;
	*(tmpBuf + END_SIZE) = BUSY_MARKER;
	++lastFreePiece;
	ifNewPiece = 1;
	freeMem(tmp + 1);
	ifNewPiece = 0;
	return lastBlock;
}

#endif



