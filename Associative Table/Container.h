// ������� ����� ��� ���� �����������

#pragma once
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "MemoryManager.h"
using namespace std;

//������� ����� ��� ���� ����������� �����������
class Container
{
protected:
	MemoryManager& _memory;
public:
    // ������� ����� ��� ����������, ������� ��������� ���������
    struct Error
    {
        char msg[256];
        Error(const char* err_msg) { strcpy(msg,err_msg); }
    };

	class Iterator
	{
	public:
		// ���������� ���� ��������� �� �������, �� ������� ��������� �������� � ������ ������.
		// ������ ���������� ������ ������
		virtual void* getElement(size_t &size) = 0;

		// ���������� true, ���� ���� ��������� �������, ����� false.
		virtual bool hasNext() = 0;

		// ������� � ���������� ��������.
		virtual void goToNext() = 0;

		// �������� �� ��������� ����������
		virtual bool equals(Iterator* right) = 0;
	};

    Container(MemoryManager& mem):_memory(mem) {}

	//������� ���������� ��������, ������ ���������� ��������� � ����������.
	virtual int size() = 0;

    // ������� ���������� ��������, ������ ������������ ����������� ���������� � ������.
	virtual size_t max_bytes() = 0;

	//������� ���������� ����������� �� ��������, ����������� �� ������ ��������� � ���������� �������. ���� ������� �� ������, ������������ ������ ���������.
	virtual Iterator* find(void* elem, size_t size) = 0;

	//�������� ���� ��������� �� ����������.
	virtual void clear() = 0;

	//�������� ���������, ���������������� ������� ���� ����������.
	virtual Iterator* newIterator() = 0;

	//������� ���������� ��������� �� ��������, ����������� �� ������ ������� ����������. ���� ��������� ������, ������������ ������� ���������.
	virtual Iterator* begin() = 0;

	//������� ���������� ��������� �� ��������, ����������� ������� �� ��������� ��������� ����������. ���� ��������� ������, ������������ ������� ���������.
	virtual Iterator* end() = 0;

	// ���� ��������� ���� ���������� true, ����� false
    virtual bool empty() = 0;

    // �������� �������� �� �������, �� ������� ��������� �������� iter.
    // ����� �������� �������� ��������� �� ��������� �� ��������� �������.
	virtual void remove(Iterator *iter) = 0;
};