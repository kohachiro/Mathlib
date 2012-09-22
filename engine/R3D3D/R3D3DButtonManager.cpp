#include "R3D3D.h"

CDXUTDialogResourceManager g_DialogResourceManager;

R3D3DButtonManager::R3D3DButtonManager(LPDIRECT3DDEVICE9 pDevice)
{
	m_nNumR3Button	= 0;
	m_pR3Button		= NULL;	
	m_pDevice       = pDevice;
	m_R3Dialog.Init( &g_DialogResourceManager );
}
R3D3DButtonManager::~R3D3DButtonManager()
{

}
HRESULT R3D3DButtonManager::AddButton( UINT ID, LPCWSTR strText, INT x, INT y, UINT width, UINT height)
{
	m_R3Dialog.AddButton(ID,strText,x,y,width,height);
	return S_OK;
}
