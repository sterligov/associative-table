#pragma once

// Базовый класс менеджера памяти
#define MIN_SIZE 40000000

class MemoryManager {
private:
	size_t _size;
public:
	MemoryManager(size_t sz) 
	{
		_size = sz;
	}
public:
    size_t size() { return _size;  }

	virtual void* allocMem(size_t sz) = 0;

    virtual void freeMem(void* ptr) = 0;

};