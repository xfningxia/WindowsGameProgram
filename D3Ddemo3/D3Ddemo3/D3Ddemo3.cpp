//-----------------------------------【头文件包含部分】---------------------------------------
//	描述：包含程序所依赖的头文件
//------------------------------------------------------------------------------------------------
#include <d3d9.h>
#include <windows.h>
#include <tchar.h>
#include<D3dx9core.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")


//-----------------------------------【宏定义部分】--------------------------------------------
//	描述：定义一些辅助宏
//------------------------------------------------------------------------------------------------
#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	L"D3Ddemo2"		//为窗口标题定义的宏
#define SAFE_RELEASE(p){if (p){(p)->Release();(p)=NULL;}} //定义安全释放宏


//-----------------------------------【全局结构体】--------------------------------------------

//顶点缓存使用1、设计顶点格式
struct CUSTOMVERTEX
{
	FLOAT x,y,z, rhw;
	DWORD color;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE) //FVF灵活顶点格式



//-----------------------------------【全局函数声明部分】-------------------------------------
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );  //窗口过程函数
HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
VOID Direct3D_Render(HWND hwnd);
VOID Direct3D_CleanUp();
float Get_FPS();

//-----------------------------------【全局变量声明部分】-------------------------------------
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; //Direct3D设备对象
ID3DXFont* g_pFont = NULL; //字体com接口
float g_FPS = 0.0f;
wchar_t g_strFPS[50];
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL; //顶点缓冲区对象



//-----------------------------------【WinMain( )函数】--------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{
	//【1】窗口创建四步曲之一：开始设计一个完整的窗口类
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra		= 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra		= 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon=(HICON)::LoadImage(NULL,L"icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    //指定窗口类的光标句柄。
	wndClass.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个灰色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"D3Ddemo2";		//用一个以空终止的字符串，指定窗口类的名字。

	//【2】窗口创建四步曲之二：注册窗口类
	if( !RegisterClassEx( &wndClass ) )				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;		

	//【3】窗口创建四步曲之三：正式创建窗口
	HWND hwnd = CreateWindow( L"D3Ddemo2",WINDOW_TITLE,		//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL );

	
	if (!(S_OK == Direct3D_Init(hwnd)))
	{
		MessageBox(hwnd,L"初始化失败",L"提示",0);
	}
	

	//【4】窗口创建四步曲之四：窗口的移动、显示与更新
	MoveWindow(hwnd,250,80,WINDOW_WIDTH,WINDOW_HEIGHT,true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow( hwnd, nShowCmd );    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	//PlaySound(L"",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
	//MessageBox(hwnd,L"",L"",0);

	//【5】消息循环过程
	MSG msg = { 0 };		//定义并初始化msg
	while( msg.message != WM_QUIT )			//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage( &msg );		//将虚拟键消息转换为字符消息
			DispatchMessage( &msg );			//分发一个消息给窗口程序。
		}else
		{
			Direct3D_Render(hwnd);
		}
	}

	//【6】窗口类的注销
	UnregisterClass(L"D3Ddemo2", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;  
}


//-----------------------------------【WndProc( )函数】--------------------------------------
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )      
{
	switch( message )						//switch语句开始
	{
	case WM_PAINT:						// 若是客户区重绘消息
		Direct3D_Render(hwnd);
		ValidateRect(hwnd, NULL);		// 更新客户区的显示
		break;									//跳出该switch语句

	case WM_KEYDOWN:                // 若是键盘按下消息
		if (wParam == VK_ESCAPE)    // 如果被按下的键是ESC
			DestroyWindow(hwnd);		// 销毁窗口, 并发送一条WM_DESTROY消息
		break;									//跳出该switch语句

	case WM_DESTROY:				//若是窗口销毁消息
		PostQuitMessage( 0 );		//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;								//跳出该switch语句

	default:									//若上述case条件都不符合，则执行该default语句
		return DefWindowProc( hwnd, message, wParam, lParam );		//调用缺省的窗口过程
	}

	return 0;			//正常退出
}

