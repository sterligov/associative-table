#ifndef TABLE
#define TABLE
#include "Table.h"
//#define USER_MEMORY
#define STANDART_MEMORY

class Table: public AbstractTable
{
public:
	typedef struct element
	{
		void *elem;
		size_t elemSize;
		void *key;
		size_t keySize;
		bool deleted;
	} element;
public:
	int HASHSIZE, capacity;
private:
	element *AssociativeArray;
public:
	Table(MemoryManager& mem, int hash) : AbstractTable(mem) 
	{
		if (hash <= 0) { throw Error("Erro: size of hash table must be more 0\n"); }
		capacity = 0;
		HASHSIZE = hash;
		#ifdef USER_MEMORY
			AssociativeArray = (element *)mem.allocMem(HASHSIZE * sizeof(element));
		#else if
			AssociativeArray = new element[HASHSIZE];
		#endif
		for (int i = 0; i < HASHSIZE; i++) 
		{ 
			AssociativeArray[i].elem = NULL; 
			AssociativeArray[i].key = NULL;
			AssociativeArray[i].deleted = false;
		}
	}

	virtual int insertByKey(void* key, size_t keySize, void* elem, size_t elemSize);

	virtual void removeByKey(void* key, size_t keySize);

	virtual Iterator* findByKey(void* key, size_t keySize);

	virtual void* at(void* key, size_t keySize, size_t &valueSize);

	virtual size_t hash_function(void* key, size_t keySize);

	virtual Container::Iterator* newIterator();

	virtual Container::Iterator* begin();

	virtual int size() {return capacity;}

    virtual size_t max_bytes() {return 5 * _memory.size();}

	virtual Iterator* find(void* elem, size_t size);

	virtual void clear();

	virtual Iterator* end();

    virtual bool empty();

	virtual void remove(Container::Iterator *iter);
	
	void reHash();
// итератор
	class Iterator: public Container::Iterator
	{
		private:
			element *itrElem;
			int curIndex;
			Table *tableIt;
		public:
			Iterator(Table *it) 
			{
				tableIt = it;
				itrElem = NULL;
				curIndex = -1;
			}
			~Iterator() {}

			virtual void* getElement(size_t &size);

			// Возвращает true, если есть следующий элемент, иначе false.
			virtual bool hasNext();

			// Переход к следующему элементу.
			virtual void goToNext();

			// проверка на равенство итераторов
			virtual bool equals(Container::Iterator* right);
			
			virtual void setElement(element *elem, int index);

			virtual element *getItr(int &index) {index = curIndex; return itrElem;}
	};
};
#endif

