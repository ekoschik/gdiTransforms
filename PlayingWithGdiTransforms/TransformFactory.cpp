
#include "Header.h"


#define T_SCALE         601
#define T_TRANSLATE     602
#define T_ROTATE        603
#define T_SHEAR         604
//#define T_REFLECT       605

HWND hDialog;

HWND hTrackScaleX;
HWND hScaleValX;
HWND hTrackScaleY;
HWND hScaleValY;

HWND hTrackTranslateX;
HWND hTranslateValX;
HWND hTrackTranslateY;
HWND hTranslateValY;

HWND hTrackShear;
HWND hShearVal;

HWND hTrackRotate;
HWND hRotateVal;


VOID TransformFactory(HDC hdc, 
                      int tranform, 
                      FLOAT arg1 = 0,
                      FLOAT arg2 = 0,
                      FLOAT arg3 = 0)
{
    XFORM xForm;

    switch (tranform) {

    case T_SCALE:
        //arg1: X scaleint component
        //arg2: Y scaleint component
        xForm.eM11 = (FLOAT) arg1;
        xForm.eM12 = (FLOAT) 0.0;
        xForm.eM21 = (FLOAT) 0.0;
        xForm.eM22 = (FLOAT) arg2;
        xForm.eDx =  (FLOAT) 0.0;
        xForm.eDy =  (FLOAT) 0.0;
        break;

    case T_TRANSLATE:
        //arg1: X translate factor
        //arg2: Y translate factor
        xForm.eM11 = (FLOAT) 1.0;
        xForm.eM12 = (FLOAT) 0.0;
        xForm.eM21 = (FLOAT) 0.0;
        xForm.eM22 = (FLOAT) 1.0;
        xForm.eDx =  (FLOAT) arg1;
        xForm.eDy =  (FLOAT) arg2;
        break;

    case T_SHEAR:
        //arg1: shear factor
        xForm.eM11 = (FLOAT) 1.0;
        xForm.eM12 = (FLOAT) arg1;
        xForm.eM21 = (FLOAT) 0.0;
        xForm.eM22 = (FLOAT) 1.0;
        xForm.eDx =  (FLOAT) 0.0;
        xForm.eDy =  (FLOAT) 0.0;
        break;

    case T_ROTATE:
        //arg1: degrees
        //arg2: rotation point X
        //arg3: rotation point Y
        xForm.eM11 = (FLOAT) cos(arg1 * PI / 180.0);
        xForm.eM12 = (FLOAT) sin(arg1 * PI / 180.0);
        xForm.eM21 = (FLOAT) -sin(arg1 * PI / 180.0);
        xForm.eM22 = (FLOAT) cos(arg1 * PI / 180.0);
        xForm.eDx =  (FLOAT) arg2 - cos(arg1) * arg2 + sin(arg1) * arg3;
        xForm.eDy =  (FLOAT) arg3 - cos(arg1) * arg3 - sin(arg1) * arg2;
        break;

    //case T_REFLECT:
    //    //arg1: horizontal reflection component
    //    //arg2: vertical reflection component
    //    xForm.eM11 = (FLOAT) arg1;
    //    xForm.eM12 = (FLOAT) 0.0;
    //    xForm.eM21 = (FLOAT) 0.0;
    //    xForm.eM22 = (FLOAT) arg2;
    //    xForm.eDx =  (FLOAT) 0.0;
    //    xForm.eDy =  (FLOAT) 0.0;
    //    break;

    default:
        ErrorMsg(L"Invalid Tranform Code");
        return;
    }

    //TODO: does it make sense to also use MWT_RIGHTMULTIPLY?
    ModifyWorldTransform(hdc, &xForm, MWT_LEFTMULTIPLY);

}

/*
 * Normal Scaling factor is 1, should be limited to (0.0,~5.0)
*/
const FLOAT gv_scale_start = 1.0;
const FLOAT gv_scale_min = 0.0;
const FLOAT gv_scale_max = 5.0;
FLOAT gv_scale_X;
FLOAT gv_scale_Y;
VOID Scale(HDC hdc) {
    TransformFactory(hdc, T_SCALE, gv_scale_X, gv_scale_Y);
}


