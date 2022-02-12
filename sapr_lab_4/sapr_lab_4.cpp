#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <sstream>
#include <string.h>


const wchar_t windowClass[] = L"win32app";
const wchar_t windowTitle[] = L"Win32API - Пример окна";

int const grid = 20;
HPEN pen;
HPEN old_Pen;
HBRUSH brush;
HBRUSH old_brush;
HPEN zero_oppacity_pen;
int pen_size = 1;
int selected = 1;

std::wstring s2ws(std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

class Trace
{


private:
    float x;
    std::vector<float> y;

public:
    void set_x(float new_x) { x = new_x; }
    void set_y(float new_y) { y.push_back(new_y);}
    Trace()
    {
        x = 0;
        y.clear();
    }

    float get_x()           { return x; }
    float get_y(int n)      { return y[n]; }

};

class FileData
{
public:
    int number_of_plots = 0;
    std::vector<std::string> plot_names;
    std::vector<Trace> traces;

    void clear()
    {
        plot_names.clear();
        traces.clear();
        number_of_plots = 0;
    }

    void ReadFromFile(wchar_t* filename)
    {
        int n = 0;

        FILE* f = _wfopen(filename, L"r");
        if (!f) {
            
        }
        else
        {
            fscanf(f, "%d", &number_of_plots);

            for (int i = 0; i != number_of_plots; i++) {
                char name[32];
                fscanf(f, "%s", name);
                plot_names.push_back(name);
            }

            while (!feof(f)) {
                float data = 0;
                fwscanf(f, L"%f", &data);
                traces.push_back(Trace());
                traces[n].set_x(data);
                for(int i = 0; i != number_of_plots; i++)
                {
                    fwscanf(f, L"%f", &data);
                    traces[n].set_y(data);
                }
                n++;
            }
            n = 0;
            fclose(f);
        }
    }

    void Draw(HDC hdc, RECT r)
    {

        int color1 = 1, color2 = 1, color3 = 1;
        float k1 = 0; 
        float k2 = 0;
        float k3 = 0;
       
        float temptemp = 0;
        std::string test;
        LPCWSTR temp;              
        for (int i = 0; i < traces.size(); i++)
        {
            if (k1 < traces[i].get_x())
            {
                k1 = traces[i].get_x();
            }
        }
        std::wstringstream wss, wss1;
        test = k1 ;
        temp = s2ws(test).c_str();
        TextOut(hdc, r.right - 200, r.bottom - 60, temp, wcslen(temp));
        wss1 << traces[0].get_x();
        temp = wss1.str().c_str();
        TextOut(hdc, r.left + 20, r.bottom - 40, temp, wcslen(temp));        



        for (int i = 0; i < traces.size(); i++)
        {
            for (int j = 0; j != number_of_plots; j++)
            {
                if (k2 < traces[i].get_y(j))
                {
                    k2 = traces[i].get_y(j);
                }
            }
        }        

        for (int i = 0; i < traces.size(); i++)
        {
            for (int j = 0; j != number_of_plots; j++)
            {
                if (k3 > traces[i].get_y(j))
                {
                    k3 = traces[i].get_y(j);
                }
            }
        }     

        k2 = k2 - k3;

        for (int j = 0; j != number_of_plots; j++)
        {
            MoveToEx(hdc, 5, r.bottom - 35, NULL);

            if ((selected == j + 1))
            {
                if ((j + 3) % 2 == 0)
                    {
                        color1 = 0;
                        color2 = 0;
                        color3 = 255;
                    }
                else
                    {
                        color1 = 133;
                        color2 = 255;
                        color3 = 133;
                    }
            }
            else
            {
                color1 = 255;
                color2 = 0;
                color3 = 0;
            }

            pen = CreatePen(PS_SOLID, pen_size, RGB(color1, color2, color3));
            HPEN pen_for_legend = CreatePen(PS_SOLID, pen_size + 1, RGB(color1, color2, color3));
            old_Pen = (HPEN)SelectObject(hdc, pen);

            //std::wstring stemp = s2ws(plot_names[j]);
            //LPCWSTR result = stemp.c_str();
            
            std::wstring test = std::wstring(plot_names[j].begin(), plot_names[j].end());
            LPCWSTR sw = test.c_str();

            {
                int shift = plot_names[j].size() * 20 * j + 5;
                int shift_y = r.bottom - 20;
                TextOut(hdc, shift, shift_y, sw, wcslen(sw));
                SelectObject(hdc, old_Pen);
            }

            for (int i = 0; i != traces.size(); i++)
            {
                float x1;
                float y1;

                x1 = ((r.right-10) * traces[i].get_x()/ k1) + 5;
                y1 = (r.bottom - ((r.bottom-35) * (traces[i].get_y(j) - k3) / (k2))) - 30;
                
                if(i == 0)
                    MoveToEx(hdc, x1, y1, NULL);
                SelectObject(hdc, pen);

                if (i == traces.size() - 1)
                    MoveToEx(hdc, x1, y1, NULL);
                else
                    LineTo(hdc, x1, y1);

                if (((i % 20) == 0)&&(i >= 20)||(i == 0))
                {   
                    MoveToEx(hdc, x1 ,y1 ,(LPPOINT) NULL);
                    MoveToEx(hdc, x1 + 5 ,y1 ,(LPPOINT) NULL);
                    LineTo(hdc, (x1     +0),    (y1 - 5));
                    LineTo(hdc, (x1     -5),    (y1 - 0));
                    LineTo(hdc, (x1     +0),    (y1 + 5));
                    LineTo(hdc, (x1     +5),    (y1 + 0));
                    MoveToEx(hdc, x1, y1, (LPPOINT)NULL);
                }
                SelectObject(hdc, old_Pen);
            }
            MoveToEx(hdc, 5, r.bottom - 35, NULL);
        }
    };
};

void draw_grid(HWND hWnd, HDC hdc)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    float width_step = (rect.right - 10) / grid;
    float heigh_step = (rect.bottom - 35) / grid;

    brush = CreateSolidBrush(RGB(0, 0, 0));
    old_brush = (HBRUSH)SelectObject(hdc, brush);
    pen = CreatePen(PS_SOLID, pen_size, RGB(133, 133, 133));
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(hdc, old_brush);
    brush = CreateSolidBrush(RGB(150, 150, 150));
    old_brush = (HBRUSH)SelectObject(hdc, brush);
    pen = CreatePen(PS_SOLID, pen_size, RGB(133, 133, 133));
    Rectangle(hdc, rect.left + 5, rect.top + 5, rect.right - 5, rect.bottom - 30);
    
    for (int j = 5 + heigh_step; j < (rect.bottom - 30); j += heigh_step)
    {
        for (int i = 5 + width_step; i < (rect.right - 5); i += width_step)
        {
            SetPixel(hdc, i, j, RGB(100, 0, 0));
        }
    }
    old_Pen = (HPEN)SelectObject(hdc, pen);
    SelectObject(hdc, old_Pen);

}

