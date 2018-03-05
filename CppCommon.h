
#pragma once

#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <list>
#include <mutex>
#include <string>
#include <string.h>

#include <unistd.h>

#define LOCK(mtx) \
	std::lock_guard<std::mutex> lock_##__LINE__(mtx)

enum LogLevel {
	DEBUG = 0,
	ERROR,
	SILENT
};

using Logger = LogLevel;

constexpr Logger JobLogger(SILENT);

#define LogMessage(level, logger, fmt, ...) \
	if(logger <= level) { \
		printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
	}

#define LogDebug(logger, fmt, ...) \
	LogMessage(DEBUG, logger, fmt, __VA_ARGS__);

#define Fatal(fmt, ...) \
	printf("FATAL ERROR: [%s:%d] " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
	exit(1);

#define FatalSz(fmt) \
	printf("FATAL ERROR: [%s:%d] " fmt "\n", __FILE__, __LINE__); \
	exit(1);

#include <Threads.h>
#include <Jobs.h>