/*
* Normale Translate factor is 0, should be limited to (-400,600)
*/
const int gv_translate_start = 0;
const int gv_translate_min = -400;
const int gv_translate_max = 600;
int gv_translate_H;
int gv_translate_V;
VOID Translate(HDC hdc) {
    TransformFactory(hdc, T_TRANSLATE, (FLOAT)gv_translate_H, (FLOAT)gv_translate_V);
}

/*
* Normal Shear factor is 0, should be limited to (-3,3)
*/
const FLOAT gv_shear_start = 0.0;
const FLOAT gv_shear_min = -3.0;
const FLOAT gv_shear_max = 3.0;
FLOAT gv_shear;
VOID Shear(HDC hdc) {
    TransformFactory(hdc, T_SHEAR, gv_shear);
}


/*
* Normal Rotation factor is 0, should be limited to (0,360)
*/
const int gv_rotate_start = 0.0;
const int gv_rotate_min = 0.0;
const int gv_rotate_max = 360.0;
const int rotation_centerdot_rad = 4;
int gv_rotate;
int gv_rotate_X;
int gv_rotate_Y;
VOID Rotate(HDC hdc) {
    TransformFactory(hdc, T_ROTATE, gv_rotate, (FLOAT)gv_rotate_X, (FLOAT)gv_rotate_Y);
}


///*
//* Normal Reflect factor is 0, 
//*/
//const int gv_reflect_start = 0.0;
//const int gv_reflect_min = 0.0; //todo: set these
//const int gv_reflect_max = 360.0;
//int gv_reflect_V;
//int gv_reflect_H;
//VOID Reflect(HDC hdc) {
//    TransformFactory(hdc, T_REFLECT, (FLOAT)gv_reflect_H, (FLOAT)gv_reflect_V);
//}



VOID ApplyTransforms(HDC hdc)
{
    Rotate(hdc);
    Shear(hdc);
    Scale(hdc);
    Translate(hdc);
    //Reflect(hdc);
}

VOID UpdateLabels()
{
    //Scale X
    {
        LRESULT pos = SendMessageW(hTrackScaleX, TBM_GETPOS, 0, 0);
        FLOAT val = pos / 10.0 + .05;
        wchar_t buf[30];
        swprintf(buf, 30, L"%.1f", val);
        SetWindowTextW(hScaleValX, buf);
        gv_scale_X = val;
    }
    //Scale Y
    {
        LRESULT pos = SendMessageW(hTrackScaleY, TBM_GETPOS, 0, 0);
        FLOAT val = pos / 10.0 + .05;
        wchar_t buf[30];
        swprintf(buf, 30, L"%.1f", val);
        SetWindowTextW(hScaleValY, buf);
        gv_scale_Y = val;
    }
    //Translate X
    {
        LRESULT pos = SendMessageW(hTrackTranslateX, TBM_GETPOS, 0, 0);
        wchar_t buf[30];
        swprintf(buf, 30, L"%d", pos);
        SetWindowTextW(hTranslateValX, buf);
        gv_translate_H = pos;
    }
    //Translate Y
    {
        LRESULT pos = SendMessageW(hTrackTranslateY, TBM_GETPOS, 0, 0);
        wchar_t buf[30];
        swprintf(buf, 30, L"%d", pos);
        SetWindowTextW(hTranslateValY, buf);
        gv_translate_V = pos;
    }
    //Shear
    {
        LRESULT pos = SendMessageW(hTrackShear, TBM_GETPOS, 0, 0);
        FLOAT val = pos / 10.0;
        wchar_t buf[30];
        swprintf(buf, 30, L"%.1f", val);
        SetWindowTextW(hShearVal, buf);
        gv_shear = val;
    }

    //Rotate
    {
        LRESULT pos = SendMessageW(hTrackRotate, TBM_GETPOS, 0, 0);
        wchar_t buf[30];
        swprintf(buf, 30, L"%d", pos);
        SetWindowTextW(hRotateVal, buf);
        gv_rotate = pos;
    }
    redraw();
}

