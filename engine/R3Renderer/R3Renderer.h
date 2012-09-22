#ifndef R3RENDERER_H
#define R3RENDERER_H

#define DIRECT3D 0
#define OPENGL 1

#include "R3RenderDevice.h"
class R3Renderer
{

	public:
		R3Renderer(HINSTANCE hInst);
		~R3Renderer(void);
		HRESULT				CreateDevice(int);
		void				Release();
		LPR3RENDERDEVICE	GetDevice() { return (LPR3RENDERDEVICE)m_pDevice; }
		HINSTANCE			GetModule() { return m_hDLL;    }

	private:
		R3RenderDevice	*m_pDevice;
		HMODULE			m_hDLL;
};
typedef class R3Renderer *LPR3RENDERER;
typedef HRESULT (*CREATERENDERDEVICE)(HINSTANCE hDLL, R3RenderDevice **pInterface);
typedef HRESULT (*RELEASERENDERDEVICE)(R3RenderDevice **pInterface);

#endif