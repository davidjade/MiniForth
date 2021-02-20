#pragma once

#include "Objects.h"
#include "Words.h"


// stack pointers and stack functions
Object DataStackPtr;
Object ReturnStackPtr;

void Push(Object* Stack, Object* Object);
void Pop(Object* Stack, Object* Object);
void LocalPop(Object* Stack, Object* Object);	// Note: can only be used to get a local sizeof(Object) Object!


// Dictionary pointer and dictionary functions
Object DictionaryPtr;

void* AllocNative(size_t Size);


// Inner Interpreter registers
Object WordAddress;
Object InstructionAddress;


// Inner Interpreter execution state machine
void* _Run();
void* _Next();
void* _Colon();
void* _Semi();
void* _Terminate();
void* _Constant();
void* _Variable();
void* _DoRelativeJump();
void* _DoLoop();
void* _DoConditionalRelativeJump();


// Init Inner Interpreter
void InitInterpreter();


// Execute a Secondary Word (has to have a COLON & SEMI/TERMINATE to work)
void Execute(Word* Word);

