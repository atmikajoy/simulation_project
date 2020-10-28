#define _CRT_SECURE_NO_WARNINGS

#include "driver.h"
#include <iostream>
#include <cstring>
#include "log_dir.h"

int main()
{
	using namespace atmik::simulation;
	// std::cout << clock::max_days << '\n';
	driver::run( { 8, 0, 0 }, { 20, 0, 0 }, 10 );
}