//Direct3D_Init
//Direct3D的初始化
HRESULT Direct3D_Init(HWND hwnd)
{
	//1、创建Direct3D接口对象
	LPDIRECT3D9 pD3D = NULL;
	if(NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))	//初始化Direct3D接口对象，并进行DirectX版本协商
		return S_FALSE;

	//2、获取硬件设备信息
	D3DCAPS9 caps; int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps)))
	{
		return E_FAIL;
	}
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//3、填充D3DPRESENT_PARAMETERS结构体
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


	//4、创建Direct3D设备接口
	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,vp,&d3dpp,&g_pd3dDevice)))
		return E_FAIL;

	SAFE_RELEASE(pD3D)

		if(!(S_OK == Objects_Init(hwnd)))
			return E_FAIL;

	return S_OK;
}


//Object_Init
//渲染资源初始化
HRESULT Objects_Init(HWND hwnd)
{
	if(FAILED(D3DXCreateFont(g_pd3dDevice,36,0,0,1,FALSE,DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,0,_T("微软雅黑"),&g_pFont)))
		return E_FAIL;

	srand(timeGetTime());

	//顶点缓存使用2、创建顶点缓存
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(6*sizeof(CUSTOMVERTEX),0,D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT,&g_pVertexBuffer,NULL)))
	{
		return E_FAIL;
	}

	//顶点缓存使用3、访问顶点缓存
	//顶点数据的设置

	//两个随机三角形
// 	CUSTOMVERTEX vertices[]=
// 	{
// 		{300.0f,100.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
// 		{500.0f,100.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
// 		{300.0f,300.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
// 		{300.0f,300.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
// 
// 		{(float)(800.0*rand()/RAND_MAX+1.0),(float)(600.0*rand()/(RAND_MAX+1.0)),
// 		0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
// 
// 		{(float)(800.0*rand()/RAND_MAX+1.0),(float)(600.0*rand()/(RAND_MAX+1.0)),
// 		0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
// 	};

	//两个三角形拼成矩形
	CUSTOMVERTEX vertices[]=
	{
		{300.0f,100.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
		{500.0f,100.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
		{300.0f,200.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
		{500.0f,100.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
		{300.0f,200.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
		{500.0f,200.0f,0.0f,1.0f,D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256),},
	};



	//填充顶点缓冲区
	VOID* pVertices;
	if(FAILED(g_pVertexBuffer->Lock(0,sizeof(vertices),(void**)&pVertices,0)))
		return E_FAIL;

	memcpy(pVertices,vertices,sizeof(vertices));
	
	g_pVertexBuffer->Unlock();

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,false);
	

	return S_OK;
}


//Direct3D_Render
//使用Direct3D进行渲染
VOID Direct3D_Render(HWND hwnd)
{
	//1、清屏
	g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0),1.0f,0);

	RECT formatRect;
	GetClientRect(hwnd,&formatRect);

	//2、开始绘制
	g_pd3dDevice->BeginScene();


	//3、正式绘制

	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);//设置渲染状态

	//顶点缓存使用4、绘制图形
	g_pd3dDevice->SetStreamSource(0,g_pVertexBuffer,0,sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);


	//显示fps
	int charCount = swprintf_s(g_strFPS,20,_T("FPS:%0.3f"),Get_FPS());
	g_pFont->DrawText(NULL,g_strFPS,charCount,&formatRect,DT_TOP|DT_RIGHT,D3DCOLOR_XRGB(255,30,100));





	//4、结束绘制
	g_pd3dDevice->EndScene();

	//5、翻转
	g_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}

//Direct3D_CleanUp
//资源清理
VOID Direct3D_CleanUp()
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pd3dDevice);
}

//Get_FPS()  用于计算FPS
float Get_FPS()
{
	static float fps = 0;
	static int frameCount = 0;
	static float currentTime = 0.0f;
	static float lastTime = 0.0f;

	frameCount++;
	currentTime = timeGetTime()*0.001f;

	if (currentTime - lastTime > 1.0f)
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}

	return fps;
}