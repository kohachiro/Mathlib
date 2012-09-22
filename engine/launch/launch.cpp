#include "launch.h"

#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_COMBOBOX            9
#define IDC_CHECKBOX            14

LPR3RENDERER		g_pRenderer= NULL;
LPR3RENDERDEVICE	g_pDevice  = NULL;
HINSTANCE			g_hInst	= NULL;

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	return ProgramStartup(DIRECT3D);
}

HRESULT ProgramStartup(UINT iAPI)
{
	g_pRenderer = new R3Renderer(g_hInst);
	F_RETURN( g_pRenderer->CreateDevice(iAPI) );
	if ((g_pDevice = g_pRenderer->GetDevice())==NULL)
		return E_FAIL;
	F_RETURN(GUIInit());
	return g_pDevice->Init( true, 800, 600 );
}
HRESULT ProgramCleanup()
{
	return  S_OK;
}
HRESULT GUIInit()
{
	int iY = 10; 
	R3DialogManager *pDialogManager=g_pDevice->GetR3DialogManager();
	pDialogManager->AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 ,0 ,false );
	pDialogManager->AddButton( IDC_TOGGLEREF, L"Toggle REF", 35, iY += 24, 125, 22  ,0 ,false );
	pDialogManager->AddButton( IDC_CHANGEDEVICE, L"Change Device", 35, iY += 24, 125, 22 ,0 ,false );
	pDialogManager->SetCallBack(GUIEvent);
	int iHUD=pDialogManager->AddDialog();

	pDialogManager->SetLocation( iHUD, 600, 0);
	pDialogManager->SetSize( iHUD, 0, 170 );
	pDialogManager->AddStatic( 10, L"Static 1", 35, iY += 24, 125, 22 ,false );
	pDialogManager->AddComboBox( 19, 35, iY += 24, 125, 22 ,0 ,false );

	pDialogManager->AddCheckBox( 21, L"Checkbox1", 35, iY += 24, 125, 22 ,false ,0 ,false );
	pDialogManager->AddCheckBox( 11, L"Checkbox2", 35, iY += 24, 125, 22 ,false ,0 ,false );
	pDialogManager->AddRadioButton( 12, 1, L"Radio1G1", 35, iY += 24, 125, 22 ,false ,0 ,false );
	pDialogManager->AddRadioButton( 13, 1, L"Radio2G1", 35, iY += 24, 125, 22 ,false ,0 ,false );
	pDialogManager->AddRadioButton( 14, 1, L"Radio3G1", 35, iY += 24, 125, 22 ,false ,0 ,false );
	pDialogManager->AddButton( 17, L"Button1", 35, iY += 24, 125, 22 ,0 ,false );
	pDialogManager->AddButton( 18, L"Button2", 35, iY += 24, 125, 22 ,0 ,false );
	pDialogManager->AddRadioButton( 15, 2, L"Radio1G2", 35, iY += 24, 125, 22 ,false ,0 ,false );
	pDialogManager->AddRadioButton( 16, 2, L"Radio2G3", 35, iY += 24, 125, 22 ,false ,0 ,false );

	pDialogManager->AddSlider( 20, 50, iY += 24, 100, 22 ,0 ,100 ,50 ,false );
	pDialogManager->AddEditBox( 20, L"Test", 35, iY += 24, 125, 32 ,false );
	int iSampleUI=pDialogManager->AddDialog();
	pDialogManager->AddItem( iSampleUI, 19, L"Text1", NULL);
	pDialogManager->AddItem( iSampleUI, 19, L"Text2", NULL);
	pDialogManager->AddItem( iSampleUI, 19, L"Text3", NULL);
	pDialogManager->SetChecked( iSampleUI, 16, true, true );
	pDialogManager->SetChecked( iSampleUI, 14, true,true );
	pDialogManager->SetLocation( iSampleUI, 600, 150);
	pDialogManager->SetSize( iSampleUI, 0, 300 );

	int iFont = pDialogManager->AddFont (15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial");
	pDialogManager->AddText( iFont, 5, 5, 0xFFFFFF00, g_pDevice->GetDeviceStats());
	

	return  S_OK;
}
void CALLBACK GUIEvent(UINT nEvent, int nControlID)
{
	switch(nControlID)
	{
	case IDC_TOGGLEFULLSCREEN:
		g_pDevice->ToggleFullScreen();
		break;
	case IDC_TOGGLEREF:
		g_pDevice->ToggleREF();
		break;
	case IDC_CHANGEDEVICE:
		g_pDevice->GetR3DialogManager()->SettingDlg();
		//g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() );
		break;
	default:
		break;
	
	}

}