bool isOpen = 0;

FileData data;

long __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    PAINTSTRUCT ps;
    HDC hdc;
    RECT r;
    int x_main = 0;
    int y_main = 0;
    char* name;
    GetClientRect(hWnd, &r);

    switch (message) {

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        draw_grid(hWnd, hdc);
        if(isOpen)
        {
            data.Draw(hdc, r);
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_LBUTTONDOWN:
        x_main = LOWORD(lParam);
        y_main = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, true);
        break;

    case WM_RBUTTONDOWN:
        x_main = LOWORD(lParam);
        y_main = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, true);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_DELETE) {
            OPENFILENAME ofn;
            wchar_t szFileName[MAX_PATH] = L"";
            data.clear();
            isOpen = false;
            ZeroMemory(&ofn, sizeof(ofn));

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = L"txt";

            if (GetOpenFileName(&ofn))
            {
                MessageBox(hWnd, ofn.lpstrFile, L"File name", MB_OK);
                isOpen = true;
                data.ReadFromFile(ofn.lpstrFile);
                InvalidateRect(hWnd, NULL, true);
            }
        }
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = windowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, L"Can’t register window class!", L"Win32 API Test", NULL);
        return 1;
    }

    HWND hWnd = CreateWindow(windowClass, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, L"Can’t create window!", L"Win32 API Test", NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}