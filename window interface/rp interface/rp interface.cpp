// rp interface.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "rp interface.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hInputField;
HWND hPrompt;
HWND hButton;



// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RPINTERFACE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RPINTERFACE));

    MSG msg;

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RPINTERFACE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RPINTERFACE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
    //RECT rect = { 5, 5, 100, 100 };
    //TCHAR greeting1[] = _T("русский текст: русский текст");
    //TCHAR greeting2[] = _T("переменная:");
    grid_identifier GB_grid = { 12, 8, 50 };
    RECT r_temp;
    //int var1 = 1337;
    //std::wstring a = std::to_wstring(var1);

    
    switch (message)
    {
    case WM_CREATE:
        BootData(hWnd);
        create_input_box(hWnd, &::hButton, &::hInputField, &::hPrompt, 325, 500);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case 2:
                MakeTurn(hWnd, get_input_field_value(::hInputField));
                update_info(hWnd);
                break;
            case IDM_ABOUT:
                DialogBox(::hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            
            HBRUSH hWallBrush = CreateSolidBrush(RGB(128, 128, 128));
            //HBRUSH hCharBrush = CreateSolidBrush(RGB(255, 0, 0));
            //HBITMAP image = (HBITMAP)LoadImage(NULL, L"g:\\sans.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            //if (image == NULL) MessageBox(0, L"Couldn't load the image", L"Error", MB_OK | MB_ICONERROR);
            //else hCharBrush = CreatePatternBrush(image);
            
            for (int x = 0; x < x_max; x++) {
                for (int y = 0; y < y_max; y++) {
                    int this_sq_id_temp = ::GameBoard[y][x];
                    if (this_sq_id_temp == 1) {
                        r_temp = get_square_from_grid(GB_grid, x + 1, y + 1);
                        FillRect(hdc, &r_temp, hWallBrush);
                    }
                    else if (this_sq_id_temp != 0 && BattleMembers[this_sq_id_temp - 2].get_hp() > 0) {
                        r_temp = get_square_from_grid(GB_grid, x + 1, y + 1);
                        FillRect(hdc, &r_temp, ::character_hBrushes[this_sq_id_temp - 2]);
                    }
                }
            }
            draw_line_grid(hdc, GB_grid);
            every_print_character_data(hdc, 700, 50);
            PrintTurnOrder(hdc, 700, 500);
            /*
            TextOut(hdc,
                5, 5,
                greeting1, _tcslen(greeting1));

            TextOut(hdc,
                5, 20,
                greeting2, _tcslen(greeting2));

            TextOut(hdc,
                110, 20,
                a.c_str(), a.length());
            */


            //SelectObject(hdc, hBrush);
            //DeleteObject(hCharBrush);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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
