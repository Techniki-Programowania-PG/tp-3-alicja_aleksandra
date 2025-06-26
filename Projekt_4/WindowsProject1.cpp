#include "framework.h"
#include "resource.h"
#include <objidl.h>
#include <gdiplus.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <memory>
#include <commctrl.h>

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib, "comctl32.lib")

using namespace Gdiplus;

#define MAX_LOADSTRING 100

const int T1_X = 200, T1_Y = 400;
const int T2_X = 620, T2_Y = 450;
const int W = 300, H = 30;
const int cx = 555, cy = 440;
const int SORT_W = 30, SORT_H = 100;
const double KAT = (3.14159 / 2) + (3.14159 / 6);
const double alpha = (3.14159 / 6);

int globalSpeed = 5;
bool przepuscKolo = true;
bool przepuscKwadrat = true;
bool przepuscTrojkat = true;

enum TypFigury { KOLO, KWADRAT, TROJKAT };
enum TrybFiltru {
    FILTR_KOLO_I_KWADRAT_ODRZUC_KOLO,
    FILTR_KOLO_I_KWADRAT_ODRZUC_KWADRAT,
    FILTR_KWADRAT_I_TROJKAT_ODRZUC_KWADRAT,
    FILTR_KWADRAT_I_TROJKAT_ODRZUC_TROJKAT,
    FILTR_WYLACZONY
};
TrybFiltru trybFiltru = FILTR_WYLACZONY;

struct Element {
    int x, y, r;
    int segment;
    double theta;
    int speed;
    double x_start, y_start, end_x;
    TypFigury typ;
    bool aktywny;
    bool widoczny;

    Element(int startX, int startY, int radius, double angle, int spd, double x_s, double y_s, double e_x, TypFigury t)
        : x(startX), y(startY), r(radius), theta(angle), speed(spd),
        x_start(x_s), y_start(y_s), end_x(e_x), segment(0), typ(t), aktywny(true), widoczny(true){}

    void aktualizuj() {
        switch (segment) {
        case 0:
            x += speed;
            y = T1_Y - r;
            if (x >= T1_X + W) {
                x = T1_X + W;
                segment = 1;
            }
            break;

        case 1:
            x += (int)(speed * cos(alpha));
            y += (int)(speed * sin(alpha));
            if (x >= cx + 44) {
                x = cy + 160;
                segment = 2;
            }
            break;

        case 2:
            x += speed;
            y = T2_Y - r;
            if (x >= 920) {
                x = -r;  // Reset pozycji
                y = T1_Y - r;
                segment = 0;
                aktywny = true; 
                // Przywróć figurę na taśmę po pełnym obiegu
            }
            break;
        }
        
    }
    
    void rysuj(HDC hdc) {
        if (!aktywny) return;
		if (!widoczny) return;
        switch (typ) {
        case KOLO:
            Ellipse(hdc, x - r, y - r, x + r, y + r);
            break;
        case KWADRAT:
            Rectangle(hdc, x - r, y - r, x + r, y + r);
            break;
        case TROJKAT: {
            POINT pts[3] = {
                { x, y - r },
                { x - r, y + r },
                { x + r, y + r }
            };
            Polygon(hdc, pts, 3);
            break;
        }
        }
    }
};

std::vector<std::unique_ptr<Element>> figury;
std::vector<std::unique_ptr<Element>> figuryBackup;

bool CzyZderzenieZFiltrem(const Element& f) {
    const RECT filtr = { T2_X, T2_Y, T2_X + 30, T2_Y + 30 };
    return f.x + f.r >= filtr.left && f.x - f.r <= filtr.right &&
        f.y + f.r >= filtr.top && f.y - f.r <= filtr.bottom;
}

void GetRotatedRect(POINT pts[4], int cx, int cy, int w, int h, double theta) {
    double cosT = cos(theta), sinT = sin(theta);
    int dx[4] = { -w / 2, w / 2, w / 2, -w / 2 }, dy[4] = { -h / 2, -h / 2, h / 2, h / 2 };
    for (int i = 0; i < 4; ++i) {
        pts[i].x = cx + (dx[i] * cosT - dy[i] * sinT);
        pts[i].y = cy + (dx[i] * sinT + dy[i] * cosT);
    }
}

void RysujFiltrKwadrat(HDC hdc) {
    Rectangle(hdc, T2_X, T2_Y, T2_X + 30, T2_Y + 30);
}

