#include "28_Table.h"
#include <ctime>

class TableTest
{
private:
	Table *table;
	double insertTime, removeByKeyTime, removeTime, findTime, clearTime;
protected:

	TableTest() {}

	int atTest(Table *table, int amount);

	int insertTest(Table *table, int amount);

	int removeByKeyTest(Table *table, int amount);
	
	int removeTest(Table *table, int amount);

	int clearTest(Table *table, int amount);

	int findTest(Table *table, int amount); 

	int findByKeyTest(Table *table, int amoutn);

	double getInsertTime() { return insertTime; }
	
	double getRemoveTime() { return removeTime; }
	
	double getRemoveByKeyTime() { return removeByKeyTime; }

	double getClearTime() { return clearTime; }

public:
	~TableTest() {}
};

typedef	int (TableTest::*funcPointer)(Table*, int);

class test_28: public TableTest
{
	static const int NUM_TEST_FUNC = 7;
	char *funcName[NUM_TEST_FUNC];
	funcPointer funcPtr[NUM_TEST_FUNC];
public:
	test_28() : TableTest()
	{
		funcName[0] = "insert";
		funcName[1] = "removeByKey";
		funcName[2] = "remove";
		funcName[3] = "clear";
		funcName[4] = "at";
		funcName[5] = "findByKey";
		funcName[6] = "find";

		funcPtr[0] = &test_28::insertTest;
		funcPtr[1] = &test_28::removeByKeyTest;
		funcPtr[2] = &test_28::removeTest;
		funcPtr[3] = &test_28::clearTest;
		funcPtr[4] = &test_28::atTest;
		funcPtr[5] = &test_28::findByKeyTest;
		funcPtr[6] = &test_28::findTest;
	}
	
	void all_test(Table *table, int amount);

	void insert_test(Table *table, int amount);
	
	void removeByKey_test(Table *table, int amount);

	void remove_test(Table *table, int amount);

	void clear_test(Table *table, int amount);

	void at_test(Table *table, int amount);

	void findByKey_test(Table *table, int amoutn);

	void find_test(Table *table, int amount);

}; 

