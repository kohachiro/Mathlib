#include "R3D3D.h"

void R3D3DDialogManager::SetCallBack(LPONEVENT callbackfuc)
{
	m_OnEvent=callbackfuc;
}

void WINAPI R3D3DDialogManager::StaticOnEvent( UINT nEvent, int nControlID, 
                                            CDXUTControl* pControl, void* pUserData )
{
    R3D3DDialogManager* pR3D3DDialogManager = (R3D3DDialogManager*) pUserData;
    if( pR3D3DDialogManager && pR3D3DDialogManager->m_OnEvent )
        pR3D3DDialogManager->m_OnEvent( nEvent, nControlID);
}
void R3D3DDialogManager:: SettingDlg()
{
	m_SettingsDlg.SetActive( !m_SettingsDlg.IsActive() );
}
R3D3DDialogManager::R3D3DDialogManager(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice	= pDevice;
	m_OnEvent	= NULL;
	m_pSprite	= NULL;
	//m_DialogResourceManager	= NULL;
	//m_SettingsDlg			= NULL;
	m_SettingsDlg.Init( &m_DialogResourceManager );
}
R3D3DDialogManager::~R3D3DDialogManager()
{		
	for (std::vector<R3Dialog *>::iterator i=m_vR3Dialog.begin();i!=m_vR3Dialog.end();i++){
		delete (*i)->dialog;
		delete (*i);
	}
	m_vR3Dialog.clear();
}

////////////////////////////////


void R3D3DDialogManager::SetLocation(int ID, int x, int y )
{
	m_vR3Dialog[ID]->x=x;
	m_vR3Dialog[ID]->y=y;
}
void R3D3DDialogManager::SetSize(int ID, int width, int height )
{
	m_vR3Dialog[ID]->width=width;
	m_vR3Dialog[ID]->height=height;
}
void R3D3DDialogManager::SetBackgroundColors(int ID, R3COLOR colorTopLeft, R3COLOR colorTopRight, R3COLOR colorBottomLeft, R3COLOR colorBottomRight )
{
	m_vR3Dialog[ID]->colorTopLeft=colorTopLeft;
	m_vR3Dialog[ID]->colorTopRight=colorTopRight;
	m_vR3Dialog[ID]->colorBottomLeft=colorBottomLeft;
	m_vR3Dialog[ID]->colorBottomRight=colorBottomRight;
}

////////////////////////////////

int R3D3DDialogManager::AddDialog()
{
	R3Dialog *m_pR3Dialog=new R3Dialog();
	m_pR3Dialog->dialog->Init( &m_DialogResourceManager );
	for (std::vector<R3Static *>::iterator i = m_R3Element.m_vR3Static.begin();i != m_R3Element.m_vR3Static.end(); ++i){
		m_pR3Dialog->dialog->AddStatic((*i)->ID,(*i)->text,(*i)->x,(*i)->y,(*i)->width,(*i)->height,(*i)->bIsDefault);
		delete (*i);
	}
	m_R3Element.m_vR3Static.clear();
	for (std::vector<R3Button *>::iterator i = m_R3Element.m_vR3Button.begin();i != m_R3Element.m_vR3Button.end(); ++i){
		m_pR3Dialog->dialog->AddButton((*i)->ID,(*i)->text,(*i)->x,(*i)->y,(*i)->width,(*i)->height,(*i)->nHotkey,(*i)->bIsDefault);
		delete (*i);
	}
	m_R3Element.m_vR3Button.clear();
	for (std::vector<R3CheckBox *>::iterator i = m_R3Element.m_vR3CheckBox.begin();i != m_R3Element.m_vR3CheckBox.end(); ++i){
		m_pR3Dialog->dialog->AddCheckBox((*i)->ID,(*i)->text,(*i)->x,(*i)->y,(*i)->width,(*i)->height,(*i)->bChecked,(*i)->nHotkey,(*i)->bIsDefault);
		delete (*i);
	}
	m_R3Element.m_vR3CheckBox.clear();
	for (std::vector<R3RadioButton *>::iterator i = m_R3Element.m_vR3RadioButton.begin();i != m_R3Element.m_vR3RadioButton.end(); ++i){
		m_pR3Dialog->dialog->AddRadioButton((*i)->ID,(*i)->nButtonGroup,(*i)->text,(*i)->x,(*i)->y,(*i)->width,(*i)->height,(*i)->bChecked,(*i)->nHotkey,(*i)->bIsDefault);
		delete (*i);
	}
	m_R3Element.m_vR3RadioButton.clear();
	for (std::vector<R3ComboBox *>::iterator i = m_R3Element.m_vR3ComboBox.begin();i != m_R3Element.m_vR3ComboBox.end(); ++i){
		m_pR3Dialog->dialog->AddComboBox((*i)->ID,(*i)->x,(*i)->y,(*i)->width,(*i)->height, (*i)->nHotkey,(*i)->bIsDefault);
		delete (*i);
	}
	m_R3Element.m_vR3ComboBox.clear();
	for (std::vector<R3Slider *>::iterator i = m_R3Element.m_vR3Slider.begin();i != m_R3Element.m_vR3Slider.end(); ++i){
		m_pR3Dialog->dialog->AddSlider((*i)->ID,(*i)->x,(*i)->y,(*i)->width,(*i)->height, (*i)->min, (*i)->max, (*i)->value,(*i)->bIsDefault);
		delete (*i);
	}
	m_R3Element.m_vR3Slider.clear();
	for (std::vector<R3EditBox *>::iterator i = m_R3Element.m_vR3EditBox.begin();i != m_R3Element.m_vR3EditBox.end(); ++i){
		m_pR3Dialog->dialog->AddEditBox((*i)->ID,(*i)->text,(*i)->x,(*i)->y,(*i)->width,(*i)->height, (*i)->bIsDefault);
		delete (*i);
	}
	m_R3Element.m_vR3EditBox.clear();
	for (std::vector<R3ListBox *>::iterator i = m_R3Element.m_vR3ListBox.begin();i != m_R3Element.m_vR3ListBox.end(); ++i){
		m_pR3Dialog->dialog->AddListBox((*i)->ID,(*i)->x,(*i)->y,(*i)->width,(*i)->height, (*i)->dwStyle);
		delete (*i);
	}
	m_R3Element.m_vR3ListBox.clear();
	m_pR3Dialog->dialog->SetCallback(StaticOnEvent,(void *)this);
	m_vR3Dialog.push_back(m_pR3Dialog);
	return (int)m_vR3Dialog.size()-1;
}

