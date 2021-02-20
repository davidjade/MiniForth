#pragma once

#include <minmax.h>
#include <memory.h>
#include <stddef.h>


typedef unsigned char byte;
// typedef short int integer;
typedef int integer;


#pragma pack(push,1)

typedef void* (*CodeAddress)();

struct _Word;	// forward declaration

typedef struct _Object
{
	union
	{
		integer i;
		struct _Object* Ptr;
		struct _Word* Word;
		CodeAddress CA;
		byte* NativePtr;		// this is used for direct native memory access and variable Object* manipulation
	};
} Object;


typedef struct _Word
{
	char Name[10];
	byte Flags;
	struct _Word* Next;
#pragma warning(suppress:4200)
	Object CodeBody[];
} Word;

#pragma pack(pop)


#define MAX_OBJECT_SIZE (sizeof(Object))


inline int GetObjectSize(Object* Object) { return sizeof(Object); }
inline int GetPreviousObjectSize(Object* Object) { return sizeof(Object); }


inline void CopyObject(Object* Source, Object* Dest) { memcpy(Dest, Source, GetObjectSize(Source)); }
inline void LocalCopyObject(Object* Source, Object* Dest) { memcpy(Dest, Source, sizeof(Object)); }


inline void SetNULLObject(Object* Object) { Object->NativePtr = NULL; }
inline void SetIntObject(Object* Object, integer Value) { Object->i = Value; }
inline void SetObjectPtr(Object* ObjectPtr, Object* Object) { ObjectPtr->Ptr = Object; }
inline void SetWordRefObject(Object* Object, Word* Word) { Object->Word = Word; }
inline void SetCodeAddressObject(Object* Object, CodeAddress CA) { Object->CA = CA; }


inline void MakeNULLObject(Object* Object) { Object->NativePtr = NULL; }
inline void MakeIntObject(Object* Object, integer Value) { SetIntObject(Object, Value); }
inline void MakeObjectPtr(Object* ObjectPtr, Object* Object) { SetObjectPtr(ObjectPtr, Object); }
inline void MakeWordRefObject(Object* Object, Word* Word) { SetWordRefObject(Object, Word); }
inline void MakeCodeAddressObject(Object* Object, CodeAddress CA) { SetCodeAddressObject(Object, CA); }


inline void IncPtr(Object* Ptr) { Ptr->NativePtr += GetObjectSize(Ptr->Ptr); }
inline void DecPtr(Object* Ptr) { Ptr->NativePtr -= GetPreviousObjectSize(Ptr->Ptr); }


