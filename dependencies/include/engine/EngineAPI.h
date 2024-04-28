//Used to ensure other projects can access functionality marked with the ENGINE_API macro
//This enables usage of the functionality through the dynamically linked library Engine.dll which is generated from this Engine project
#pragma once

#pragma warning(push)
#pragma warning(disable: 4005) //Disables the macro-redefinition error since redefining this macro is desired to enable exposing functionality through the dynamically linked library


#ifdef ENGINE_BUILD_DLL
	#define ENGINE_API __declspec(dllexport)
#else
	#define ENGINE_API __declspec(dllimport)
#endif
#pragma warning(pop)