//////////////////////////////////////////

void R3D3DDialogManager::AddStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault )
{
	R3Static *m = new R3Static( ID, strText, x, y, width, height, bIsDefault );
	m_R3Element.m_vR3Static.push_back(m);
}
void R3D3DDialogManager::AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault )
{
	R3Button *m = new R3Button(ID,strText,x,y,width,height,nHotkey,bIsDefault);
	m_R3Element.m_vR3Button.push_back(m);
}
void R3D3DDialogManager::AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault )
{
	R3CheckBox *m = new R3CheckBox( ID, strText, x, y, width, height, bChecked, nHotkey, bIsDefault );
	m_R3Element.m_vR3CheckBox.push_back(m);
}
void R3D3DDialogManager::AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault )
{
	R3RadioButton *m = new R3RadioButton( ID, nButtonGroup, strText, x, y, width, height, bChecked, nHotkey, bIsDefault );
	m_R3Element.m_vR3RadioButton.push_back(m);
}
void R3D3DDialogManager::AddComboBox( int ID, int x, int y, int width, int height, UINT nHotKey, bool bIsDefault )
{
	R3ComboBox *m = new R3ComboBox( ID, x, y, width, height, nHotKey, bIsDefault );
	m_R3Element.m_vR3ComboBox.push_back(m);
}
void R3D3DDialogManager::AddSlider( int ID, int x, int y, int width, int height, int min, int max, int value, bool bIsDefault )
{
	R3Slider *m = new R3Slider( ID, x, y, width, height, min, max, value, bIsDefault );
	m_R3Element.m_vR3Slider.push_back(m);
}
void R3D3DDialogManager::AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault )
{
	R3EditBox *m = new R3EditBox( ID, strText, x, y, width, height, bIsDefault );
	m_R3Element.m_vR3EditBox.push_back(m);
}
void R3D3DDialogManager::AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle )
{
	R3ListBox *m = new R3ListBox( ID, x, y, width, height, dwStyle );
	m_R3Element.m_vR3ListBox.push_back(m);
}


////////////////////////////////

void R3D3DDialogManager::AddItem( int DID, int ID, const WCHAR* strText, void* pData )
{
	m_vR3Dialog[DID]->dialog->GetComboBox(ID)->AddItem( strText, pData );
}

void R3D3DDialogManager::SetChecked( int DID, int ID, bool bChecked, bool bClearGroup )
{
	m_vR3Dialog[DID]->dialog->GetRadioButton(ID)->SetChecked( bChecked, bClearGroup );
}

////////////////////////////////

int  R3D3DDialogManager::AddFont( int height, int width, int weight, int wiplevels, bool italic, DWORD charset, DWORD outputprecision, DWORD quality, DWORD pitchandfamily, LPCTSTR pfacename )
{
	R3Font* pR3Font= new R3Font( height, width, weight, wiplevels, italic, charset, outputprecision, quality, pitchandfamily, pfacename );
	m_vR3Font.push_back( pR3Font );
	return (int)m_vR3Font.size() - 1;
}

void  R3D3DDialogManager::AddText( int ID, int x, int y, DWORD color, LPCWSTR text )
{
	R3Text* pText=new R3Text( x, y, color, text );
	m_vR3Font[ID]->Text.push_back( pText );
}