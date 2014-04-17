
#include "Header.h"

HWND hMainWnd;
extern HWND hDialog;
HINSTANCE hInst;

HBITMAP g_hbmHOMER = NULL;
HBITMAP g_hbmMASK = NULL;

BOOL makecenter = TRUE;
BOOL sb_preapplytransform = FALSE;
BOOL sb_bezier = TRUE;

POINT text_loc;

#define S_BLTMODE       701
#define S_DOTMODE       706
#define S_TEXTMODE      707


int draw_style = S_BLTMODE;

VOID redraw()
{
    RECT rc;
    GetClientRect(hMainWnd, (LPRECT)&rc);
    InvalidateRect(hMainWnd, &rc, TRUE);
    RedrawWindow(hMainWnd, &rc, 0, RDW_INVALIDATE);
}

#define MAXPOINTS 25
POINT pointarr[MAXPOINTS];
int ptrPoint = 0;


VOID draw(HDC hdc)
{
    SetGraphicsMode(hdc, GM_ADVANCED);

    //Draw homer
    switch (draw_style) {

    case S_BLTMODE:
    {
        HDC hdcMem = CreateCompatibleDC(hdc);
        BITMAP bm;
        SelectObject(hdcMem, g_hbmHOMER);
        GetObject(g_hbmHOMER, sizeof(bm), &bm);

        //Calculate target rect
        RECT rc;
        int height = bm.bmHeight;
        int width = bm.bmWidth;
        GetClientRect(hMainWnd, (LPRECT)&rc);
        if (makecenter) {
            int midx = (rc.right - rc.left) / 2;
            int midy = (rc.bottom - rc.top) / 2;
            rc.top = midy - (height / 2);
            rc.bottom = midy + (height / 2);
            rc.left = midx - (width / 2);
            rc.right = midx + (width / 2);
        }
        else {
            rc.top = rc.left = 10;
            rc.right = width + 10;
            rc.bottom = height + 10;
        }


        //Outline target rect BEFORE applying transform
        Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

        //Draw dot at the rotation center
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        Ellipse(hdc, 
                gv_rotate_X - rotation_centerdot_rad, 
                gv_rotate_Y - rotation_centerdot_rad,
                gv_rotate_X + rotation_centerdot_rad,
                gv_rotate_Y + rotation_centerdot_rad);

        ApplyTransforms(hdc);

        //Draw Homer
        SelectObject(hdcMem, g_hbmMASK);
        BitBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCAND);
        SelectObject(hdcMem, g_hbmHOMER);
        BitBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCPAINT);


        DeleteDC(hdcMem);
        break;
    }

    case S_DOTMODE:
    {
        //each dot in the array is drawn, then the bezier line.
        //If the transforms are applied before both,they should line up,
        //but if the dots are drawn before applying the transform they won't.

        if (sb_preapplytransform)
            ApplyTransforms(hdc);
        
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        for (int i = 0; i < ptrPoint; i++) {
            Ellipse(hdc,
                pointarr[i].x - rotation_centerdot_rad,
                pointarr[i].y - rotation_centerdot_rad,
                pointarr[i].x + rotation_centerdot_rad,
                pointarr[i].y + rotation_centerdot_rad);
            
        }

        if (!sb_preapplytransform)
            ApplyTransforms(hdc);

        if (sb_bezier)
            PolyBezier(hdc, pointarr, ptrPoint);
        else
            Polyline(hdc, pointarr, ptrPoint);

        break;
    }

    case S_TEXTMODE:
    {
        ApplyTransforms(hdc);
        ExtTextOut(hdc, text_loc.x, text_loc.y, 0, 0, L"Evan Rocks", 10, 0);
        break;
    }


    }
    
}


