#include "28_Table.h"
#include "28_Mem.h"

size_t Table::hash_function(void *key, size_t keySize)
{
	unsigned char *tmpKey;
	tmpKey = (unsigned char *)key;
	unsigned int hash = 0;
	unsigned int i;
 
	for (i = 0; i < keySize; tmpKey++, i++) 
	{
		hash += (*tmpKey);
		hash -= (hash << 13) | (hash >> 19);
	}
	return hash % HASHSIZE;
}

void Table::reHash()
{
	int oldSize = HASHSIZE;
	HASHSIZE *= 2;
	element *oldArray;
	oldArray = AssociativeArray;
	#ifdef USER_MEMORY
		AssociativeArray = (element *)_memory.allocMem(HASHSIZE * sizeof(element));
	#else if
		AssociativeArray = new element[HASHSIZE];
	#endif
	for (int i = 0; i < HASHSIZE; i++)
	{
		AssociativeArray[i].elem = NULL;
		AssociativeArray[i].key = NULL;
		AssociativeArray[i].deleted = false;
	}
	for (int i = 0; i < oldSize; i++)
	{
		insertByKey(oldArray[i].key, oldArray[i].keySize, oldArray[i].elem, oldArray[i].elemSize);
		#ifdef USER_MEMORY
			_memory.freeMem(oldArray[i].elem);
			_memory.freeMem(oldArray[i].key);
		#else if
			delete oldArray[i].elem;
			delete oldArray[i].key;
		#endif
	}
	#ifdef USER_MEMORY
		_memory.freeMem(oldArray);
	#else if
		delete oldArray;
	#endif
}


int Table::insertByKey(void* key, size_t keySize, void* elem, size_t elemSize)
{
	if (!key || !elem) { return -1; }
	int index = hash_function(key, keySize);
	int i;
	for (i = index; AssociativeArray[i].elem != NULL; i++)
	{
		if (i == HASHSIZE) { i = 0; } 
		if ((i + 1) == index || (i == HASHSIZE - 1 && index == 0)) 
		{ 
			reHash();  
			insertByKey(key, keySize, elem, elemSize);
			return 0;
		} // нет места
	}
	// добавим элемент
	try
	{
		#ifdef USER_MEMORY
			AssociativeArray[i].elem = (void *)_memory.allocMem(elemSize);
			AssociativeArray[i].key = (void *)_memory.allocMem(keySize);
		#else if
			AssociativeArray[i].elem = new char[elemSize];
			AssociativeArray[i].key = new char[keySize];
		#endif
	}
	catch (Mem::memoryError err)
	{
		cout << err.msg;
	}
	AssociativeArray[i].elemSize = elemSize;
	AssociativeArray[i].keySize = keySize;
	memcpy(AssociativeArray[i].elem, elem, elemSize);
	memcpy(AssociativeArray[i].key, key, keySize);
	AssociativeArray[i].deleted = false;
	++capacity;
	return 0;
}

void Table::removeByKey(void* key, size_t keySize)
{
	if (key == NULL || keySize <= 0) { return; }
	int index = hash_function(key, keySize);
	for (int i = index ; ; i++)
	{
		if (i == HASHSIZE) { i = 0; } 
		if (!AssociativeArray[i].elem)
		{
			if (AssociativeArray[i].deleted == false) { return; } // не нашли
		}
		else if (!memcmp(AssociativeArray[i].key, key, keySize) && AssociativeArray[i].keySize == keySize)
		{   
			#ifdef USER_MEMORY
				_memory.freeMem(AssociativeArray[i].elem);
				_memory.freeMem(AssociativeArray[i].key);
			#else if
				delete AssociativeArray[i].elem;
				delete AssociativeArray[i].key;
			#endif
			AssociativeArray[i].elem = NULL;
			AssociativeArray[i].key = NULL;
			AssociativeArray[i].deleted = true;
			--capacity;
			return;
		}
		if ((i + 1) == index || (i == HASHSIZE - 1 && index == 0)) { return; } // не нашли
	}
}

void* Table::at(void* key, size_t keySize, size_t &valueSize)
{
	if (key == NULL || keySize <= 0) {return NULL;}
	int index = hash_function(key, keySize);
	for (int i = index; ; i++)
	{
		if (i == HASHSIZE) { i = 0; } 
		if (!AssociativeArray[i].elem)
		{
			if (AssociativeArray[i].deleted == false) 
			{ 
				valueSize = 0;
				return NULL; 
			} // не нашли
		}
		else if (!(memcmp(AssociativeArray[i].key, key, keySize)) && (AssociativeArray[i].keySize == keySize))
		{
			valueSize = AssociativeArray[i].elemSize; 
			return AssociativeArray[i].elem;
		}

		if ((i + 1) == index || (i == HASHSIZE - 1 && index == 0))  
		{ 
			valueSize = 0;
			return NULL; 
		} // не нашли
	}

}

