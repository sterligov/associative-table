#include "28_test.h"
#define PRINT_FUNC_ERROR(MESSAGE) cout << "Error in " << MESSAGE << " function\n"; 

void test_28::all_test(Table *table, int amount)
{
	int error;
	for (int i = 0; i < NUM_TEST_FUNC; i++)
	{
		error = (this->*funcPtr[i])(table, amount);
		if (error) { PRINT_FUNC_ERROR(funcName[i]); }
	}
	if (!error) 
	{
		cout << "insert: elements - " << amount << ", time - " << getInsertTime() << " seconds\n";
		cout << "removeByKey: elements - " << amount << ", time - " << getRemoveByKeyTime() << " seconds\n";
		cout << "remove: elements - " << amount << ", time - " << getRemoveTime() << " seconds\n";
		cout << "clear: elements - " << amount << ", time - " << getClearTime() << " seconds\n";
		cout << "findByKey test - Done\n";
		cout << "at test - Done\n";
		cout << "find test - Done\n";
		cout << "---------------------------------------\n";
		cout << "Done\n";
	}
}

void test_28::insert_test(Table *table, int amount)
{
	int error;
	error = insertTest(table, amount);
	if (error) { PRINT_FUNC_ERROR("insert"); }
	else
	{
		cout << "insert: elements - " << amount << ", time - " << getInsertTime() << " seconds\n";
	}
}

void test_28::removeByKey_test(Table *table, int amount)
{
	int error;
	error = removeByKeyTest(table, amount);
	if (error) { PRINT_FUNC_ERROR("removeByKey"); }
	else
	{
		cout << "removeByKey: elements - " << amount << ", time - " << getRemoveByKeyTime() << " seconds\n";
	}
}

void test_28::remove_test(Table *table, int amount)
{
	int error;
	error = removeTest(table, amount);
	if (error) { PRINT_FUNC_ERROR("remove"); }
	else
	{
		cout << "remove: elements - " << amount << ", time - " << getRemoveTime() << " seconds\n";
	}
}

void test_28::clear_test(Table *table, int amount)
{
	int error;
	error = clearTest(table, amount);
	if (error) { PRINT_FUNC_ERROR("clear"); }
	else
	{
		cout << "clear: elements - " << amount << ", time - " << getClearTime() << " seconds\n";
	}
}

void test_28::at_test(Table *table, int amount)
{
	int error;
	error = atTest(table, amount);
	if (error) { PRINT_FUNC_ERROR("at"); }
	else { cout << "--------------\n" << "Done\n"; }
}

void test_28::findByKey_test(Table *table, int amount)
{
	int error;
	error = findByKeyTest(table, amount);
	if (error) { PRINT_FUNC_ERROR("findByKey"); }
	else { cout << "--------------\n" << "Done\n"; }
}

void test_28::find_test(Table *table, int amount)
{
	int error;
	error = findTest(table, amount);
	if (error) { PRINT_FUNC_ERROR("find"); }
	else { cout << "--------------\n" << "Done\n"; }
}

/////////////////////////////////////////


int TableTest::insertTest(Table *table, int amount)
{
	clock_t prev;
	prev = clock();
	for (int i = 0; i < 2 * amount; i += 2)
	{
		table->insertByKey(&i, sizeof(i), &i, sizeof(i));
	}
	for (int i = 0; i < 2 * amount; i++)
	{
		size_t sz;
		void *tmp;
		tmp = table->at(&i, sizeof(i), sz);
		if ( ((i % 2 == 0) && (!tmp)) || ((i % 2 != 0) && (tmp)) ) 
		{ 
			table->clear();
			return 1; 
		}			
	}
	TableTest::insertTime = (double)(clock() - prev) / CLOCKS_PER_SEC;
	table->clear();
	return 0;
}

int TableTest::removeByKeyTest(Table *table, int amount)
{
	for (int i = 0; i < 2 * amount; i++)
	{
		table->insertByKey(&i, sizeof(i), &i, sizeof(i));
	}

	clock_t prev;
	prev = clock();
	for (int i = 0; i < 2 * amount; i += 2) // удаляем четные
	{
		table->removeByKey(&i, sizeof(i));
	}
	TableTest::removeByKeyTime = (double)(clock() - prev) / CLOCKS_PER_SEC;
	
	for (int i = 0; i < 2 * amount; i++)
	{
		void *tmp;
		size_t sz;
		tmp = table->at(&i, sizeof(i), sz);
		if ( ((i % 2) && (!tmp)) || (!(i % 2) && (tmp)) ) 
		{ 
			table->clear(); 
			return 1; 
		}
	} 
	table->clear();
	return 0;
}

