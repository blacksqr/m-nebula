#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_windisp.cc -- Win32 Display-Initialisierung
//
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------

#include "kernel/ntypes.h"

#ifdef __WIN32__
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "gfx/nglserver.h"
#include "input/ninputserver.h"

static nGlServer *_global_gls;	// nur innerhalb CreateWindow() gueltig!
extern nKey nwin32_TranslateKey(int);

int ngl_multisampleformat(HDC hdc, nGlServer *gls)
{
	n_assert(hdc);
	if (isExtensionSupported("WGL_ARB_multisample") && wglChoosePixelFormatARB) 
	{
		int samples = 64;
		// These Attributes Are The Bits We Want To Test For In Our Sample
		// Everything Is Pretty Standard, The Only One We Want To 
		// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
		// These Two Are Going To Do The Main Testing For Whether Or Not
		// We Support Multisampling On This Hardware.
		int iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, gls->disp_bpp/4*3,
			WGL_ALPHA_BITS_ARB, gls->disp_bpp/4,
			WGL_DEPTH_BITS_ARB, gls->disp_zbufbits,
			WGL_STENCIL_BITS_ARB, gls->disp_sbufbits,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, samples,
			0, 0
		};

		int		pixelFormat = 0;
		UINT	numFormats = 0;
		float	fAttributes[] = { 0, 0 };		

		int valid = true;
		while (valid && !numFormats && samples > 0) 
		{
			valid = wglChoosePixelFormatARB(hdc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
			if (valid && !numFormats )
			{
				samples -= 2;
				iAttributes[19] = samples;
			}			
		}
		if (numFormats && valid) 
		{
			gls->disp_samples = samples;
			return pixelFormat;
		}
		else
			return 0;
	}

	return 0;
}

//-------------------------------------------------------------------
//  ngl_setup_pixelformat()
//  13-Nov-98   floh    created
//  11-Jan-01   floh    try to get 32 bit zbuffer
//-------------------------------------------------------------------
static bool ngl_pixelformat(HDC hdc, int pf, PIXELFORMATDESCRIPTOR& pfd)
{
	n_assert(hdc);

	if (pf == -1 || pf == 0) 
	{    
		n_memset(&pfd,0,sizeof(pfd));
		pfd.nSize    = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags  = PFD_DRAW_TO_WINDOW | 
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = (uchar)GetDeviceCaps(hdc, BITSPIXEL);

		pfd.cDepthBits   = 32;
		pfd.cStencilBits = 8;

		while (!(pf = ChoosePixelFormat(hdc,&pfd)) && pfd.cDepthBits)
		{
			pfd.cDepthBits -= 8;
			if (!pfd.cDepthBits)
			{
				pfd.cDepthBits   = 32;
				pfd.cStencilBits = 0;
			}
		}

		if (!pf) 
		{
			n_warn("nGlServer: ChoosePixelFormat() failed!");
			return false;
		}
	} 
	else if (!DescribePixelFormat(hdc, pf, sizeof(pfd), &pfd))
	{
		return false;
	}

	if (!SetPixelFormat(hdc, pf, &pfd)) 
	{
		n_warn("nGlServer: SetPixelFormat() failed!");
		return false;
	}
	return true;
}

static bool ngl_setup_pixelformat(nGlServer *gls)
{
	n_assert(gls->hdc);
	PIXELFORMATDESCRIPTOR pfd;

	if (!ngl_pixelformat(gls->hdc, -1, pfd)) 
	{
		return false;	
	}
	else
	{
		gls->disp_bpp = pfd.cColorBits;
		gls->disp_zbufbits = pfd.cDepthBits;
		gls->disp_sbufbits = pfd.cStencilBits;
		return true;
	}
}


//-------------------------------------------------------------------

//  ngl_resize()
//  13-Nov-98   floh    created
//-------------------------------------------------------------------
static void ngl_resize(nGlServer *gls)
{
	n_assert(gls->hwnd);
	RECT r;
	GLsizei w,h;
	GetClientRect(gls->hwnd,&r);
	w = (GLsizei) r.right;
	h = (GLsizei) r.bottom;
	gls->_reshapeDisplay(w,h);
}

