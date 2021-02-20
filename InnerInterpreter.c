#include "InnerInterpreter.h"

#include <malloc.h>




// stack storage
#define STACK_SIZE 1024
Object* DataStack;
Object* ReturnStack;


// Dictionary storage
#define DICTIONARY_SIZE 32768
Object* Dictionary;



void InitInterpreter()
{
	// create stacks
	DataStack = (Object*)malloc(MAX_OBJECT_SIZE * STACK_SIZE);
	if (DataStack)
	{
		memset(DataStack, 0, MAX_OBJECT_SIZE * STACK_SIZE);	// effectively fills it will NULLObjects
		MakeObjectPtr(&DataStackPtr, DataStack);

	}

	ReturnStack = (Object*)malloc(MAX_OBJECT_SIZE * STACK_SIZE);
	if (ReturnStack)
	{
		memset(ReturnStack, 0, MAX_OBJECT_SIZE * STACK_SIZE);
		MakeObjectPtr(&ReturnStackPtr, ReturnStack);
	}

	// create Dictionary
	Dictionary = (Object*)malloc(DICTIONARY_SIZE);
	if (Dictionary)
	{
		memset(Dictionary, 0, DICTIONARY_SIZE);
		MakeObjectPtr(&DictionaryPtr, Dictionary);
	}

	// init interpreter registers
	MakeObjectPtr(&WordAddress, NULL);
	MakeObjectPtr(&InstructionAddress, NULL);
}

void Push(Object* Stack, Object* Object)
{
	IncPtr(Stack);
	CopyObject(Object, Stack->Ptr);
}


void Pop(Object* Stack, Object* Object)
{
	CopyObject(Stack->Ptr, Object);

	DecPtr(Stack);
}


void LocalPop(Object* Stack, Object* Object)
{
	LocalCopyObject(Stack->Ptr, Object);
	DecPtr(Stack);
}


void* AllocNative(size_t Size)
{
	void* dest = memset(DictionaryPtr.NativePtr, 0, Size);
	DictionaryPtr.NativePtr += Size;
	return dest;
}


void* _Run()	// only used to enter interpreter
{
	CodeAddress CodeAddressPtr = WordAddress.Ptr->CA;
	IncPtr(&WordAddress);
	
	return CodeAddressPtr();	// execute native code
}


void* _Next()
{
	SetObjectPtr(&WordAddress, InstructionAddress.Ptr->Word->CodeBody);
	IncPtr(&InstructionAddress);

	CodeAddress CodeAddressPtr = WordAddress.Ptr->CA;
	IncPtr(&WordAddress);

	return CodeAddressPtr();	// execute native code
}


void* _Colon()
{
	Push(&ReturnStackPtr, &InstructionAddress);
	SetObjectPtr(&InstructionAddress, WordAddress.Ptr);

	return _Next;
}


void* _Semi()
{
	LocalPop(&ReturnStackPtr, &InstructionAddress);
	return _Next;
}


void* _Terminate()
{
	return NULL;
}

void* _Constant()
{
	Push(&DataStackPtr, WordAddress.Ptr);

	return _Next;
}

void* _Variable()
{
	Push(&DataStackPtr, &WordAddress);

	return _Next;
}

void* _DoRelativeJump()
{
	// since everything is fixed, a simple direct jump is possible
	InstructionAddress.Ptr += InstructionAddress.Ptr->i;

	return _Next;
}

void* _DoLoop()
{
	Object index;
	Object limit;

	LocalPop(&ReturnStackPtr, &index);
	LocalPop(&ReturnStackPtr, &limit);

	// (the Abs of this is the number of iterations left)
	int w = index.i - limit.i;

	// increment loop index
	index.i += 1;

	// check for end of looping (not sure how this works but I got it from another Forth implementation - it handles both inc & dec looping)
	if (((index.i - limit.i) ^ w) > 0)
	{
		Push(&ReturnStackPtr, &limit);
		Push(&ReturnStackPtr, &index);

		_DoRelativeJump();
	}
	else
	{
		IncPtr(&InstructionAddress);
	}

	return _Next;
}


void* _DoConditionalRelativeJump()
{
	Object flag;

	LocalPop(&DataStackPtr, &flag);

	if (flag.i)
	{
		IncPtr(&InstructionAddress);
	}
	else
	{
		_DoRelativeJump();
	}

	return _Next;
}


void Execute(Word* Word)
{
	// set the word to execute
	SetObjectPtr(&WordAddress, Word->CodeBody);

	// start the interpreter loop
	CodeAddress func = _Run;
	do
	{
		func = func();
	} while (func != NULL);
}


