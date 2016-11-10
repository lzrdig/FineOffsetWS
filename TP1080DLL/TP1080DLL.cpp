// TP1080DLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "WSapi.h"



#if defined(TP1080DLL_EXPORT) // inside DLL
#   define TP1080DLL   __declspec(dllexport)
#else // outside DLL
#   define TP1080DLL   __declspec(dllimport)
#endif  // TP1080DLL_EXPORT





extern "C" TP1080DLL ISingleton & GetSingleton();




ISingleton & GetSingleton()
{
	static CWSapi inst;
	return inst;
}