
#include <CppCommon.h>

static std::vector<TestObject*>& GetAllTests(){
	static std::vector<TestObject*> allTests;
	return allTests;
}

int RunTests(){
	printf("== Running tests ==\n");

	std::vector<TestObject*> failures;
	string lastCategory = "";

	for(TestObject* test : GetAllTests()){
		string category = test->m_category;
		if(category != lastCategory){
			printf("\e[32m[--------]\e[0m\n");
			lastCategory = category;
		}

		printf("\e[32m[  TEST  ]\e[0m   %s::%s\n",
				category.c_str(),
				test->m_name.c_str());

		int status = test->Run();

		if (status == OK){
			printf("\e[32m[   OK   ]\e[0m\n");
		} else {
			printf("\e[31m[ FAILED ]\e[0m\n"); 
			failures.push_back(test);
		}
	}

	printf("\e[32m[--------]\e[0m\n");
	printf("== Done ==\n");
	for(TestObject* test : failures){
		printf("  Failure: %s::%s\n", test->m_category.c_str(), test->m_name.c_str());
	}
	printf("\n");

	if(failures.size() == 0)
		return OK;
	else
		return FAILED;
}

TestObject::TestObject(string category, string name, TestFunc func){
	GetAllTests().push_back(this);
	m_category = category;
	m_name = name;
	m_func = func;
}

int TestObject::Run(){
	int testStatus = OK;

	m_func(&testStatus);

	if (testStatus == OK){
		return OK;
	} else {
		return FAILED;
	}
}



TEST(Test, TestPass){
	int x = 3;
	ASSERT_EQ(x, 3);
	ASSERT_TRUE(x == 3);
};

