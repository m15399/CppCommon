
#pragma once

#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <list>
#include <mutex>
#include <string>
#include <string.h>
#include <assert.h>

#include <unistd.h>

using std::string;
using std::vector;
using std::list;
using std::mutex;
using std::cout;

#define CONCAT1(a, b) a##b
#define CONCAT(a, b) CONCAT1(a, b)

#define UNIQNAME(a) CONCAT(a,__LINE__)

#define LOCK(mtx) \
	std::lock_guard<std::mutex> CONCAT(_lock,__LINE__)(mtx)



enum LogLevel {
	DEBUG = 0,
	ERROR,
	SILENT
};

using Logger = LogLevel;

constexpr Logger JobLogger(SILENT);

#define LogMessage(level, logger, fmt, ...) \
	if(logger <= level) { \
		printf("[" #logger "] %s:%d<%lx> " fmt "\n", __FILE__, __LINE__, (unsigned long)pthread_self(), __VA_ARGS__); \
	}

#define LogDebug(logger, fmt, ...) \
	LogMessage(DEBUG, logger, fmt, __VA_ARGS__);
#define LogDebugSz(logger, fmt) \
	LogDebug(logger, fmt "%s", "");


#define Fatal(fmt, ...) \
	printf("FATAL ERROR: [%s:%d] " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
	exit(1);

#define FatalSz(fmt) \
	printf("FATAL ERROR: [%s:%d] " fmt "\n", __FILE__, __LINE__); \
	exit(1);



#include <Tests.h>
#include <Threads.h>
#include <Jobs.h>
#include <Graph.h>

