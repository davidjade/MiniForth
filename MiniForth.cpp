// MiniTIL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Windows.h>

extern "C"
{
#include "InnerInterpreter.h"
#include "MiniForth.h"
}


long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}




int main()
{
	std::cout << "Sorting...\n";

	InitInterpreter();

	InitWords();

	//TestStack();


	// execute and time the TEST1 Word
	size_t count = 5;
	long long average = 0;
	for (size_t i = 0; i < count; i++)
	{
		long long start = milliseconds_now();
		Execute(TEST1);
		long long elapsed = milliseconds_now() - start;
		average += elapsed;

		std::cout << "Sort time: " << elapsed << "\n";
	}

	std::cout << "Average time: " << average / count << "\n";


	/*
		Once you step over the above Execute call, try viewing the global DataStackPtr variable 
		Press Shift+F9 to bring up the variable inspector and this enter this

			DataStackPtr

		Expand the Ptr and you will see top of stack result (IntObject with a value of 4)

	
		Next try viewing the DataStack global variable here (the whole stack of objects). 
		Enter this expression

			DataStack, view(array)

		The ", view(array)" shows the custom Object array walking visualization and will walk the variable sized Objects on the stack!

	*/
	 
}



void TestStack()
{
	Object o1, o2, o3;
	MakeIntObject(&o1, 1);
	MakeIntObject(&o2, 2);
	MakeIntObject(&o3, 3);

	Push(&DataStackPtr, &o1);
	Push(&DataStackPtr, &o2);
	Push(&DataStackPtr, &o3);


	Object o4, o5, o6;
	MakeIntObject(&o4, 0);
	MakeIntObject(&o5, 0);
	MakeIntObject(&o6, 0);

	Pop(&DataStackPtr, &o4);
	Pop(&DataStackPtr, &o5);
	Pop(&DataStackPtr, &o6);
}
