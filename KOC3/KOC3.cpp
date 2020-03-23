#include <stdafx.h> 
#include <windows.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <commctrl.h> 
#include "resource.h"

HWND hWndDialog; HINSTANCE ghInstance; // Переменная для хранения хендела процесса 
// Описание используемой оконной процедуры 
BOOL CALLBACK PviewDlgProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam); 
// Главное приложение программы 
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	MSG msg; 
	ghInstance = hInstance; 
	// Создание диалогового окна 
	hWndDialog = CreateDialogParam (hInstance, MAKEINTRESOURCE (IDD_DIALOG1), NULL, (DLGPROC) PviewDlgProc, (LONG)0);

	ShowWindow(hWndDialog, nCmdShow); 
	UpdateWindow(hWndDialog);

	// Стандартный цикл обработки сообщений приложения
	while (GetMessage (&msg, NULL, 0, 0)) 
		if (!IsDialogMessage (hWndDialog, &msg)) { 
		TranslateMessage (&msg); 
		DispatchMessage (&msg); 
		} 
	DestroyWindow (hWndDialog); 
	return 0; 
}

//Процедура обработки сообщений диалогового окна 
BOOL CALLBACK PviewDlgProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {

	static HWND hList1, hSpin, hStatic; static RECT rect; // координаты прямоугольника 
	static HDC hdc, hdcm; // контекст устройтва 
	static PAINTSTRUCT ps; // структура для перерисовки окна 
	static HBITMAP hbm; // дескриптор изображения 
	static HBRUSH hbr; // дескриптор кисти 
	static HPEN whitePen, blackPen; // дескриптор пера 
	static int lx, ly; // координаты конца экрана 
	static int spin; // интервал между линиями

	switch (wMsg) { // сообщение об инициализации диалоговой формы 
	case WM_INITDIALOG: // установка параметров спинера 
		InitCommonControls(); // подключить общие элементы управления 
		hSpin = GetDlgItem (hWnd, IDC_SPIN1); // получить дескриптор спиннера 
		hList1 = GetDlgItem (hWnd, IDC_EDIT1); // получить дескриптор окна редактирования 
		SendMessage(hSpin, UDM_SETBUDDY, (WPARAM)hList1, 0);//связать спиннер с окном редактирования 
		SendMessage(hSpin, UDM_SETRANGE, 0, MAKELONG(100,5)); // установить диапазон спиннера 
		SendMessage(hSpin,UDM_SETPOS,0,20); // начальное значение спиннера

		hStatic = GetDlgItem(hWnd, IDC_STATIC); 
		hdc = GetDC(hStatic); // получаем контекст устройства окна hWnd 
		GetClientRect(hStatic, &rect); // получить координаты графического окна 
		lx = rect.right; 
		ly = rect.bottom; 
		// создать в памяти контекст устройства hdcm, 
		// совместимый с текущим контекстом устройства вывода hdc. 
		hdcm = CreateCompatibleDC(hdc); // Создать холст в памяти заданного размера 
		hbm = CreateCompatibleBitmap(hdc, lx,ly); 
		hbr = (HBRUSH)GetStockObject(WHITE_BRUSH); // белый цвет кисти 
		blackPen = (HPEN)GetStockObject(BLACK_PEN); // черный цвет пера 
		whitePen = (HPEN)GetStockObject(WHITE_PEN); // белый цвет пера 
		// привязать к контексту устройства: 
		SelectObject(hdcm, hbm); // холст 
		SelectObject(hdcm, hbr); // кисть 
		SelectObject(hdcm, blackPen); // перо 
		// закрашивает прямоугольную область с координатами(x, y), шириной width // и высотой height, используя текущую кисть и растровую операцию dwRaster 
		// PATCOPY - область заполняется текущей кистью; 
		PatBlt(hdcm,0,0, lx, ly, PATCOPY); 
		InvalidateRect(hWnd, NULL, false); 
		break;

	case WM_CLOSE: PostQuitMessage(0); 
		break;

	case WM_PAINT: 
		hdc = BeginPaint(hStatic, &ps); // получить контекст устройства 
		BitBlt(hdc, 0, 0, lx, ly, hdcm, 0, 0, SRCCOPY); // Вывод битового изображения 
		EndPaint(hStatic, &ps); // очистить контекст устройства
		break;

	case WM_COMMAND: 
		switch (LOWORD(wParam)) {
	case IDC_BUTTON1: 
		spin = SendMessage(hSpin, UDM_GETPOS, 0, 0); // значение спиннера 
		for (int i = 0; i <= ly;){ 
			LineTo(hdcm, lx, i); // нарисовать линию 
			i += spin; MoveToEx(hdcm, 0, i, NULL); // переместить текущие координаты 
		}
		MoveToEx(hdcm, 0, 0, NULL); 
		InvalidateRect(hWnd, NULL, false); 
		break;

	case IDC_BUTTON2: 
		spin = SendMessage(hSpin, UDM_GETPOS, 0, 0); 
		for (int i = 0; i <= lx;) { 
			LineTo(hdcm, i, ly); 
			i += spin; 
			MoveToEx(hdcm, i, 0, NULL); 
		} 
		MoveToEx(hdcm, 0, 0, NULL); 
		InvalidateRect(hWnd, NULL, false); 
		break; 

	case IDC_BUTTON3: 
		SelectObject(hdcm, whitePen); // смена пера на белый 
		Rectangle(hdcm, 0, 0, lx, ly); // закрасить графическое окно белым 
		SelectObject(hdcm, blackPen); // смена пера на черный 
		InvalidateRect(hWnd, NULL, false); 
		break;

	default: 
		return FALSE;
		} 
	break; 
	default: 
		return FALSE;
	} 
	return TRUE;
}