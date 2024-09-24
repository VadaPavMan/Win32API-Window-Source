#include <Windows.h>
#include <iostream>

bool running = true;

void* buffer_memory;
int buffer_width;
int buffer_height;
BITMAPINFO buffer_bitmap_info;

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	 case WM_CLOSE:
	 case WM_DESTROY:
	 {
		running = false;
	 }break;

	 case WM_SIZE: {
		 RECT rect;
		 GetClientRect(hwnd, &rect);
		 buffer_width = rect.right - rect.left;
		 buffer_height = rect.bottom - rect.top;

		 int buffer_size = buffer_width * buffer_height * sizeof(unsigned int);

		 if (buffer_memory) VirtualFree(buffer_memory, 0, MEM_RELEASE);
		 buffer_memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		 buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
		 buffer_bitmap_info.bmiHeader.biWidth = buffer_width;
		 buffer_bitmap_info.bmiHeader.biHeight = buffer_height;
		 buffer_bitmap_info.bmiHeader.biPlanes = 1;
		 buffer_bitmap_info.bmiHeader.biBitCount = 32;
		 buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

	 }break;
	
	 default: 
	 {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	 }
	}
	return result;
}

int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Create window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName="Window Class";
	window_class.lpfnWndProc = window_callback;

	// Resigter class 
	RegisterClassA(&window_class);

	// Create window
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		window_class.lpszClassName,                     // Window class
		"Window Project",    // Window text
		WS_OVERLAPPEDWINDOW | WS_VISIBLE ,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);
	HDC hdc = GetDC(hwnd);

	if (hwnd == NULL)
	{
		return 0;
	}

	while (running)
	{
		//Input
		MSG message;
		while (PeekMessage(&message, hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
		//Simulate
		unsigned int* pixel = (unsigned int*)buffer_memory;
		for (int y = 0; y < buffer_height; y++)
		{
			int red = 161 + (36 - 161) * y / buffer_height;     
			int green = 161 + (36 - 161) * y / buffer_height;   
			int blue = 161 + (36 - 161) * y / buffer_height;

			for (int x = 0; x < buffer_width; x++)
			{
				*pixel++ = RGB(red, green, blue);
			}
		}

		//Render
		StretchDIBits(hdc, 0, 0, buffer_width, buffer_height, 0, 0, buffer_width, buffer_height, buffer_memory, &buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}
	
}