//-------------------------------------------------------------------
//  ngl_initText()
//  21-Feb-99   floh    created
//-------------------------------------------------------------------
static bool ngl_Text(HDC hdc, int text_listbase, int text_numglyphs, TEXTMETRIC& t_metric)
{
	n_assert(hdc);

	if (!GetTextMetrics(hdc,&t_metric)) {
		n_printf("ngl_initText(): GetTextMetrics() failed!\n");
		return false;
	}
	wglUseFontBitmaps(hdc, 0, text_numglyphs, text_listbase);
	return true;
}

static bool ngl_initText(nGlServer *gls)
{
	n_assert(gls);
	n_assert(gls->hdc);
	TEXTMETRIC t_metric;
	gls->text_initialized = false;
	gls->text_listbase  = 1000;
	gls->text_numglyphs = 256;

	if (!ngl_Text(gls->hdc, gls->text_listbase, gls->text_numglyphs, t_metric))
		return false;
	else
	{			
		gls->text_height = (float)t_metric.tmHeight;
		gls->text_initialized = true;
		return true;
	}
}
//-------------------------------------------------------------------
//  ngl_killText()
//  21-Feb-99   floh    created
//-------------------------------------------------------------------
static void ngl_killText(nGlServer *gls)
{
	if (gls->text_initialized) {
		glDeleteLists(gls->text_listbase,gls->text_numglyphs);
	}
}