void PrzywrocFigury() {
    figury.clear();
    for (const auto& f : figuryBackup) {
        figury.emplace_back(std::make_unique<Element>(*f));
    }
    przepuscKolo = true;
    przepuscKwadrat = true;
    przepuscTrojkat = true;
}

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
    InitCommonControlsEx(&icc);

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wcscpy_s(szTitle, L"Symulacja Taśmy Produkcyjnej");
    wcscpy_s(szWindowClass, L"MainWindowClass");

    if (!MyRegisterClass(hInstance)) return 0;
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"MainWindowClass";
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WINDOWSPROJECT1);
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hWnd = CreateWindowW(L"MainWindowClass", szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        SetTimer(hWnd, 1, 30, NULL);
        double x_s = cx - (SORT_W / 2.0) * cos(KAT);
        double y_s = cy - (SORT_H / 2.0) * sin(KAT);
        double e_x = cx + (SORT_W / 2.0) * cos(KAT);

        figury.emplace_back(std::make_unique<Element>(T1_X, T1_Y - 20, 20, KAT, globalSpeed, x_s, y_s, e_x, KOLO));
        figury.emplace_back(std::make_unique<Element>(T1_X - 150, T1_Y - 20, 20, KAT, globalSpeed, x_s, y_s, e_x, KWADRAT));
        figury.emplace_back(std::make_unique<Element>(T1_X - 300, T1_Y - 20, 20, KAT, globalSpeed, x_s, y_s, e_x, TROJKAT));

        for (const auto& f : figury) {

            figuryBackup.emplace_back(std::make_unique<Element>(*f));
            std::vector<std::unique_ptr<Element>> noweFigury;
        }
        break;
    }
    case WM_TIMER: {
        std::vector<std::unique_ptr<Element>> noweFigury;

        for (auto& f : figury) {
            if (!f->aktywny) continue;

            f->aktualizuj();

            bool widoczny = true;
            bool odrzucony = false;

            if (CzyZderzenieZFiltrem(*f)) {
                switch (trybFiltru) {
                case FILTR_KOLO_I_KWADRAT_ODRZUC_KOLO:
                    if (f->typ == KOLO) {
                        odrzucony = true;
                    }
                    else if (f->typ == TROJKAT) {
                        widoczny = false;
                    }
                    break;

                case FILTR_KOLO_I_KWADRAT_ODRZUC_KWADRAT:
                    if (f->typ == KWADRAT) {
                        odrzucony = true;
                    }
                    else if (f->typ == TROJKAT) {
                        widoczny = false;
                    }
                    break;

                case FILTR_KWADRAT_I_TROJKAT_ODRZUC_KWADRAT:
                    if (f->typ == KWADRAT) {
                        odrzucony = true;
                    }
                    else if (f->typ == KOLO) {
                        widoczny = false;
                    }
                    break;

                case FILTR_KWADRAT_I_TROJKAT_ODRZUC_TROJKAT:
                    if (f->typ == TROJKAT) {
                        odrzucony = true;
                    }
                    else if (f->typ == KOLO) {
                        widoczny = false;
                    }
                    break;

                case FILTR_WYLACZONY:
                    break;
                }

                if (odrzucony) {
                    f->aktywny = false;

                    noweFigury.emplace_back(std::make_unique<Element>(
                        -20, T1_Y - 20, 20, KAT, globalSpeed,
                        f->x_start, f->y_start, f->end_x, f->typ
                    ));
                }
                else {
                    // status widoczności do elementu
                    f->widoczny = widoczny;
                }
            }
        }

        // nowe figury
        for (auto& nowa : noweFigury) {
            figury.emplace_back(std::move(nowa));
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILTER_1:
            trybFiltru = FILTR_KOLO_I_KWADRAT_ODRZUC_KOLO;
            break;

        case ID_FILTER_2:
            trybFiltru = FILTR_KOLO_I_KWADRAT_ODRZUC_KWADRAT;
            break;

        case ID_FILTER_3:
            trybFiltru = FILTR_KWADRAT_I_TROJKAT_ODRZUC_KWADRAT;
            break;

        case ID_FILTER_4:
            trybFiltru = FILTR_KWADRAT_I_TROJKAT_ODRZUC_TROJKAT;
            break;

        case ID_FILTER_OFF:
            trybFiltru = FILTR_WYLACZONY;
            PrzywrocFigury();
            break;
        }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Rectangle(hdc, T1_X, T1_Y, T1_X + W, T1_Y + H);
        Rectangle(hdc, T2_X, T2_Y, T2_X + W, T2_Y + H);
        POINT pts[4];
        GetRotatedRect(pts, cx, cy, SORT_W, SORT_H, KAT);
        Polygon(hdc, pts, 4);
        RysujFiltrKwadrat(hdc);
        for (auto& f : figury) f->rysuj(hdc);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY: {
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    }
    default:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }
    return 0;
    
}
    

