#ifndef TABLE_OPT
#define TABLE_OPT
#include "28_Table.h"
#include "28_Mem.h"
//#define USER_MEMORY
#define STANDART_MEMORY


class Table_28_opt: public Table
{
public:
	typedef struct element
	{
		void *elem;
		size_t elemSize;
		void *key;
		size_t keySize;
		element *next;
	} element;
private:
	Table_28_opt::element **AssociativeArray;
public:
	Table_28_opt(MemoryManager& mem, int hash) : Table(mem, hash) 
	{
		if (hash <= 0) { throw Error("Error in parametrs of constructor"); }
		Table::capacity = 0;
		Table::HASHSIZE = hash;
		#ifdef USER_MEMORY
			Table_28_opt::AssociativeArray = (Table_28_opt::element **)mem.allocMem(HASHSIZE * sizeof(Table_28_opt::element*));
		#else if
			Table_28_opt::AssociativeArray = new Table_28_opt::element*[HASHSIZE];
		#endif
		for (int i = 0; i < Table::HASHSIZE; i++) { Table_28_opt::AssociativeArray[i] = NULL; }
	}

	~Table_28_opt()
	{
		this->clear();
	}

	
	virtual int insertByKey(void* key, size_t keySize, void* elem, size_t elemSize);

	virtual void removeByKey(void* key, size_t keySize);

	virtual Container::Iterator* findByKey(void* key, size_t keySize);

	virtual void* at(void* key, size_t keySize, size_t &valueSize);

	virtual size_t hash_function(void* key, size_t keySize);

	virtual Container::Iterator* newIterator();

	virtual Container::Iterator* begin();

	virtual int size() {return capacity;}

    virtual size_t max_bytes() {return 5 * _memory.size();}

	virtual Container::Iterator* find(void* elem, size_t size);

	virtual void clear();

	virtual Container::Iterator* end();

    virtual bool empty();

	virtual void remove(Container::Iterator *iter);
	
	class Iterator: public Container::Iterator
	{
		private:
			element *itrElem;
			int curIndex;
			Table_28_opt *tableIt;
		public:
			Iterator(Table_28_opt *it) 
			{
				Table_28_opt::Iterator::tableIt = it;
				Table_28_opt::Iterator::itrElem = NULL;
				Table_28_opt::Iterator::curIndex = -1;
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

