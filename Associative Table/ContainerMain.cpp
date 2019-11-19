#include "28_Table.h"
#include "28_Table_opt.h"
#include "28_Mem.h"
#include "28_Test.h"


int main()
{
	Mem m(100000000, 10);
	Table_28_opt table_opt(m, 50000);
	Table table(m, 50000);
	//int x = 10;
	//table_opt.insertByKey(&x, sizeof(x), &x, sizeof(x));
	//int *y;
	//Container::Iterator *it;
	//it = table_opt.findByKey(&x, sizeof(x));
	//size_t t;
	//y = (int *)it->getElement(t);
	//cout << *y;
	test_28 test;
	test.all_test(&table_opt, 1000);
//	test.all_test(&table, 10000);
	getchar();
	return 0;
}