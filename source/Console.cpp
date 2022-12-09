#ifdef _WIN32

#include "stdafx.h"
#include "Console.h"

namespace LaplataRayTracer
{
	//
	void WriteImpl(CString& str)
	{
		HANDLE hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD d0, d1;
		::WriteConsole(hOutput, str.GetBuffer(), str.GetLength(), &d0, &d1);
	}

	//
	OutputConsole::OutputConsole()
	{
		::AllocConsole();
	}
	OutputConsole::~OutputConsole()
	{
		::FreeConsole();
	}

	void OutputConsole::Write(const char *p)
	{
		CString str = (TCHAR *)p;
		WriteImpl(str);
	}

	//
	OutputConsole g_Console;

}

#endif // _WIN32
