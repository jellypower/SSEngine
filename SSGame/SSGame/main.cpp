#include "pch.h"

#include <SSCollision/Public/ModuleEntry/SSCollisionGlobalVariableSet.h>
#include <SSEngineDefault/Public/WindowManager/IWindowManager.h>


#include "Resource.h"



#include "SSBuildSettings.h"

#include "ModuleEntryScriptRunner.h"


#include "SSEngineDefault/Public/RawInput/IRawInputProcessor.h"
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"
#include "SSEngineDefault/Public/RawInput/RawInputUtils.h"
#include "SSEngineDefault/Public/WindowManager/WindowUtils.h"
#include "SSEngineDefault/Public/SSContainer/SSString/FixedStringW.h"
#include "SSEngineDefault/Public/TestCodes/TestFunctions.h"

#include "SSGame/SSGame.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"


#define MAX_LOADSTRING 100


WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];


HRESULT					InitWindow(HINSTANCE, int, RECT);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK		About(HWND, UINT, WPARAM, LPARAM);

void					AnalyzeCommandLineArgs();




int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{

	AnalyzeCommandLineArgs();

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SSGAME, szWindowClass, MAX_LOADSTRING);



	// 1. ======================================================================
	RunLoadLibraries();
	RunModuleEntryScript();
	// ======================================================================

	if (FAILED(InitWindow(hInstance, nCmdShow, g_WndRect)))
	{
		__debugbreak();
		return FALSE;
	}
	g_hInst = hInstance;

	// 2. ======================================================================
	RunModuleEntryScriptPostInitWindow(g_hInst, g_hWnd, false, false);
	// ======================================================================






	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SSGAME));


	MSG msg = { 0 };

	g_FrameInfoProcessor->StartUpXXX();


	g_Game = DBG_NEW SSGame(g_Renderer, g_CollDevice);

	g_Game->StartupEngine();
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_RawInputProcessor->ProcessInputStartOfFrame();
			g_FrameInfoProcessor->PerFrameXXX();
			g_Game->EnginePerFrame();
			g_RawInputProcessor->ProcessInputEndOfFrame();
			g_MainWindowManager->ProcessWindowEndOfFrame();
		}
	}
	g_Game->CleanupEngine();

	// End Of Loop
	{
		delete g_Game;
		g_Game = nullptr;
	}


	RunModuleExitScript();

	// Resource Leak Check
	if (_CrtDumpMemoryLeaks()) SS_INTERRUPT();
	if (_CrtCheckMemory() == false) SS_INTERRUPT();

	RunUnloadLibraries();
	return (int)msg.wParam;

	// _CrtDumpMemoryLeaks 함수는 내가 직접 호출하지 않아도 exe가 종료될 때 알아서 호출된다.
	// 그런데 메모리 Leak이 있는데 위에서 RunUnloadLibraries을 통해 모듈을 언로드 해줬기 때문에
	// exe가 종료될 때 호출되는 _CrtDumpMemoryLeaks함수는 크래시를 낼 수 있다.
}


HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, RECT WindowSize)
{

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SSGAME));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SSGAME); // Win32로 창 만들어서 등록
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// RegisterClass를 통해 화면에 어떤 객체를 올릴지 클래스를 "등록"
	if (!RegisterClassExW(&wcex)) {
		__debugbreak();
		return E_FAIL;
	}


	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	AdjustWindowRect(&WindowSize, WS_OVERLAPPEDWINDOW, FALSE);

	int32 Width = WindowSize.right - WindowSize.left;
	int32 Height = WindowSize.bottom - WindowSize.top;

	// 위에서 RegisterClass를 통해 등록한 클래스를 "생성"해서 윈도우 창 생성
	HWND hWnd = CreateWindow(szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height
		, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		__debugbreak();
		return E_FAIL;
	}


	ShowWindow(hWnd, nCmdShow);
	// nCmdShow: 윈도우 및 컨트롤의 표시/숨김 여부 설정과 최대화, 최소화 여부 설정


	UpdateWindow(hWnd);

	g_hWnd = hWnd;


	return S_OK;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_ACTIVATE:
		//		g_RawInputProcessor->ResetCurInputState();
		Win32ProcessInputEvent(g_RawInputProcessor, hWnd, message, wParam, lParam);
		Win32ProcessWindowEvent(g_MainWindowManager, hWnd, message, wParam, lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSELEAVE:
	{
		Win32ProcessInputEvent(g_RawInputProcessor, hWnd, message, wParam, lParam);
		break;
	}


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void AnalyzeCommandLineArgs()
{
	int32 argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

	for (int32 i = 1; i < argc; i++)
	{
		SS::FixedStringW<100> Key;
		SS::FixedStringW<100> Value;


		utf16* splitStart = wcschr(argv[i], L'=');
		if (splitStart != nullptr)
		{
			uint32 newStrlen = splitStart - argv[i];

			Key = argv[i];
			Key.CutOut(newStrlen);
			Value = (splitStart + 1);
		}
		else
		{
			Key = argv[i];
		}


		if (wcscmp(Key.C_Str(), L"g_exportSSMaterial") == 0)
		{
			if (wcscmp(Value.C_Str(), L"true") == 0)
			{
				// TODO: Export 기능 구현 25/01/31
				// SSFBXImporter::g_exportSSMaterial = true;
			}
		}
		else if (wcscmp(Key.C_Str(), L"g_WndRect") == 0)
		{
			swscanf(Value.C_Str(), L"{%d,%d,%d,%d}",
				&g_WndRect.left, &g_WndRect.top, &g_WndRect.right, &g_WndRect.bottom);
		}

	}

}