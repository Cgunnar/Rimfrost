#pragma once

#ifdef RIMFROST_BUILD_DLL
	#define RIMFROST_API __declspec(dllexport)
#else
	#define RIMFROST_API __declspec(dllimport)
#endif // BUILD_TEST_DLL