LRESULT CALLBACK WndProc(HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    hMainWnd = hwnd;

    switch (msg)
    {
    case WM_CREATE:
    {   
        BOOL status = TRUE;
        
        // Load Color Image
        g_hbmHOMER = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HOMER_COLOR));
        if (g_hbmHOMER == NULL) {
            ErrorMsg(L"Could not load color image!");
            status = FALSE;
        }

        // Load Image Mask
        g_hbmMASK = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HOMER_MASK));
        if (g_hbmMASK == NULL) {
            ErrorMsg(L"Could not load mask!");
            status = FALSE;
        }
        
        // Setup Global Init Values
        InitTransformGlobals();

        // Create Trasition Editor Dialog Box
        if (!CreateTransitionEditorDialogBox(hwnd)) {
            ErrorMsg(L"Could not create dialog box!");
            status = FALSE;
        }
        
        // If anything fails, close everything
        if (!status) {
            DestroyWindow(hwnd);
        }

        break;
    }

    case WM_PAINT:
        if (g_hbmHOMER != NULL) {
            PAINTSTRUCT ps;
            draw(BeginPaint(hMainWnd, &ps));
            EndPaint(hwnd, &ps);
        }
        break;

    case WM_LBUTTONDOWN:
        if (draw_style == S_BLTMODE) {

            //in S_BLTMODE mode, clicking sets the rotation center

            gv_rotate_X = LOWORD(lParam);
            gv_rotate_Y = HIWORD(lParam);
            redraw();
        }
        else if (draw_style == S_TEXTMODE) {

            text_loc.x = LOWORD(lParam);
            text_loc.y = HIWORD(lParam);
            redraw();
        
        }
        else if (draw_style == S_DOTMODE) {

            //in S_DOTMODE, clicking adds a point to pointarr

            if (ptrPoint >= MAXPOINTS) {
                ptrPoint = 0;
            }
            pointarr[ptrPoint].x = LOWORD(lParam);
            pointarr[ptrPoint].y = HIWORD(lParam);
            ptrPoint++;
            redraw();
       
        }
        break;

    case WM_KEYDOWN: 
        //for simplicity, using f keys to switch between each modes

        if (wParam == VK_F1) {
            
            //f1: normal blt mode
            
            draw_style = S_BLTMODE;
            makecenter = FALSE;
        }
        else if (wParam == VK_F2) {

            //f2: centered blt mode

            draw_style = S_BLTMODE;
            makecenter = TRUE;
        }
        else if (wParam == VK_F3) {

            //f3: preapply, line, dot mode

            draw_style = S_DOTMODE;
            sb_preapplytransform = TRUE;
            sb_bezier = FALSE;
        }
        else if (wParam == VK_F4) {

            //f4: postapply, line, dot mode

            draw_style = S_DOTMODE;
            sb_preapplytransform = FALSE;
            sb_bezier = FALSE;
        }
        else if (wParam == VK_F5) {

            //f5: preapply, bezier, dot mode

            draw_style = S_DOTMODE;
            sb_preapplytransform = TRUE;
            sb_bezier = TRUE;
        }
        else if (wParam == VK_F6) {

            //f6: postapply, bezier, dot mode

            draw_style = S_DOTMODE;
            sb_preapplytransform = FALSE;
            sb_bezier = TRUE;
        }
        else if (wParam == VK_F7) {

            //f7: text mode

            draw_style = S_TEXTMODE;
            redraw();
        }
        redraw();
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        DestroyWindow(hDialog);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    InitCommonControls();

    hInst = hInstance;

    //Register Window Class

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = g_wndClassName;
    wc.hIconSm = 0;
    if (!RegisterClassEx(&wc)) {
        ErrorMsg(L"Window Registration Failed!");
        return 0;
    }

    hMainWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,        //extended window style
        g_wndClassName,          //ties the new window to the window class we just created
        L"GDI transforms",       //(Initial) Window Title
        WS_OVERLAPPEDWINDOW,     //window style
        CW_USEDEFAULT,           //start x
        CW_USEDEFAULT,           //start y
        800,                     //start width 
        700,                     //start height
        NULL,                    //Parent Window Handle
        NULL,                    //Menu Handle
        hInstance,               //This Application's Instance Handle
        NULL);                   //Window Creation Data

    if (hMainWnd == NULL) {
        ErrorMsg(L"Window Creation Failed!");
        return 0;
    }


    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    //Ye Ol' Message Pump
    MSG m;
    while (GetMessage(&m, NULL, 0, 0) > 0) {
        if (!IsDialogMessage(hDialog, &m)) {
            TranslateMessage(&m);
            DispatchMessage(&m);
        }
    }

    return m.wParam;

}