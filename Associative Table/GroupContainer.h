#pragma once
#include "Container.h"

//������� ����� ��� ��������� ������ ����������� �����������
class GroupContainer: public Container
{
public:
	GroupContainer(MemoryManager& mem):Container(mem) {}
};