
#include "InnerInterpreter.h"

#include <malloc.h>
#include <string.h>


#define countof(a) (sizeof(a) / sizeof(a[0]))


Word* LastWord = NULL;


Word* MakePrimitiveWord(char* name, CodeAddress CA)
{
	Word* word = (Word*)AllocNative(sizeof(Word));
	if (word)
	{
		strcpy_s(word->Name, sizeof(word->Name), name);
		word->Next = LastWord;
		LastWord = word;

		Object CAObject;
		MakeCodeAddressObject(&CAObject, CA);
		CopyObject(&CAObject, DictionaryPtr.Ptr);
		IncPtr(&DictionaryPtr);


		// NULLObject terminate so it can be traversed to find the end
		Object null;
		MakeNULLObject(&null);
		CopyObject(&null, DictionaryPtr.Ptr);
		IncPtr(&DictionaryPtr);
	}

	return word;
}


Word* MakeSecondaryWord(char* name, CodeAddress CA, Object Objects[], size_t Count)
{
	Word* word = MakePrimitiveWord(name, CA);
	if (word)
	{
		// back up to rewrite CodeBody termination to append more Words
		DecPtr(&DictionaryPtr);

		for (size_t i = 0; i < Count; i++)
		{
			CopyObject(&Objects[i], DictionaryPtr.Ptr);
			IncPtr(&DictionaryPtr);
		}

		// NULLObject terminate so it can be traversed to find the end
		Object null;
		MakeNULLObject(&null);
		CopyObject(&null, DictionaryPtr.Ptr);
		IncPtr(&DictionaryPtr);
	}

	return word;
}


Word* MakeVariable(char* name, CodeAddress CA, Object* Value)	// NOTE: Also makes a CONSTANT (when using the constant CA)
{
	Word* word = MakePrimitiveWord(name, CA);
	if (word)
	{
		// back up to rewrite CodeBody termination to append more Words
		DecPtr(&DictionaryPtr);

		CopyObject(Value, DictionaryPtr.Ptr);
		IncPtr(&DictionaryPtr);

		// NULLObject terminate so it can be traversed to find the end
		Object null;
		MakeNULLObject(&null);
		CopyObject(&null, DictionaryPtr.Ptr);
		IncPtr(&DictionaryPtr);
	}

	return word;
}


Word* MakeArray(char* name, CodeAddress CA, Object* Value, size_t Count)
{
	Word* word = MakePrimitiveWord(name, CA);
	if (word)
	{
		// back up to rewrite CodeBody termination to append more Words
		DecPtr(&DictionaryPtr);

		for (size_t i = 0; i < Count; i++)
		{
			CopyObject(Value, DictionaryPtr.Ptr);
			IncPtr(&DictionaryPtr);
		}

		// NULLObject terminate so it can be traversed to find the end
		Object null;
		MakeNULLObject(&null);
		CopyObject(&null, DictionaryPtr.Ptr);
		IncPtr(&DictionaryPtr);
	}

	return word;
}


void* _Zero()
{
	Object i;
	MakeIntObject(&i, 0);

	Push(&DataStackPtr, &i);

	return _Next;
}

void* _One()
{
	Object i;
	MakeIntObject(&i, 1);

	Push(&DataStackPtr, &i);

	return _Next;
}

void* _Two()
{
	Object i;
	MakeIntObject(&i, 2);

	Push(&DataStackPtr, &i);

	return _Next;
}

void AddObject(Object* a, Object* b, Object* r)
{
	MakeIntObject(r, a->i + b->i);
}

void MinusObject(Object* a, Object* b, Object* r)
{
	MakeIntObject(r, a->i - b->i);
}

void* _Plus()
{
	Object a;
	Object b;

	LocalPop(&DataStackPtr, &a);
	LocalPop(&DataStackPtr, &b);

	Object i;
	AddObject(&b, &a, &i);

	Push(&DataStackPtr, &i);

	return _Next;
}

void* _Minus()
{
	Object a;
	Object b;

	LocalPop(&DataStackPtr, &a);
	LocalPop(&DataStackPtr, &b);

	Object i;
	MinusObject(&b, &a, &i);

	Push(&DataStackPtr, &i);

	return _Next;
}

void* _Mul()
{
	Object a;
	Object b;

	LocalPop(&DataStackPtr, &a);
	LocalPop(&DataStackPtr, &b);

	Object i;
	MakeIntObject(&i, b.i * a.i);

	Push(&DataStackPtr, &i);

	return _Next;
}


static Object OneObject = { .i = 1 };

void* _OnePlus()
{
	Object a;

	LocalPop(&DataStackPtr, &a);
	
	Object i;
	AddObject(&a, &OneObject, &i);

	Push(&DataStackPtr, &i);

	return _Next;
}


void* _OneMinus()
{
	Object a;

	LocalPop(&DataStackPtr, &a);
	
	Object i;
	MinusObject(&a, &OneObject, &i);

	Push(&DataStackPtr, &i);

	return _Next;
}