//-------------------------------------------------------------------
//  ngl_WinProc
//  13-Nov-98   floh    created
//  01-Dec-98   floh    + Auswertung von Input-Messages
//  10-Sep-99   floh    + doh, during WM_DESTROY, the GL context has not
//                        released...
//  24-Dec-99   floh    + fullscreen support
//                      + accelerator support for Alt-Enter, Alt-Plus
//                        Alt-Minus
//  22-Jan-00   floh    + WM_SETFOCUS and WM_KILLFOCUS support
//-------------------------------------------------------------------
static LONG WINAPI ngl_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG retval = 1;
	PAINTSTRUCT ps;

	// user data of window contains 'this' pointer
	nGlServer *gls = (nGlServer *) GetWindowLong(hWnd,0);
	if (!gls) gls = _global_gls;

	switch (uMsg) {
		case WM_CREATE:
			gls->hdc = GetDC(hWnd);
			// pixel format
			if (ngl_setup_pixelformat(gls)) 
			{
				gls->hrc = wglCreateContext(gls->hdc);
				wglMakeCurrent(gls->hdc, gls->hrc);
				ngl_initText(gls);
			} 
			else 
			{
				DestroyWindow(hWnd);
			}
			break;			

		case WM_PAINT:
			BeginPaint(hWnd,&ps);
			SwapBuffers(gls->hdc);
			EndPaint(hWnd,&ps);
			break;        

		case WM_SIZE:
			ngl_resize(gls);
			break;                    

		case WM_SETFOCUS:
			if (gls->ref_is.isvalid()) gls->ref_is->ObtainFocus();
			break;

		case WM_KILLFOCUS:
			if (gls->ref_is.isvalid()) gls->ref_is->LoseFocus();
			break;

		case WM_DESTROY:
			// release gl context...
			if (gls->is_fullscreen) 
				ChangeDisplaySettings(0, 0);
			gls->invalidate_context();
			gls->hwnd = 0;
			gls->quit_requested = true;
			//PostQuitMessage(0);
			break;

		case WM_SETCURSOR:
			// show/hide the system and/or custom d3d cursor
			if (!gls->GetCursorShown())
			{
				SetCursor(NULL);
				return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case nGlServer::N_GLCMD_TOGGLEFULLSCREEN:
					{
						// toggle windowed/fullscreen 
						char mode_str[64];
						if (gls->is_fullscreen) {
							// switch to windowed
							sprintf(mode_str,"type(win)-w(%d)-h(%d)",gls->disp_w,gls->disp_h);
						} else {
							// switch to fullscreen
							sprintf(mode_str,"type(full)-w(%d)-h(%d)-bpp(%d)",gls->disp_w,gls->disp_h,gls->disp_bpp);
						}
						gls->CloseDisplay();
						gls->SetDisplayMode(mode_str);
						gls->OpenDisplay();
					}
					break;
				case nGlServer::N_GLCMD_NEXTMODE:
					{
						// switch to next fullscreen mode
						char mode_str[64];
						if (gls->nextDisplayMode(mode_str,sizeof(mode_str))) 
						{
							gls->CloseDisplay();
							gls->SetDisplayMode(mode_str);
							if (!gls->OpenDisplay()) 
							{
								gls->SetDisplayMode("type(full)-w(640)-h(480)");
								gls->OpenDisplay();
							}
						}
					}
					break;
				case nGlServer::N_GLCMD_PREVMODE:
					{
						// switch to previous fullscreen mode
						char mode_str[64];
						if (gls->prevDisplayMode(mode_str,sizeof(mode_str))) 
						{
							gls->CloseDisplay();
							gls->SetDisplayMode(mode_str);
							if (!gls->OpenDisplay()) {
								gls->SetDisplayMode("type(full)-w(640)-h(480)");
								gls->OpenDisplay();
							}
						}
					}
					break;
			}
			break;

		case WM_KEYDOWN:
			if (gls->ref_is.isvalid()) {
				nKey nk = nwin32_TranslateKey((int)wParam);
				nInputEvent *ie = gls->ref_is->NewEvent();
				if (ie) {
					ie->SetType(N_INPUT_KEY_DOWN);
					ie->SetDeviceId(N_INPUT_KEYBOARD(0));
					ie->SetKey(nk);
					gls->ref_is->LinkEvent(ie);
				}
			}
			break;

		case WM_KEYUP:
			if (gls->ref_is.isvalid()) {
				nKey nk = nwin32_TranslateKey((int)wParam);
				nInputEvent *ie = gls->ref_is->NewEvent();
				if (ie) {
					ie->SetType(N_INPUT_KEY_UP);
					ie->SetDeviceId(N_INPUT_KEYBOARD(0));
					ie->SetKey(nk);
					gls->ref_is->LinkEvent(ie);
				}
			}
			break;
		case WM_SYSKEYDOWN:			
		case WM_SYSKEYUP:
			break;
		case WM_CHAR:
			if (gls->ref_is.isvalid()) {
				nInputEvent *ie = gls->ref_is->NewEvent();
				if (ie) {
					ie->SetType(N_INPUT_KEY_CHAR);
					ie->SetDeviceId(N_INPUT_KEYBOARD(0));
					ie->SetChar((int) wParam);
					gls->ref_is->LinkEvent(ie);
				}
			}
			break;

		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			if (gls->ref_is.isvalid()) {
				nInputEvent *ie = gls->ref_is->NewEvent();
				if (ie) {
					short x = LOWORD(lParam);
					short y = HIWORD(lParam);
					switch(uMsg)
					{
					case WM_LBUTTONDBLCLK:
					case WM_RBUTTONDBLCLK:
					case WM_MBUTTONDBLCLK:
						ie->SetType(N_INPUT_BUTTON_DBLCLCK);
						break;
					case WM_LBUTTONDOWN:
					case WM_RBUTTONDOWN:
					case WM_MBUTTONDOWN:
						SetCapture(hWnd);
						ie->SetType(N_INPUT_BUTTON_DOWN);
						break;
					case WM_LBUTTONUP:
					case WM_RBUTTONUP:
					case WM_MBUTTONUP:
						ReleaseCapture();
						ie->SetType(N_INPUT_BUTTON_UP);
						break;
					}
					switch(uMsg) 
					{
					case WM_LBUTTONDBLCLK:
					case WM_LBUTTONDOWN:
					case WM_LBUTTONUP:
						ie->SetButton(0);
						break;
					case WM_RBUTTONDBLCLK:
					case WM_RBUTTONDOWN:
					case WM_RBUTTONUP:
						ie->SetButton(1);
						break;
					case WM_MBUTTONDBLCLK:
					case WM_MBUTTONDOWN:
					case WM_MBUTTONUP:
						ie->SetButton(2);
						break;
					}
					ie->SetDeviceId(N_INPUT_MOUSE(0));
					ie->SetAbsPos(x, y);
					float relX = float(x) / gls->disp_w;
					float relY = float(y) / gls->disp_h;
					ie->SetRelPos(relX, relY);
					gls->ref_is->LinkEvent(ie);
				}
			}
			break;

		case WM_MOUSEMOVE:
			if (gls->ref_is.isvalid()) {
				short x = LOWORD(lParam);
				short y = HIWORD(lParam);
				nInputEvent *ie = gls->ref_is->NewEvent();
				if (ie) {
					ie->SetType(N_INPUT_MOUSE_MOVE);
					ie->SetDeviceId(N_INPUT_MOUSE(0));
					ie->SetAbsPos(x, y);
					float relX = float(x) / gls->disp_w;
					float relY = float(y) / gls->disp_h;
					ie->SetRelPos(relX, relY);
					gls->ref_is->LinkEvent(ie);
				}
			}
			break;

		default:
			retval = DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
	}
	return retval;
}

