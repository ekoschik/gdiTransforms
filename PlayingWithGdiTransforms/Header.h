
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "resource.h"
#include <math.h>

#define PI 3.14159265

const LPCWSTR g_wndClassName = L"myWindowClass";

extern HWND hMainWnd;
extern HINSTANCE hInst;

extern HBITMAP g_hbmHOMER;
extern HBITMAP g_hbmMASK;

//#define IDD_DIALOG1 40401

__inline VOID ErrorMsg(LPCWSTR msg) {
    MessageBox(hMainWnd, msg, L"D'oh!", MB_OK | MB_ICONEXCLAMATION);
}

VOID redraw();



/*
* Transform Factory
*/

VOID InitTransformGlobals();
VOID ApplyTransforms(HDC hdc);
BOOL CreateTransitionEditorDialogBox(HWND hwnd);
INT_PTR CALLBACK TransitionWndProc(HWND hwnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam);

extern const FLOAT gv_scale_start;
extern const FLOAT gv_scale_min;
extern const FLOAT gv_scale_max;
extern FLOAT gv_scale_X;
extern FLOAT gv_scale_Y;

extern const int gv_translate_start;
extern const int gv_translate_min;
extern const int gv_translate_max;
extern int gv_translate_H;
extern int gv_translate_V;

extern const FLOAT gv_shear_start;
extern const FLOAT gv_shear_min;
extern const FLOAT gv_shear_max;
extern FLOAT gv_shear;

extern const int gv_rotate_start;
extern const int gv_rotate_min;
extern const int gv_rotate_max;
extern int gv_rotate;
extern int gv_rotate_X;
extern int gv_rotate_Y;
extern const int rotation_centerdot_rad;

//extern const int gv_reflect_start;
//extern const int gv_reflect_min;
//extern const int gv_reflect_max;
//extern int gv_reflect_V;
//extern int gv_reflect_H;