int TableTest::removeTest(Table *table, int amount)
{
	// бежим итератором и удаляем, затем проверяем на пустоту
	for (int i = 0; i < amount; i++)
	{
		table->insertByKey(&i, sizeof(i), &i, sizeof(i));
	}
	
	size_t sz;
	Container::Iterator *it;
	it = table->begin();

	clock_t prev;
	prev = clock();
	while (it->getElement(sz))
	{
		table->remove(it);
	}
	removeTime = (double)(clock() - prev) / CLOCKS_PER_SEC;

	it = table->begin();
	if (it != NULL) { return 1; }
	return 0;
}

int TableTest::clearTest(Table *table, int amount)
{
	for (int i = 0; i < amount; i++)
	{
		table->insertByKey(&i, sizeof(i), &i, sizeof(i));
	}

	clock_t prev;
	prev = clock();
	table->clear();
	clearTime = (double)(clock() - prev) / CLOCKS_PER_SEC;

	Container::Iterator *it = table->begin();
	if (it != NULL) { return 1; }
	return 0;
}

int TableTest::atTest(Table *table, int amount)
{
	srand(time(NULL));
	int *testArray;
	testArray = new int[2 * amount];

	for (int i = 0; i < 2 * amount; i++)
	{
		if ( i % 2 == 0) { testArray[i] = rand() % amount; }
		else { testArray[i] = amount + rand() % (2 * amount);}
	}
	
	for (int i = 0; i < 2 * amount; i += 2) 
	{
		table->insertByKey(&testArray[i], sizeof(int), &testArray[i], sizeof(int));
	}

	for (int i = 0; i < 2 * amount; i++)
	{
		size_t sz;
		void *result;
		result = table->at(&testArray[i], sizeof(int), sz);
		if ( (result != NULL) && (i % 2 != 0)) { return 1; } // если на нечетном не пустота - ошибка
		if ( (memcmp(result, &testArray[i], sz) || !result) && (i % 2 == 0) ) { return 1; }
	}
	table->clear();
	return 0;
}

int TableTest::findByKeyTest(Table *table, int amount)
{
	srand(time(NULL));
	int *testArray;
	testArray = new int[2 * amount];
	Container::Iterator* it = table->newIterator();

	for (int i = 0; i < 2 * amount; i++)
	{
		if ( i % 2 == 0) { testArray[i] = rand() % amount; }
		else { testArray[i] = amount + rand() % (2 * amount);}
	}
	
	for (int i = 0; i < 2 * amount; i += 2) 
	{
		table->insertByKey(&testArray[i], sizeof(int), &testArray[i], sizeof(int));
	}

	for (int i = 0; i < 2 * amount; i++)
	{
		it = table->findByKey(&testArray[i], sizeof(int));
		if ( (it != NULL) && (i % 2 != 0)) { return 1; } // если на нечетном не пустота - ошибка
		if ( i % 2 == 0 )
		{
			void *result;
			size_t sz;
			result = it->getElement(sz);
			if (memcmp(result, &testArray[i], sz) || !result) { return 1; }
		}
	}
	table->clear();
	return 0;	
} 

int TableTest::findTest(Table *table, int amount)
{
	int count = 1000;
	srand(time(NULL));
	int *testArray;
	testArray = new int[2 * count];
	Container::Iterator* it = table->newIterator();

	for (int i = 0; i < 2 * count; i++)
	{
		if ( i % 2 == 0) { testArray[i] = rand() % count; }
		else { testArray[i] = amount + rand() % (2 * count);}
	}
	
	for (int i = 0; i < 2 * count; i += 2) 
	{
		table->insertByKey(&testArray[i], sizeof(int), &testArray[i], sizeof(int));
	}

	for (int i = 0; i < 2 * count; i++)
	{
		it = table->find(&testArray[i], sizeof(int));
		if ( (it != NULL) && (i % 2 != 0)) { return 1; } // если на нечетном не пустота - ошибка
		if ( i % 2 == 0 )
		{
			void *result;
			size_t sz;
			result = it->getElement(sz);
			if (memcmp(result, &testArray[i], sz) || !result) { return 1; }
		}
	}
	table->clear();
	return 0;
}



