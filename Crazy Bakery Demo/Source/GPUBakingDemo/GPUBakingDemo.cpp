// GPUBakingDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GPUBakingDemo.h"

#include <sstream>
#include <commdlg.h>		//用于OPENFILENAME

#include "GPUBake.h"
#include "SceneLoader.h"
#include "Image.h"
#include "File.h"
#include "String.h"
#include "Dump.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
HWND m_hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

ICrazyBakery* g_pCrazyBakery = NULL;
IScene* g_pScene = NULL;
wchar_t g_strExePath[256];
std::wstring g_strAssetPath;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//void Split(const std::wstring& strSource, const std::wstring& c, std::vector<std::wstring>& vOutput)
//{
//	std::wstring::size_type pos1, pos2;
//	size_t len = strSource.length();
//	pos2 = strSource.find(c);
//	pos1 = 0;
//	while (std::wstring::npos != pos2)
//	{
//		vOutput.emplace_back(strSource.substr(pos1, pos2 - pos1));
//
//		pos1 = pos2 + c.size();
//		pos2 = strSource.find(c, pos1);
//	}
//	if (pos1 != len)
//		vOutput.emplace_back(strSource.substr(pos1));
//}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GPUBAKINGDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GPUBAKINGDEMO));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GPUBAKINGDEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GPUBAKINGDEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   m_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!m_hWnd)
   {
      return FALSE;
   }

   ShowWindow(m_hWnd, nCmdShow);
   UpdateWindow(m_hWnd);

   RegisterDumpFuncs();//向烘焙工程注册所有的Dump函数

   g_pCrazyBakery = CreateCrazyBakery();
   if (!(g_pCrazyBakery && g_pCrazyBakery->Init()))
   {
	   if (g_pCrazyBakery)
	   {
		   g_pCrazyBakery->Close();
		   delete g_pCrazyBakery;
		   g_pCrazyBakery = NULL;
	   }
	   
	   MessageBox(NULL, L"Init CrazyBakery failed!", L"", MB_OK);
   }

   //获取资源路径
   ::GetCurrentDirectoryW(256, g_strExePath);
   g_strAssetPath = g_strExePath;
   size_t iPos = g_strAssetPath.find_last_of(L"\\");
   g_strAssetPath = g_strAssetPath.substr(0, iPos + 1);
   g_strAssetPath += L"Data\\";

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
			switch (wmId)
			{
			case IDM_OPENSCENE:
			{
				wchar_t szFilePath[1024] = { 0 };   // 所选择的文件最终的路径
				OPENFILENAME ofn = { 0 };
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = m_hWnd;
				ofn.lpstrFilter = L"json文件(*.json)\0";//要选择的文件后缀   
				ofn.lpstrInitialDir = L"./";//默认的文件路径   
				ofn.lpstrFile = szFilePath;//存放文件的缓冲区   
				ofn.nMaxFile = sizeof(szFilePath) / sizeof(*szFilePath);
				ofn.nFilterIndex = 0;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER; //标志如果是多选要加上OFN_ALLOWMULTISELECT 
				if (!GetOpenFileName(&ofn))//调用完毕这行代码后，当前路径就变成了场景文件所在的目录
				{
					break;
				}

				//加载场景
				if (g_pScene)
				{
					g_pCrazyBakery->ReleaseScene(g_pScene->GetFileName());//手动释放场景，避免重复使用导致问题
				}
				g_pScene = g_pCrazyBakery->GetScene(szFilePath);
				CSceneLoader cLoader;
				cLoader.Load(szFilePath, g_pScene);
				cLoader.PrepareData(g_pScene);

				std::wstring strText(L"Crazy Bakery Demo. Openned scene is ");
				strText += g_pScene->GetFileName();
				SetWindowText(hWnd, strText.c_str());
			}
				break;
			case IDM_BAKE:
			{
				if (NULL == g_pScene)
				{
					MessageBox(NULL, L"Please open one scene first.", L"", MB_OK);
					break;
				}

				{
					//修改软件标题
					wchar_t strWindowText[256];
					GetWindowText(hWnd, strWindowText, 256);
					std::wstring strText(L"Crazy Bakery Demo. Openned scene is ");
					strText += g_pScene->GetFileName();
					strText += L"   ";
					strText += L"Begin to bake......";
					SetWindowText(hWnd, strText.c_str());
				}

				//设置烘焙选项
				IBakeOption* pOption = g_pCrazyBakery->GetBakeOption();
//				pOption->Resolution = { 512,512 };
				pOption->filterRadius = 5;
			//	pOption->maxRayBounce = 4;
				std::vector<size_t> vSampleNumberInOverture;
				vSampleNumberInOverture.push_back(64);
				vSampleNumberInOverture.push_back(16);
				vSampleNumberInOverture.push_back(4);
				pOption->SetSampleNumbersInOverture(vSampleNumberInOverture.size(), vSampleNumberInOverture.data());
				pOption->SetSampleNumbersInBeauty(1024);
				pOption->m_boDumpOvertureDirectLight = false;
				pOption->m_boDumpOvertureIndirectLight = false;
//				pOption->m_boDumpObj = true;
//				pOption->m_boOutputGammaTexture = true;

				ITimer* pTimer = g_pCrazyBakery->GetTimer();
				pTimer->Reset();

				//设置输出数据
				std::vector<ILightmap*> vLightmaps;
				for (size_t i=0;i< g_pScene->GetScattererNumber();++i)
				{
					const IScatterer* pScatterer = g_pScene->GetScatterer(i);
					if (pScatterer->m_boGenGI)
					{
						ILightmap* pLMData = g_pCrazyBakery->GetLightmapData(i);
						pLMData->m_texWidth = pScatterer->m_texWidth;
						pLMData->m_texHeight = pScatterer->m_texHeight;
						pLMData->SetOutputPath(pScatterer->GetOutputPath());//烘焙完毕会使用路径名判断将哪个IScatterer里的烘焙数据存储到哪个CLightmapData里
						vLightmaps.push_back(pLMData);
					}
				}

				//烘焙，烘焙，烘焙
				bool boBaked = false;
				boBaked = g_pCrazyBakery->Bake(g_pScene, pOption, vLightmaps.data(), vLightmaps.size());//GPUBake(g_pScene, pOption, vLightmaps.data(), vLightmaps.size());

				float TimeElapsed = pTimer->GetElapsedTime();
				std::wstringstream ssTime;
				ssTime << TimeElapsed;
			//	std::wstring strShow(L"Time used(In Seconds): ");
			//	strShow += ssTemp.str();
			//	MessageBox(NULL, strShow.c_str(), L"", MB_OK);

				{
					std::wstring strText(L"Crazy Bakery Demo. Openned scene is ");
					strText += g_pScene->GetFileName();
					strText += L"   ";
					strText += L"Baking is finished, ";
					strText += L"time used: ";
					strText += ssTime.str();
					strText += L" seconds.";
					SetWindowText(hWnd, strText.c_str());
				}

				//输出Lightmap
				bool boDumpLightmap = true;
				if (boBaked && boDumpLightmap)
				{
					ImageRGBf image;

					for (size_t i = 0; i < vLightmaps.size(); ++i)
					{
						ILightmap* pLD = vLightmaps[i];
						MyFloat4* buffer = pLD->GetBuffer();

						image.Resize(pLD->m_texWidth, pLD->m_texHeight);
						for (int y = 0, k = 0; y < pLD->m_texHeight; y++)
							for (int x = 0; x < pLD->m_texWidth; x++, k++)
							{
								if (bool boGamma = true)
								{
									image.SetPixel(x, y, float3{ pow(buffer[k].x,1 / 2.2f), pow(buffer[k].y,1 / 2.2f), pow(buffer[k].z,1 / 2.2f) });
								}
								else
								{
									image.SetPixel(x, y, float3{ buffer[k].x, buffer[k].y, buffer[k].z });
								}
							}

						File fi;
						fi.m_channel = File::RGB;
						fi.m_format = File::u8; //File::f32;
						fi.m_path = pLD->GetOutputPath();
						fi.Save(image);
					}
				}

				//输出obj
				bool boDumpObj = true;
				if (boBaked && boDumpObj && g_pScene->GetScattererNumber())
					g_pDumpMergedScatterer(g_pScene, GetParent(g_pScene->GetScatterer(0)->GetOutputPath()).c_str(), L".png");
				
				for (size_t i = 0; i < vLightmaps.size(); ++i)
				{
					g_pCrazyBakery->ReleaseLightmapData(i);//手动释放，避免重复使用导致问题
				}
				vLightmaps.clear();

				g_pCrazyBakery->ReleaseBakeOption();
			}
				
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		if (g_pCrazyBakery)
		{
			g_pCrazyBakery->Close();
			delete g_pCrazyBakery;
		}

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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
