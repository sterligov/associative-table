#include "28_Table_opt.h"
#include <stdio.h>

size_t Table_28_opt::hash_function(void *key, size_t keySize)
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


int Table_28_opt::insertByKey(void* key, size_t keySize, void* elem, size_t elemSize)
{
	if (!key || !elem) { return -1; }
	int index = hash_function(key, keySize);
	element *tmp = AssociativeArray[index];
	while (tmp)
	{
		if (!memcmp(tmp->key, key, keySize) && (tmp->keySize == keySize)) // перезапишем
		{	
			try
			{
				#ifdef USER_MEMORY
					_memory.freeMem(tmp->elem);
					tmp->elem = (void *)_memory.allocMem(elemSize);
				#else if
					delete tmp->elem;
					tmp->elem = new char[elemSize];
				#endif
			}
			catch (Mem::memoryError err)
			{
				cout << err.msg;
			}
			memcpy(tmp->elem, elem, elemSize);
			tmp->elemSize = elemSize;
			return 0; 
		}
		tmp = tmp->next;
	}
	// добавим элемент
	try
	{
		#ifdef USER_MEMORY
			tmp = (element *)_memory.allocMem(sizeof(element));
			tmp->elem = (void *)_memory.allocMem(elemSize);
			tmp->key = (void *)_memory.allocMem(keySize);
		#else if
			tmp = new element;
			tmp->elem = new char[elemSize];
			tmp->key = new char[keySize];
		#endif
	}
	catch (Mem::memoryError err)
	{
		cout << err.msg;
		return -1;
	}
	tmp->elemSize = elemSize;
	tmp->keySize = keySize;
	memcpy(tmp->elem, elem, elemSize);
	memcpy(tmp->key, key, keySize);
	tmp->next = AssociativeArray[index];
	AssociativeArray[index] = tmp;
	++capacity;
	return 0;
}

void Table_28_opt::removeByKey(void* key, size_t keySize)
{
	if (key == NULL) { return; }
	int index = hash_function(key, keySize);
	element *cur = AssociativeArray[index];
	element *prev;
	cur = AssociativeArray[index];
	prev = NULL;
	while(cur)
	{
		if (!(memcmp(cur->key, key, keySize)) && (cur->keySize == keySize))
		{
			if (prev == NULL) { AssociativeArray[index] = cur->next; } // первый элемент
			else { prev->next = cur->next; }
			#ifdef USER_MEMORY
				_memory.freeMem(cur->elem);
				_memory.freeMem(cur->key);
				_memory.freeMem(cur);
			#else if
				delete cur->elem;
				delete cur->key;
				delete cur;
			#endif
			--capacity;
			return;
		}
		prev = cur;
		cur = cur->next;
	}
}

void* Table_28_opt::at(void* key, size_t keySize, size_t &valueSize)
{
	if (key == NULL || keySize <= 0) {return NULL;}
	int index = hash_function(key, keySize);
	element *cur;
	cur = AssociativeArray[index];
	while (cur)
	{
		if (!(memcmp(cur->key, key, keySize)) && (cur->keySize == keySize))
		{
			valueSize = cur->elemSize; 
			return cur->elem;
		}
		cur = cur->next;
	}
	if (!cur) {valueSize = 0; return NULL;} // не нашли
}

void Table_28_opt::remove(Container::Iterator *iter)
{
	int index;
	Table_28_opt::element *removeElem, *prev, *cur;
	Table_28_opt::Iterator *it;
	it = (Table_28_opt::Iterator *)iter;

	removeElem = it->getItr(index);
	if (!removeElem) { return; }
	iter->goToNext();

	prev = cur = AssociativeArray[index];
	if (cur == removeElem) // первый элемент в списке
	{
		cur = AssociativeArray[index]->next;
		#ifdef USER_MEMORY
			_memory.freeMem(AssociativeArray[index]->elem);
			_memory.freeMem(AssociativeArray[index]->key);
			_memory.freeMem(AssociativeArray[index]);
		#else if
			delete AssociativeArray[index]->elem;
			delete AssociativeArray[index]->key;
			delete AssociativeArray[index];
		#endif
		AssociativeArray[index] = cur;
		--capacity;
	}
	else 
	{
		element *prev;
		cur = AssociativeArray[index]->next;
		prev = AssociativeArray[index];
		while(cur)
		{
			if (cur == removeElem)
			{
				prev->next = cur->next;
				#ifdef USER_MEMORY
					_memory.freeMem(cur->elem);
					_memory.freeMem(cur->key);
					_memory.freeMem(cur);
				#else if
					delete cur->elem;
					delete cur->key;
					delete cur;
				#endif
				--capacity;
				return;
			}
			prev = cur;
			cur = cur->next;
		}
	}
}