void Table::remove(Container::Iterator *iter)
{
	int index;
	element *removeElem;
	Table::Iterator *it;
	it = (Table::Iterator *)iter;
	removeElem = it->getItr(index);
	if (!removeElem) { return; }
	iter->goToNext();

	#ifdef USER_MEMORY
		_memory.freeMem(AssociativeArray[index].elem);
		_memory.freeMem(AssociativeArray[index].key);
	#else if
		delete AssociativeArray[index].elem;
		delete AssociativeArray[index].key;
	#endif
	AssociativeArray[index].elem = NULL;
	AssociativeArray[index].key = NULL;
	AssociativeArray[index].deleted = true;
}

Container::Iterator* Table::findByKey(void* key, size_t keySize)
{
	if (!key || keySize <= 0) {return NULL;}
	int index = hash_function(key, keySize);
	if (AssociativeArray[index].elem == NULL && AssociativeArray[index].deleted == false) 
		return NULL; // не нашли
	for (int i = index ; ; i++)
	{
		if (i == HASHSIZE) { i = 0; } 
		if (!AssociativeArray[i].elem)
		{
			if (AssociativeArray[i].deleted == false) { return NULL; } // не нашли
		}
		else if (!(memcmp(AssociativeArray[i].key, key, keySize)) && (AssociativeArray[i].keySize == keySize))
		{
			Table::Iterator *it;
			it = new Table::Iterator(this);
			it->setElement(&AssociativeArray[i], i);
			return it;
		}
		if ((i + 1) == index || (i == HASHSIZE - 1 && index == 0))  { return NULL; } // не нашли
	}
}

Container::Iterator* Table::newIterator()
{
	Table::Iterator *itr;
	itr = new Table::Iterator(this);
	return itr;
}

Container::Iterator* Table::begin()
{
	for (int i = 0; i < HASHSIZE; i++)
	{
		if (AssociativeArray[i].elem != NULL)
		{
			Table::Iterator *it;
			it = new Table::Iterator(this);
			it->setElement(&AssociativeArray[i], i); 
			return it;
		}
	}
	return NULL;
}

Container::Iterator* Table::end()
{
	int index = -1;
	for (int i = 0; i < HASHSIZE; i++)
	{
		if (AssociativeArray[i].elem != NULL) { index = i; }
	}

	if (index == -1) { return NULL; } // пустая таблица
	else
	{
		Table::Iterator *it;
		it = new Table::Iterator(this);
		it->setElement(&AssociativeArray[index], index); 
		return it;
	}
}

bool Table::empty() 
 {
	 if (!capacity) return true;
	 else return false;
 }

Container::Iterator* Table::find(void* elem, size_t size)
{
	for (int i = 0; i < HASHSIZE; i++)
	{
		if (AssociativeArray[i].elem != NULL)
		{
			if (!memcmp(AssociativeArray[i].elem, elem, size) && AssociativeArray[i].elemSize == size)
			{
				Table::Iterator *it;
				it = new Table::Iterator(this);
				it->setElement(&AssociativeArray[i], i); 
				return it;
			}
		}
	}
	return NULL; // не нашли
}

void Table::clear() 
{
	int j = 0;
	for (int i = 0; i < HASHSIZE; i++)
	{
		if (AssociativeArray[i].elem != NULL)
		{
			++j;
			#ifdef USER_MEMORY
				_memory.freeMem(AssociativeArray[i].elem);
				_memory.freeMem(AssociativeArray[i].key);
			#else if
				delete AssociativeArray[i].elem;
				delete AssociativeArray[i].key;
			#endif
			AssociativeArray[i].elem = NULL;
			AssociativeArray[i].key = NULL;
			--capacity;
		}
		AssociativeArray[i].deleted = false;
	}
}

void *Table::Iterator::getElement(size_t &size)
{
	if (itrElem == NULL)
	{
		size = 0;
		return NULL;
	}
/*	else if (itrElem->elem == NULL)
	{
		size = 0;
		return NULL;
	} */
	else
	{
		size = itrElem->elemSize;
		return itrElem->elem;
	}
}

bool Table::Iterator::hasNext()
{
	int index = this->curIndex;
	for (int i = index + 1; i < tableIt->HASHSIZE; i++)
	{
		if (tableIt->AssociativeArray[i].elem != NULL) { return true; }
	}
	return false;
}

void Table::Iterator::goToNext()
{
	int index = this->curIndex;
	itrElem = NULL; 
	curIndex = -1;
	for (int i = index + 1; i < tableIt->HASHSIZE; i++)
	{
		if (tableIt->AssociativeArray[i].elem != NULL)
		{
			itrElem = &(tableIt->AssociativeArray[i]);
			this->curIndex = i;
			return;
		}
	}
}

bool Table::Iterator::equals(Container::Iterator* right)
{
	size_t size;
	if (itrElem->elem == right->getElement(size))
		return true;
	else return false;
}

void Table::Iterator::setElement(element *ptr, int index)
{
	itrElem = ptr;
	curIndex = index;
}