#include <windows.h>
#include <stdio.h>

// 导入GetWindowTitle函数
typedef int (__stdcall *GetWindowTitleFunc)(HWND hwnd, unsigned char* buffer, int buffer_size);

int main() {
    // 加载DLL
    HMODULE hDll = LoadLibraryA("emoji_window.dll");
    if (!hDll) {
        printf("无法加载emoji_window.dll\n");
        return 1;
    }

    // 获取函数指针
    GetWindowTitleFunc GetWindowTitle = (GetWindowTitleFunc)GetProcAddress(hDll, "GetWindowTitle");
    if (!GetWindowTitle) {
        printf("无法找到GetWindowTitle函数\n");
        FreeLibrary(hDll);
        return 1;
    }

    // 测试1: 获取记事本窗口标题
    HWND hwnd = FindWindowA("Notepad", NULL);
    if (hwnd) {
        printf("找到记事本窗口: 0x%p\n", hwnd);
        
        // 第一次调用：获取所需长度
        int len = GetWindowTitle(hwnd, nullptr, 0);
        printf("标题长度: %d 字节\n", len);
        
        if (len > 0) {
            // 第二次调用：获取实际内容
            unsigned char* buffer = new unsigned char[len + 1];
            int result = GetWindowTitle(hwnd, buffer, len);
            buffer[len] = 0;  // 添加null终止符
            
            printf("标题内容: %s\n", buffer);
            printf("返回长度: %d\n", result);
            
            delete[] buffer;
        }
    } else {
        printf("未找到记事本窗口，请先打开记事本\n");
    }

    // 测试2: 测试无效窗口句柄
    printf("\n测试无效窗口句柄:\n");
    int result = GetWindowTitle(NULL, nullptr, 0);
    printf("返回值: %d (应该是-1)\n", result);

    // 测试3: 获取桌面窗口标题
    HWND desktop = GetDesktopWindow();
    printf("\n测试桌面窗口:\n");
    int desktop_len = GetWindowTitle(desktop, nullptr, 0);
    printf("桌面窗口标题长度: %d\n", desktop_len);

    FreeLibrary(hDll);
    printf("\n测试完成!\n");
    return 0;
}
