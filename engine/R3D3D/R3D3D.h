#ifndef R3D3D_H
#define R3D3D_H

#include <windows.h>
#include <vector>
#include "..\R3Renderer\R3RenderDevice.h"

#define DXUT_AUTOLIB
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTsettingsdlg.h"
#include "SDKmesh.h"
#include "SDKmisc.h"

struct R3Dialog
{
	CDXUTDialog *dialog;
	int x;
	int y;
	int width;
	int height;
	R3COLOR colorTopLeft;
	R3COLOR colorTopRight;
	R3COLOR colorBottomLeft;
	R3COLOR colorBottomRight;
	R3Dialog()
	{
		dialog=new CDXUTDialog();
	}
};
struct R3Text
{
	int x;
	int y;
	DWORD color;
	LPCWSTR text;
	R3Text( int cx, int cy, DWORD ccolor, LPCWSTR ctext )
	{
		x=cx;
		y=cx;
		color=ccolor;
		text=ctext;
	}
};
struct R3Font
{
	int Height;
	int Width;
	int Weight;
	int MipLevels;
	bool Italic;
	DWORD CharSet;
	DWORD OutputPrecision;
	DWORD Quality;
	DWORD PitchAndFamily;
	LPCTSTR pFacename;
	ID3DXFont* pFont;
	std::vector<R3Text *>	Text;
	R3Font( int height, int width, int weight, int wiplevels, bool italic, DWORD charset, DWORD outputprecision, DWORD quality, DWORD pitchandfamily, LPCTSTR pfacename )
	{
		Height=height;
		Width=width;
		Weight=Weight;
		MipLevels=wiplevels;
		Italic=italic;
		CharSet=charset;
		OutputPrecision=outputprecision;
		Quality=quality;
		PitchAndFamily=pitchandfamily;
		pFacename=pfacename;
		pFont=NULL;
	}
};
class R3D3DDialogManager:public R3DialogManager
{
protected:
	LPDIRECT3DDEVICE9	m_pDevice;
	R3Element	m_R3Element;
	LPONEVENT   m_OnEvent;
public:
	ID3DXFont*				m_pFont;
	ID3DXSprite*			m_pSprite;
	std::vector<R3Font *>	m_vR3Font;
	std::vector<R3Dialog *>	m_vR3Dialog;
	CDXUTDialogResourceManager m_DialogResourceManager;
	CD3DSettingsDlg         m_SettingsDlg;
	R3D3DDialogManager(LPDIRECT3DDEVICE9 pDevice);
	~R3D3DDialogManager();
	void AddStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault );
	void AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault );
	void AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault );
	void AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault );
	void AddComboBox( int ID, int x, int y, int width, int height, UINT nHotKey, bool bIsDefault );
	void AddSlider( int ID, int x, int y, int width, int height, int min, int max, int value, bool bIsDefault );
	void AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault );
	void AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle );

	void AddItem( int DID, int ID, const WCHAR* strText, void* pData );
	void SetChecked( int DID, int ID, bool bChecked, bool bClearGroup );

	void SetLocation( int ID, int x, int y );
	void SetSize( int ID, int width, int height );
	void SetBackgroundColors( int ID, R3COLOR colorTopLeft, R3COLOR colorTopRight, R3COLOR colorBottomLeft, R3COLOR colorBottomRight );

	int AddDialog();
	void SetCallBack(LPONEVENT callbackfuc);
	static void WINAPI StaticOnEvent( UINT nEvent, int nControlID, 
                                            CDXUTControl* pControl, void* pUserData );
	void SettingDlg();

	int AddFont( int height, int width, int weight, int wiplevels, bool italic, DWORD charset, DWORD outputprecision, DWORD quality, DWORD pitchandfamily, LPCTSTR pfacename );
	void AddText( int ID, int x, int y, DWORD color, LPCWSTR text );
};

class R3D3D : public R3RenderDevice 
{
public:
	R3D3D(HINSTANCE hDLL);
	~R3D3D();
	HRESULT Init( bool isWindows, int width, int height );
    void Release();
	R3DialogManager* GetR3DialogManager(){return m_pD3DDialogManager;};
	void ToggleFullScreen(){DXUTToggleFullScreen();};
	void ToggleREF(){DXUTToggleREF();};
	LPCWSTR GetDeviceStats(){return DXUTGetDeviceStats();};
	R3D3DDialogManager			*m_pD3DDialogManager;

private:
	LPDIRECT3D9					m_pDC; // Used to create the D3DDevice
	LPDIRECT3DDEVICE9			m_pDevice; // Our rendering device

};



extern "C"{
	HRESULT __declspec(dllexport) CreateRenderDevice(HINSTANCE hDLL, R3RenderDevice **pDevice);
	HRESULT __declspec(dllexport) ReleaseRenderDevice(R3RenderDevice **pDevice);
}

extern R3D3D *g_R3D3D;

#endif