void* _And()
{
	Object a;
	Object b;

	LocalPop(&DataStackPtr, &a);
	LocalPop(&DataStackPtr, &b);

	Object i;
	MakeIntObject(&i, a.i & b.i);

	Push(&DataStackPtr, &i);

	return _Next;
}


void* _Dup()
{
	Push(&DataStackPtr, DataStackPtr.Ptr);	// push the current object to duplicate

	return _Next;
}

void* _TwoDup()
{
	Object p;
	LocalCopyObject(&DataStackPtr, &p);
	
	DecPtr(&p);
	Push(&DataStackPtr, p.Ptr);	
	
	IncPtr(&p);
	Push(&DataStackPtr, p.Ptr);

	return _Next;
}


void* _Drop()
{
	DecPtr(&DataStackPtr);	// simplest form of drop is to just decrement the stack pointer

	return _Next;
}

void* _TwoDrop()
{
	DecPtr(&DataStackPtr);	// simplest form of drop is to just decrement the stack pointer
	DecPtr(&DataStackPtr);

	return _Next;
}


void* _Swap()
{
	Object a;
	Object b;

	// TODO: this is a lot of copying to intermediates but not easy to eliminate since it replaces the top items
	LocalPop(&DataStackPtr, &a);
	LocalPop(&DataStackPtr, &b);

	Push(&DataStackPtr, &a);
	Push(&DataStackPtr, &b);

	return _Next;
}

void* _Over()
{
	Object p;
	LocalCopyObject(&DataStackPtr, &p);
	
	DecPtr(&p);
	Push(&DataStackPtr, p.Ptr);	// push the object to duplicate

	return _Next;
}

void* _Pick()
{
	Object i;
	LocalPop(&DataStackPtr, &i);

	Object p;
	LocalCopyObject(&DataStackPtr, &p);

	while (i.i)	// pointer walking is the most painful aspect of the Object system
	{
		DecPtr(&p);
		i.i--;
	};


	Push(&DataStackPtr, p.Ptr);	// push the object to duplicate

	return _Next;
}

void* _At()
{
	Object p;
	LocalPop(&DataStackPtr, &p);

	Push(&DataStackPtr, p.Ptr);	// push the object to dereference

	return _Next;
}

void* _Bang()
{
	Object p;
	LocalPop(&DataStackPtr, &p);

	// copy just the data directly (this leaves the Object linking in-place)
	CopyObject(DataStackPtr.Ptr, p.Ptr);
	DecPtr(&DataStackPtr);

	return _Next;
}

void* _Ast_Hash()
{
	Object a;
	LocalCopyObject(InstructionAddress.Ptr, &a);

	IncPtr(&InstructionAddress);
	Push(&DataStackPtr, &a);

	return _Next;
}

void* _Ast_Do()
{
	Object index;
	Object limit;

	// TODO: this might be faster with direct DS to RS copying (and Ptr dec) - no intermediate copies needed - but they are out of order :(
	LocalPop(&DataStackPtr, &index);
	LocalPop(&DataStackPtr, &limit);

	Push(&ReturnStackPtr, &limit);
	Push(&ReturnStackPtr, &index);

	return _Next;
}

void* _Ast_Loop()
{
	return _DoLoop();
}

void* _I()
{
	Push(&DataStackPtr, ReturnStackPtr.Ptr);
	return _Next;
}

void* _Ast_If()
{
	return _DoConditionalRelativeJump();
}


void* _Ast_Else()
{
	return _DoRelativeJump();
}


void* _Greater()
{
	Object a;
	Object b;

	LocalPop(&DataStackPtr, &a);
	LocalPop(&DataStackPtr, &b);

	Object r;
	MakeIntObject(&r, (b.i > a.i ? 1 : 0));
	Push(&DataStackPtr, &r);

	return _Next;
}

void InitWords()
{
	// Start of system Primitives
	SEMI = MakePrimitiveWord("SEMI", _Semi);

	TERMINATE = MakePrimitiveWord("TERMINATE", _Terminate);

	ZERO = MakePrimitiveWord("0", _Zero);
	ONE = MakePrimitiveWord("1", _One);
	TWO = MakePrimitiveWord("2", _Two);

	PLUS = MakePrimitiveWord("+", _Plus);
	MINUS = MakePrimitiveWord("-", _Minus);
	MUL = MakePrimitiveWord("*", _Mul);
	
	ONEPLUS = MakePrimitiveWord("1+", _OnePlus);
	ONEMINUS = MakePrimitiveWord("1-", _OneMinus);

	AND = MakePrimitiveWord("AND", _And);

	DUP = MakePrimitiveWord("DUP", _Dup);
	TWODUP = MakePrimitiveWord("2DUP", _TwoDup);
	DROP = MakePrimitiveWord("DROP", _Drop);
	TWODROP = MakePrimitiveWord("2DROP", _TwoDrop);
	SWAP = MakePrimitiveWord("SWAP", _Swap);
	OVER = MakePrimitiveWord("OVER", _Over);
	PICK = MakePrimitiveWord("PICK", _Pick);

	AT = MakePrimitiveWord("@", _At);
	BANG = MakePrimitiveWord("!", _Bang);

	AST_HASH = MakePrimitiveWord("*#", _Ast_Hash);

	AST_DO = MakePrimitiveWord("*DO", _Ast_Do);
	AST_LOOP = MakePrimitiveWord("*LOOP", _Ast_Loop);
	I = MakePrimitiveWord("I", _I);

	AST_IF = MakePrimitiveWord("*IF", _Ast_If);
	AST_ELSE = MakePrimitiveWord("*ELSE", _Ast_Else);

	GREATER = MakePrimitiveWord(">", _Greater);

	// End of system Primitives



	// test words

	MakeSortTest();

	Object TEST1_CODEBODY[] =
	{
		// Test SORT
		{ .Word = SORT },
		{ .Word = TERMINATE },
	};

	TEST1 = MakeSecondaryWord("TEST1", _Colon, TEST1_CODEBODY, countof(TEST1_CODEBODY));

}