Container::Iterator* Table_28_opt::findByKey(void* key, size_t keySize)
{
	if (!key || keySize <= 0) { return NULL; }
	int index = Table_28_opt::hash_function(key, keySize);
	element *tmp = Table_28_opt::AssociativeArray[index];
	if (AssociativeArray[index] == NULL) {return NULL;} // не нашли
	while(tmp)
	{
		if	(!memcmp(tmp->key, key, keySize) && tmp->keySize == keySize)
		{
			break; // нашли
		}
		tmp = tmp->next;
	}
	if (!tmp) {return NULL;} // прошли по всему списку(не нашли)
	Table_28_opt::Iterator *it;
	it = new Table_28_opt::Iterator(this);
	it->setElement(tmp, index);
	return it;
}

Container::Iterator* Table_28_opt::newIterator()
{
	Table_28_opt::Iterator *itr;
	itr = new Table_28_opt::Iterator(this);
	return itr;
}

Container::Iterator* Table_28_opt::begin()
{
	for (int i = 0; i < HASHSIZE; i++)
	{
		if (AssociativeArray[i] != NULL)
		{
			Table_28_opt::Iterator *it;
			it = new Table_28_opt::Iterator(this);
			it->setElement(AssociativeArray[i], i); 
			return it;
		}
	}
	return NULL;
}

Container::Iterator* Table_28_opt::end()
{
	int index = -1;
	for (int i = 0; i < HASHSIZE; i++)
	{
		if (AssociativeArray[i] != NULL) { index = i; }
	}

	if (index == -1) { return NULL; } // пустая таблица
	else
	{
		Table_28_opt::Iterator *it;
		it = new Table_28_opt::Iterator(this);
		it->setElement(AssociativeArray[index], index); 
		return it;
	}
}

bool Table_28_opt::empty() 
 {
	 if (!capacity) return true;
	 else return false;
 }

Container::Iterator *Table_28_opt::find(void* elem, size_t size)
{
	element *tmp;
	for (int i = 0; i < HASHSIZE; i++)
	{
		tmp = AssociativeArray[i];
		while (tmp)
		{
			if (!memcmp(tmp->elem, elem, size) && tmp->elemSize == size)
			{
				Table_28_opt::Iterator *it;
				it = new Table_28_opt::Iterator(this);
				it->setElement(tmp, i); 
				return it;
			}
			tmp = tmp->next;
		}
	}
	return NULL; // не нашли
}

void Table_28_opt::clear() 
{
	element *tmp;
	for (int i = 0; i < HASHSIZE; i++)
	{
		while(AssociativeArray[i] != NULL)
		{
			tmp = AssociativeArray[i];
			AssociativeArray[i] = AssociativeArray[i]->next;
			#ifdef USER_MEMORY
 				_memory.freeMem(tmp->elem);
				_memory.freeMem(tmp->key);
				_memory.freeMem(tmp);
			#else if
				delete tmp->elem;
				delete tmp->key;
				delete tmp;
			#endif
			--capacity;
		}
	//	AssociativeArray[i] = NULL;
	}
}

void *Table_28_opt::Iterator::getElement(size_t &size)
{
	if (itrElem == NULL)
	{
		size = 0;
		return NULL;
	}

	else
	{
		size = itrElem->elemSize;
		return itrElem->elem;
	}
}

bool Table_28_opt::Iterator::hasNext()
{
	if (itrElem->next) // в том же списке
	{
		return true;
	}
	int index = this->curIndex;
	for (int i = index + 1; i < tableIt->HASHSIZE; i++)
	{
		if (tableIt->Table_28_opt::AssociativeArray[i] != NULL) { return true; }
	}
	return false;
}

void Table_28_opt::Iterator::goToNext()
{

	if (itrElem->next) // список не из одного элемента
	{
		itrElem = itrElem->next;
		return;
	}
	else
	{
		int index = this->curIndex;
		itrElem = NULL; // будет лежать в итераторе, если нет следующего элемента
		curIndex = -1;
		for (int i = index + 1; i < tableIt->HASHSIZE; i++)
		{
			if (tableIt->AssociativeArray[i] != NULL)
			{
				itrElem = tableIt->AssociativeArray[i];
				this->curIndex = i;
				return;
			}
		}
	}
}

bool Table_28_opt::Iterator::equals(Container::Iterator* right)
{
	size_t size;
	if (itrElem->elem == right->getElement(size))
		return true;
	else return false;
}

void Table_28_opt::Iterator::setElement(element *ptr, int index)
{
	itrElem = ptr;
	curIndex = index;
}



