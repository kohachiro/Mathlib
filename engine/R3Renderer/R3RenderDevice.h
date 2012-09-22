#ifndef R3RENDERDEVICE_H
#define R3RENDERDEVICE_H

#include <windows.h>
#include <stdio.h>
#include <vector>

typedef DWORD R3COLOR;
#define R3COLOR_ARGB(a,r,g,b) \
    ((R3COLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))


struct R3Static
{
	int ID;
	LPCWSTR text;
	int	x;
	int	y;
	int	width;
	int	height;
	bool bIsDefault;
	R3Static(int id, LPCWSTR strText, int cx, int cy, int cw, int ch,bool bi)
	{
		ID=id;
		text=strText;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		bIsDefault=bi;
	}
};
struct R3Button 
{
	int ID;
	LPCWSTR text;
	int	x;
	int	y;
	int	width;
	int	height;
	UINT nHotkey;
	bool bIsDefault;
	R3Button( int id, LPCWSTR strText, int cx, int cy, int cw, int ch,UINT nh, bool bi)
	{
		ID=id;
		text=strText;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		nHotkey=nh;
		bIsDefault=bi;
	}
};
struct R3CheckBox
{
	int ID;
	LPCWSTR text;
	int	x;
	int	y;
	int	width;
	int	height;
	bool bChecked;
	UINT nHotkey;
	bool bIsDefault;
	R3CheckBox( int id, LPCWSTR strText, int cx, int cy, int cw, int ch,bool bc,UINT nh, bool bi)
	{
		ID=id;
		text=strText;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		bChecked=bc;
		nHotkey=nh;
		bIsDefault=bi;
	}
};
struct R3RadioButton
{
	int ID;
	UINT nButtonGroup;
	LPCWSTR text;
	int	x;
	int	y;
	int	width;
	int	height;
	bool bChecked;
	UINT nHotkey;
	bool bIsDefault;
	R3RadioButton( int id,UINT nb, LPCWSTR strText, int cx, int cy, int cw, int ch,bool bc,UINT nh, bool bi)
	{
		ID=id;
		nButtonGroup=nb;
		text=strText;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		bChecked=bc;
		nHotkey=nh;
		bIsDefault=bi;
	}
};
struct R3ComboBox
{
	int ID;
	int	x;
	int	y;
	int	width;
	int	height;
	UINT nHotkey;
	bool bIsDefault;
	R3ComboBox( int id, int cx, int cy, int cw, int ch, UINT nh, bool bi)
	{
		ID=id;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		nHotkey=nh;
		bIsDefault=bi;
	}
};
struct R3Slider
{
	int ID;
	int	x;
	int	y;
	int	width;
	int	height;
	int min;
	int max;
	int value;
	bool bIsDefault;
	R3Slider( int id, int cx, int cy, int cw, int ch, int mn, int mx, int v, bool bi)
	{
		ID=id;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		min=mn;
		max=mx;
		value=v;
		bIsDefault=bi;
	}
};
struct R3EditBox
{
	int ID;
	LPCWSTR text;
	int	x;
	int	y;
	int	width;
	int	height;
	bool bIsDefault;
	R3EditBox( int id, LPCWSTR strText, int cx, int cy, int cw, int ch, bool bi)
	{
		ID=id;
		text=strText;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		bIsDefault=bi;
	}
};
struct R3ListBox
{
	int ID;
	int	x;
	int	y;
	int	width;
	int	height;
	DWORD dwStyle;
	R3ListBox( int id, int cx, int cy, int cw, int ch, DWORD dw)
	{
		ID=id;
		x=cx;
		y=cy;
		width=cw;
		height=ch;
		dwStyle=dw;
	}
};
struct R3Element
{
	std::vector<R3Static *>			m_vR3Static;
	std::vector<R3Button *>			m_vR3Button;
	std::vector<R3CheckBox *>		m_vR3CheckBox;
	std::vector<R3RadioButton *>	m_vR3RadioButton;
	std::vector<R3ComboBox *>		m_vR3ComboBox;
	std::vector<R3Slider *>			m_vR3Slider;
	std::vector<R3EditBox *>		m_vR3EditBox;
	std::vector<R3ListBox *>		m_vR3ListBox;
};

typedef void (CALLBACK* LPONEVENT)( UINT nEvent, int nControlID);
class R3DialogManager
{
protected:

public:
	R3DialogManager(){};
	virtual ~R3DialogManager(){};
	virtual void AddStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault )=0;
	virtual void AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault )=0;
	virtual void AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault)=0;
	virtual void AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault)=0;
	virtual void AddComboBox( int ID, int x, int y, int width, int height, UINT nHotKey, bool bIsDefault )=0;
	virtual void AddSlider( int ID, int x, int y, int width, int height, int min, int max, int value, bool bIsDefault )=0;
	virtual void AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault )=0;
	virtual void AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle )=0;
	
	virtual void AddItem( int DID, int ID, const WCHAR* strText, void* pData )=0;
	virtual void SetChecked( int DID, int ID, bool bChecked, bool bClearGroup )=0;

	virtual void SetLocation( int ID, int x, int y )=0;
	virtual void SetSize( int ID, int width, int height )=0;
	virtual void SetBackgroundColors( int ID, R3COLOR colorTopLeft, R3COLOR colorTopRight, R3COLOR colorBottomLeft, R3COLOR colorBottomRight )=0;

	virtual int AddDialog()=0;

	virtual void SetCallBack(LPONEVENT callbackfuc)=0;
	virtual void SettingDlg()=0;

	virtual int AddFont( int height, int width, int weight, int wiplevels, bool italic, DWORD charset, DWORD outputprecision, DWORD quality, DWORD pitchandfamily, LPCTSTR pfacename )=0;
	virtual void AddText( int ID, int x, int y, DWORD color, LPCWSTR text )=0;
};


class R3RenderDevice 
{
	public:
		R3RenderDevice() {};
		virtual ~R3RenderDevice(){};
		virtual HRESULT Init( bool isWindows, int width, int height )=0;
		virtual void Release()=0;
		virtual R3DialogManager* GetR3DialogManager()=0;
		virtual void ToggleFullScreen()=0;
		virtual void ToggleREF()=0;
		virtual LPCWSTR GetDeviceStats()=0;
	protected:
		HINSTANCE				m_hDLL;					// dll module handle
		FILE					*m_pLog;					// log file
};
typedef class R3RenderDevice *LPR3RENDERDEVICE;
#endif