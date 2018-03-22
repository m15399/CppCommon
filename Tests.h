
#pragma once

#include <CppCommon.h>

int RunTests();

enum TestStatus {
	OK,
	FAILED
};

using TestFunc = std::function<void(int*)>;

class TestObject {
public:
	string m_category;
	string m_name;
	TestFunc m_func;

	TestObject(string category, string name, TestFunc func);
	int Run();
};

#define TEST(category, name) \
	static void UNIQNAME(testFunc)(int* _testStatus); \
	static TestObject UNIQNAME(testObject)(#category, #name, UNIQNAME(testFunc)); \
	static void UNIQNAME(testFunc)(int* _testStatus)

#define PRINT_FAIL_LOC() \
	printf("  FAILURE at %s::%d\n", __FILE__, __LINE__);
// TODO we should throw instead of return
#define TEST_FAIL(msg) \
	*_testStatus = FAILED; return;
#define ASSERT_TRUE(e) \
	if (!(e)){ PRINT_FAIL_LOC(); TEST_FAIL() }
#define ASSERT_FALSE(e) \
	if ((e)){ PRINT_FAIL_LOC(); TEST_FAIL() }
#define ASSERT_EQ(e, e2) \
	{ \
		auto UNIQNAME(_e1val) = (e); \
		auto UNIQNAME(_e2val) = (e2); \
		if (UNIQNAME(_e1val) != UNIQNAME(_e2val)){ \
			PRINT_FAIL_LOC(); \
			std::cout << "    " << UNIQNAME(_e1val) << \
					"\n      vs\n    " \
					<< UNIQNAME(_e2val) << "\n"; \
			TEST_FAIL() \
		} \
	}