VOID InitTransformGlobals()
{
    gv_scale_X = gv_scale_start;
    gv_scale_Y = gv_scale_start;
    gv_translate_H = gv_translate_start;
    gv_translate_V = gv_translate_start;
    gv_shear = gv_shear_start;
    gv_rotate = gv_rotate_start;
    //gv_reflect_V = gv_reflect_start;
    //gv_reflect_H = gv_reflect_start;

    //by default put rotation center in the middle of the client area
    RECT rc;
    GetClientRect(hMainWnd, (LPRECT)&rc);
    gv_rotate_X = (rc.right - rc.left) / 2;
    gv_rotate_Y = (rc.bottom - rc.top) / 2;

    SendMessage(hTrackScaleX,
        TBM_SETPOS,
        (WPARAM)TRUE,
        (LPARAM)gv_scale_start * 10);

    SendMessage(hTrackScaleY,
        TBM_SETPOS,
        (WPARAM)TRUE,
        (LPARAM)gv_scale_start * 10);

    SendMessage(hTrackTranslateX,
        TBM_SETPOS,
        (WPARAM)TRUE,
        (LPARAM)gv_translate_start);

    SendMessage(hTrackTranslateY,
        TBM_SETPOS,
        (WPARAM)TRUE,
        (LPARAM)gv_translate_start);

    SendMessage(hTrackShear,
        TBM_SETPOS,
        (WPARAM)TRUE,
        (LPARAM)gv_shear_start * 10);

    SendMessage(hTrackRotate,
        TBM_SETPOS,
        (WPARAM)TRUE,
        (LPARAM)gv_rotate_start);

    UpdateLabels();


}


