#pragma once

#ifdef _WIN32

namespace LaplataRayTracer
{
	class OutputConsole
	{
	public:
		OutputConsole();
		~OutputConsole();

		void Write(const char *p);


	//	void Write(CString& str);

	};

	//
	extern OutputConsole g_Console;
}

#endif // _WIN32