//-------------------------------------------------------------------
//  classRegister()
//  14-Mar-99   floh    created
//  22-Jul-99   floh    + Support fuer Icon
//  30-Mar-00   floh    set hbrBackground to NULL_BRUSH to
//                      prevent the WM_ERASEBKGND message from
//                      being sent...
//-------------------------------------------------------------------
bool nGlServer::classRegister(void)
{
	n_assert(this->hinst);
	n_assert(!this->class_registered);

	WNDCLASS wndclass;
	HICON icon;
	HMODULE dll_module   = GetModuleHandle("nopengl.dll");
	if (dll_module) icon = LoadIcon((HINSTANCE)dll_module,"nebula_icon");
	else            icon = LoadIcon(NULL,IDI_APPLICATION);

	strcpy(this->wnd_classname,"nGlServer Class");
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC) ngl_WinProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = sizeof(void *);  // Userdata Size
	wndclass.hInstance     = this->hinst;
	wndclass.hIcon         = icon;
	wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = (const char *) &(this->wnd_classname);
	if (RegisterClass(&wndclass)) {
		this->class_registered = true;
		return true;
	} else {
		n_warn("RegisterClass() failed!");
		return false;
	}
}

//-------------------------------------------------------------------
//  classUnregister()
//  14-Mar-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::classUnregister(void)
{
	n_assert(this->hinst);
	n_assert(this->class_registered);
	this->class_registered = false;
	if (!UnregisterClass(this->wnd_classname,this->hinst)) {
		n_warn("UnregisterClass() failed!");
		return false;
	}
	return true;
}

//-------------------------------------------------------------------
//  winOpen()
//  14-Mar-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::winOpen(void)
{
	n_assert(this->hinst);
	n_assert(this->class_registered);
	n_assert(!this->hwnd);
	_global_gls = this;
	this->hwnd = CreateWindow((const char *) &(this->wnd_classname),
		"nGlServer Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		32, 32,
		NULL, NULL,
		this->hinst,
		NULL);
	if (this->hwnd) 
	{
		SetWindowLong(this->hwnd,0,(LONG)this);
		ShowWindow(this->hwnd, SW_MINIMIZE);
		UpdateWindow(this->hwnd);
		this->initGl();
		return true;
	} else {
		n_warn("CreateWindow() failed!");
		return false;
	}			
}

//-------------------------------------------------------------------
//  invalidate_context()
//  Invalidate (close) the OpenGL context, unload all resources
//  bound to the OpenGL context.
//  24-Nov-00   floh    created
//-------------------------------------------------------------------
void nGlServer::invalidate_context(void)
{
	// release all textures, vbuffers, pixelshader, meshes
	nGfxServer::releaseResources();    

	if (this->hrc) {
		ngl_killText(this);
		wglDeleteContext(this->hrc);
		this->hrc = 0;
	}
	if (this->hdc) {
		ReleaseDC(this->hwnd, this->hdc);
		this->hdc = 0;
	}
}