INT_PTR CALLBACK TransitionWndProc(HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{

    switch (msg)
    {
    case WM_HSCROLL:
        UpdateLabels();
        break;


    case WM_COMMAND:
        switch (LOWORD(wParam)){
        case ID_DIALOG_RESET:
            InitTransformGlobals();
            redraw();
            break;
        }
        break;
    

    }

    return 0;
}





BOOL CreateTransitionEditorDialogBox(HWND hwnd)
{
    hDialog = 
        CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, TransitionWndProc);
    if (!hDialog) return FALSE;


    int topbuff = 13;
    int slider_height = 40;
    int labelwidth = 80;
    int labelheight = 30;
    int labelrightbuf = 10;
    int trackbar_height = 30;
    int trackbar_width = 200;
    int curValWidth = 100;
    int curValHeight = 30;


    
    //scale x
    hTrackScaleX = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        L"",
        WS_CHILD |
        WS_VISIBLE,
        labelwidth + labelrightbuf, 
        topbuff,
        trackbar_width, 
        trackbar_height,
        hDialog,
        0,
        hInst,
        NULL
        );
    SendMessage(hTrackScaleX, 
                TBM_SETRANGE,
                (WPARAM)TRUE, 
                (LPARAM)MAKELONG(gv_scale_min * 10, gv_scale_max * 10));
    hScaleValX = CreateWindowW(L"STATIC",
                              L"0", 
                              WS_CHILD | WS_VISIBLE,
                              trackbar_width + labelwidth + topbuff,
                              topbuff,
                              curValWidth,
                              curValHeight,
                              hDialog, 
                              (HMENU)3, NULL, NULL);
    HWND hScaleLabelX = CreateWindowW(L"STATIC", 
                              L"0", 
                              WS_CHILD | WS_VISIBLE,
                              topbuff,
                              topbuff,
                              labelwidth, 
                              labelheight,
                              hDialog, 
                              (HMENU)3, NULL, NULL);
    SetWindowTextW(hScaleLabelX, L"Scale X");
    
    //scale y
    hTrackScaleY = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        L"",
        WS_CHILD |
        WS_VISIBLE,
        labelwidth + labelrightbuf, 
        slider_height + topbuff,
        trackbar_width, 
        trackbar_height,
        hDialog,
        0,
        hInst,
        NULL
        );
    SendMessage(hTrackScaleY,
        TBM_SETRANGE,
        (WPARAM)TRUE,
        (LPARAM)MAKELONG(gv_scale_min * 10, gv_scale_max * 10));
    hScaleValY = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        trackbar_width + labelwidth + topbuff, 
        slider_height + topbuff,
        curValWidth,
        curValHeight,
        hDialog,
        (HMENU)3, NULL, NULL);
    HWND hScaleLabelY = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        topbuff, 
        slider_height + topbuff,
        labelwidth, 
        labelheight,
        hDialog,
        (HMENU)3, NULL, NULL);
    SetWindowTextW(hScaleLabelY, L"Scale Y");

    //translate x
    hTrackTranslateX = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        L"",
        WS_CHILD |
        WS_VISIBLE,
        labelwidth + labelrightbuf,
        slider_height * 2 + topbuff,
        trackbar_width,
        trackbar_height,
        hDialog,
        0,
        hInst,
        NULL
        );

    SendMessage(hTrackTranslateX,
        TBM_SETRANGE,
        (WPARAM)TRUE,
        (LPARAM)MAKELONG(gv_translate_min, gv_translate_max));
    hTranslateValX = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        trackbar_width + labelwidth + topbuff,
        slider_height * 2 + topbuff,
        curValWidth,
        curValHeight,
        hDialog,
        (HMENU)3, NULL, NULL);
    HWND hTranslateLabelX = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        topbuff,
        slider_height * 2 + topbuff,
        labelwidth,
        labelheight,
        hDialog,
        (HMENU)3, NULL, NULL);
    SetWindowTextW(hTranslateLabelX, L"Translate X");

    //translate y
    hTrackTranslateY = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        L"",
        WS_CHILD |
        WS_VISIBLE,
        labelwidth + labelrightbuf,
        slider_height * 3 + topbuff,
        trackbar_width,
        trackbar_height,
        hDialog,
        0,
        hInst,
        NULL
        );

    SendMessage(hTrackTranslateY,
        TBM_SETRANGE,
        (WPARAM)TRUE,
        (LPARAM)MAKELONG(gv_translate_min, gv_translate_max));
    hTranslateValY = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        trackbar_width + labelwidth + topbuff,
        slider_height * 3 + topbuff,
        curValWidth,
        curValHeight,
        hDialog,
        (HMENU)3, NULL, NULL);
    HWND hTranslateLabelY = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        topbuff,
        slider_height * 3 + topbuff,
        labelwidth,
        labelheight,
        hDialog,
        (HMENU)3, NULL, NULL);
    SetWindowTextW(hTranslateLabelY, L"Translate Y");

    
    //shear
    hTrackShear = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        L"",
        WS_CHILD |
        WS_VISIBLE,
        labelwidth + labelrightbuf,
        slider_height * 4 + topbuff,
        trackbar_width,
        trackbar_height,
        hDialog,
        0,
        hInst,
        NULL
        );

    SendMessage(hTrackShear,
        TBM_SETRANGE,
        (WPARAM)TRUE,
        (LPARAM)MAKELONG(gv_shear_min * 10, gv_shear_max * 10));
    hShearVal = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        trackbar_width + labelwidth + topbuff,
        slider_height * 4 + topbuff,
        curValWidth,
        curValHeight,
        hDialog,
        (HMENU)3, NULL, NULL);
    HWND hShearLabel = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        topbuff,
        slider_height * 4 + topbuff,
        labelwidth,
        labelheight,
        hDialog,
        (HMENU)3, NULL, NULL);
    SetWindowTextW(hShearLabel, L"Shear");

    //rotate
    hTrackRotate = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        L"",
        WS_CHILD |
        WS_VISIBLE,
        labelwidth + labelrightbuf,
        slider_height * 5 + topbuff,
        trackbar_width,
        trackbar_height,
        hDialog,
        0,
        hInst,
        NULL
        );

    SendMessage(hTrackRotate,
        TBM_SETRANGE,
        (WPARAM)TRUE,
        (LPARAM)MAKELONG(gv_rotate_min, gv_rotate_max));
    hRotateVal = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        trackbar_width + labelwidth + topbuff,
        slider_height * 5 + topbuff,
        curValWidth,
        curValHeight,
        hDialog,
        (HMENU)3, NULL, NULL);
    HWND hRotateLabel = CreateWindowW(L"STATIC",
        L"0",
        WS_CHILD | WS_VISIBLE,
        topbuff,
        slider_height * 5 + topbuff,
        labelwidth,
        labelheight,
        hDialog,
        (HMENU)3, NULL, NULL);
    SetWindowTextW(hRotateLabel, L"Rotate");




    //All sliders have been created, update all values

    InitTransformGlobals();


    //Show and place dialog box and return success

    ShowWindow(hDialog, SW_SHOW);

    RECT rc;
    GetWindowRect(hwnd, &rc);
    MoveWindow(hDialog, rc.left, rc.bottom, 400, 400, TRUE);

    return TRUE;
}