void MakeSortTest()
{
	Object a;

	// constants and variable for RAND
	MakeIntObject(&a, 37377);
	RAND1 = MakeVariable("RAND1", _Constant, &a);

	SetIntObject(&a, 1309);
	RAND2 = MakeVariable("RAND2", _Constant, &a);

	SetIntObject(&a, 13849);
	RAND3 = MakeVariable("RAND3", _Constant, &a);

	SetIntObject(&a, 65535);
	RAND4 = MakeVariable("RAND4", _Constant, &a);

	SetIntObject(&a, 37377);
	SEED = MakeVariable("SEED", _Variable, &a);
	
	MakeRAND();

	// array for integers to sort
	size_t size = 6000;
	MakeIntObject(&a, (integer)size);
	SIZEC = MakeVariable("SIZE", _Constant, &a);

	// create INTS as a variable that has space for SIZEC Objects (i.e. an array for the integers)
	SetIntObject(&a, 0);
	INTS = MakeArray("INTS", _Variable, &a, size);

	MakeINIT();

	MakeSORT();
}


void MakeRAND()
{
	// RAND generates a psuedo random number
	Object RAND_CODEBODY[] =
	{
		{ .Word = SEED },
		{ .Word = AT },
		{ .Word = RAND2 },
		{ .Word = MUL },
		{ .Word = RAND3 },
		{ .Word = PLUS },
		{ .Word = RAND4 },
		{ .Word = AND },
		{ .Word = SEED },
		{ .Word = BANG },
		{ .Word = SEED },
		{ .Word = AT },
		{ .Word = SEMI }
	};

	RAND = MakeSecondaryWord("RAND", _Colon, RAND_CODEBODY, countof(RAND_CODEBODY));
}

void MakeINIT()
{
	// INIT fills variable INTS with random integers
	Object INIT_CODEBODY[] =
	{
		{ .Word = RAND1 },
		{ .Word = SEED },
		{ .Word = BANG },
		{ .Word = INTS },
		{ .Word = SIZEC },
		{ .Word = ZERO },
		{ .Word = AST_DO },
		{ .Word = RAND },
		{ .Word = OVER },
		{ .Word = BANG },
		{ .Word = ONEPLUS },
		{ .Word = AST_LOOP },
		{ .i = -5 },
		{ .Word = DROP },
		{ .Word = SEMI }
	};

	INIT = MakeSecondaryWord("INIT", _Colon, INIT_CODEBODY, countof(INIT_CODEBODY));
}


void MakeSORT()
{
	// SORT - INITs the array with random integers and then sorts it
	Object SORT_CODEBODY[] =
	{
		{ .Word = INIT },
		{ .Word = SIZEC },
		{ .Word = ONEMINUS },
		{ .Word = ZERO },
		{ .Word = AST_DO },
		{ .Word = INTS },
		{ .Word = DUP },
		{ .Word = ONEPLUS },
		{ .Word = SIZEC },
		{ .Word = ONEMINUS },
		{ .Word = I },
		{ .Word = MINUS },
		{ .Word = ZERO },
		{ .Word = AST_DO },
		{ .Word = OVER },	
		{ .Word = AT },
		{ .Word = OVER },
		{ .Word = AT },
		{ .Word = TWODUP },
		{ .Word = GREATER },
		{ .Word = AST_IF },
		{ .i = 10 },
		{ .Word = AST_HASH },
		{ .i = 3 },
		{ .Word = PICK },
		{ .Word = BANG },
		{ .Word = ONE },
		{ .Word = PICK },
		{ .Word = BANG },
		{ .Word = AST_ELSE },
		{ .i = 2 },
		{ .Word = TWODROP },
		{ .Word = ONEPLUS },
		{ .Word = SWAP },
		{ .Word = ONEPLUS },
		{ .Word = SWAP },
		{ .Word = AST_LOOP },
		{ .i = -23 },
		{ .Word = TWODROP },
		{ .Word = AST_LOOP },
		{ .i = -35 },
		{ .Word = SEMI }
	};

	SORT = MakeSecondaryWord("SORT", _Colon, SORT_CODEBODY, countof(SORT_CODEBODY));
}