//-------------------------------------------------------------------
//  winClose()
//  14-Mar-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::winClose(void)
{
	this->invalidate_context();
	if (this->hwnd) {
		n_assert(this->hinst);
		n_assert(this->class_registered);
		DestroyWindow(this->hwnd);
		this->hwnd = NULL;
	}
	return true;
}
#pragma warning( push )
#pragma warning( disable : 4100 )
//-------------------------------------------------------------------
//  dispOpen()
//  14-Mar-99   floh    created
//  24-Dec-99   floh    + rewritten for fullscreen support
//  22-Jan-00   floh    + in fullscreen mode, mouse pointer is hidden
//-------------------------------------------------------------------
bool nGlServer::dispOpen(bool fullscreen, int w, int h, int bpp, int x, int y)
{
	n_assert(this->hwnd);
	n_assert(!this->display_open);

	glColorMask(1, 1, 1, 1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearStencil(this->stencil_clear);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	SwapBuffers(this->hdc);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	SwapBuffers(this->hdc);

	glClearColor(this->clear_color[0],
		this->clear_color[1],
		this->clear_color[2],
		this->clear_color[3]);

	this->is_fullscreen = fullscreen; 
	if (this->is_fullscreen) {
		// fullscreen mode
		DEVMODE dm;
		n_memset(&dm,0,sizeof(dm));
		dm.dmSize       = sizeof(dm);
		dm.dmPelsWidth  = w;
		dm.dmPelsHeight = h;
		dm.dmBitsPerPel = bpp;
		dm.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		ChangeDisplaySettings(&dm,CDS_FULLSCREEN);
		SetWindowLong(this->hwnd,GWL_STYLE,WS_POPUP);
		ShowWindow(this->hwnd,SW_RESTORE);
		UpdateWindow(this->hwnd);
		SetForegroundWindow(this->hwnd);
		SetFocus(this->hwnd);
		SetWindowPos(this->hwnd,HWND_TOPMOST, x, y, w, h,0);
		// wglMakeCurrent(this->hdc, this->hrc);
	} else {
		// windowed mode
		SetWindowLong(this->hwnd,GWL_STYLE,WS_OVERLAPPEDWINDOW);
		ShowWindow(this->hwnd,SW_RESTORE);
		UpdateWindow(this->hwnd);
		SetForegroundWindow(this->hwnd);
		SetFocus(this->hwnd);
		RECT r = {0, 0, w, h}; 
		AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, 0); 
		w = r.right - r.left;
		h = r.bottom - r.top;
		SetWindowPos(this->hwnd,HWND_NOTOPMOST, x, y, w, h, 0);
		SetWindowPos(this->hwnd,HWND_TOP, x, y, w, h, 0);
		// wglMakeCurrent(this->hdc, this->hrc);
	}

	if (this->cursorShown)
	{
		::ShowCursor(TRUE);
		::SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	else
	{
		::ShowCursor(FALSE);
		::SetCursor(NULL);
	}

	this->display_open = true;

	return true;
}
#pragma warning( pop )
//-------------------------------------------------------------------
/**
14-Mar-99   floh    created
24-Dec-99   floh    fullscreen support
22-Jan-00   floh    restore mouse pointer when coming from
fullscreen mode
*/
//-------------------------------------------------------------------
bool nGlServer::dispClose(void)
{
	n_assert(this->display_open);
	this->display_open = false;
	if (this->hwnd) {
		// if we've been fullscreen, restore desktop...
		if (this->is_fullscreen) {
			ChangeDisplaySettings(NULL,0);
			this->is_fullscreen = false;
		}
		if (!ShowWindow(this->hwnd,SW_MINIMIZE)) {
			n_printf("dispClose: ShowWindow(minimized) failed!\n");
			return false;
		}
	}
	::ShowCursor(TRUE);
	::SetCursor(LoadCursor(NULL, IDC_ARROW));

	return true;
}

//-------------------------------------------------------------------
/**
@brief  Enumerate available display modes and build database under
/sys/share/display/0/modes.

FIXME:
For now, only modes that match the current desktops bit depth
will be enumerated, because SetPixelFormat() in
ngl_setup_pixelformat() may only be called once per Window.
If we want to switch bpp's at runtime, we probably have to
destroy/recreate the window (and the gl context... etc.)

24-Dec-99   floh    created
+ ignores display modes which just differ
in hertz
28-Sep-00   floh    + PushCwd()/PopCwd(0
*/
//-------------------------------------------------------------------
void nGlServer::enumDisplayModes(void)
{
	this->enumDisplayModes(this->hdc);
}

void nGlServer::enumDisplayModes(HDC _hdc)
{
	n_assert(_hdc);

	int bpp_filter = GetDeviceCaps(_hdc,BITSPIXEL);

	kernelServer->PushCwd(this->ref_devdir.get());    

	nRoot *mode_root = kernelServer->New("nroot","modes");
	kernelServer->PushCwd(mode_root);

	DEVMODE dm;
	n_memset(&dm,0,sizeof(dm));
	dm.dmSize        = sizeof(dm);
	dm.dmDriverExtra = 0;
	int i = 0;
	int act_mode = 0;
	int last_w   = 0;
	int last_h   = 0;	
	int last_bpp = 0;

	while (EnumDisplaySettings(0, i++, &dm)) 
	{
		int w   = dm.dmPelsWidth;
		int h   = dm.dmPelsHeight;
		int bpp = dm.dmBitsPerPel;

		// ignore mode?
		if ((bpp >= 16) &&
			(bpp_filter==bpp) &&
			(!((w==last_w) && (h==last_h) && (bpp==last_bpp))))
		{
			nRoot *cur_mode_root;
			nEnv  *env;
			char buf[N_MAXPATH];

			sprintf(buf,"%d",act_mode++);
			cur_mode_root = kernelServer->New("nroot",buf);
			kernelServer->PushCwd(cur_mode_root);
            sprintf(buf,"type(full)-w(%d)-h(%d)-bpp(%d)",w,h,bpp);
			env = (nEnv*) kernelServer->New("nenv","name");
			env->SetS(buf);
			env = (nEnv*) kernelServer->New("nenv","w");
			env->SetI(w);
			env = (nEnv*) kernelServer->New("nenv","h");
			env->SetI(h);
			env = (nEnv*) kernelServer->New("nenv","bpp");
			env->SetI(bpp);
			kernelServer->PopCwd();
		}
		last_w   = w;
		last_h   = h;
		last_bpp = bpp;
	}
	kernelServer->PopCwd();
	kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
	@brief Show the mouse cursor.
*/
void
nGlServer::SetCursorShown(bool show)
{
	::ShowCursor(show);
	::SetCursor(LoadCursor(NULL, IDC_ARROW));
	nGfxServer::SetCursorShown(show);
}

//------------------------------------------------------------------------------
/**
	Activate, hide, or show the app window. 

	@param  none
*/
void 
nGlServer::update_window()
{
	if (this->hwnd)
	{
		long style = GetWindowLong(this->hwnd, GWL_STYLE);
		if (this->GetWindowMode(N_WINDOWMODE_ACTIVE))
			SetForegroundWindow(this->hwnd);

		if (this->GetWindowMode(N_WINDOWMODE_FRAMED))		
			style = (style | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);					
		else		
			style = (style & ~WS_CAPTION & ~WS_THICKFRAME);		

		SetWindowLongPtr(this->hwnd, GWL_STYLE, style);

		SetWindowPos(this->hwnd, (this->GetWindowMode(N_WINDOWMODE_TOPMOST) ? HWND_TOPMOST : HWND_NOTOPMOST), 
			0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | (this->GetWindowMode(N_WINDOWMODE_HIDDEN) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));
	}
}

#endif /* __WIN32__ */
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------                        
