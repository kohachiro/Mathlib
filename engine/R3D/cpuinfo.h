#ifndef  _CPUINFO_H
#define  _CPUINFO_H
#include "Singleton.h"

namespace R3
{
	class CPUINFO:public Singleton<CPUINFO>
	{
	public:
		bool IsAA64;
		bool Is3DNOW;
		bool Is3DNOW2;
		bool IsMMXExt;

		bool IsHTT;
		bool IsMMX;
		bool IsSSE;
		bool IsSSE2;
		bool IsSSE3;
		bool IsSSSE3;
		bool IsIA64;
		bool IsEM64T;

		int Core;
		friend class Singleton<CPUINFO>;
	private:
		CPUINFO()
		{
			IsAA64=false;
			IsEM64T=false;
			unsigned long ext,oedx,oecx,core,amdcore;
			_asm
			{	
				//MMX SSE SSE2 SSE3
				mov eax, 1
				cpuid
				mov oecx,ecx
				mov oedx,edx
				//CPUINFO EXT
				mov eax, 0x80000001
				cpuid
				mov ext ,edx
				//AMD Core
				mov eax, 0x80000008
				cpuid
				mov amdcore ,ecx
				//Intel Core
				mov eax ,4
				mov ecx ,0
				cpuid
				mov core ,eax
			}
			IsHTT=(( oedx & 1<<28 ) != 0);
			IsMMX=(( oedx & 1<<23 ) != 0);
			IsSSE=(( oedx & 1<<25 ) != 0);
			IsSSE2=(( oedx & 1<<26 ) != 0);
			IsSSE3=(( oecx & 1<<0 ) != 0);
			IsSSSE3=(( oecx & 1<<9 ) != 0);
			IsIA64=(( oecx & 1<<30 ) != 0);
			
			Is3DNOW=(( ext & 1<<31 ) != 0);
			Is3DNOW2=(( ext & 1<<30 ) != 0);
			IsMMXExt=(( ext & 1<<22 ) != 0);
			if (Is3DNOW)
			{
				Core=(amdcore+1);
				IsAA64=(( ext & 1<<29 ) != 0);
			}	
			else
			{
				IsEM64T=(( ext & 1<<29 ) != 0);
				Core=(((core & 0xFC000000) >> 26 )+1);
			}
		
		}

	};
}
#endif