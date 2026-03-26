"""检查非DPI-aware模式下回调参数是否被放大"""
import ctypes
from ctypes import CFUNCTYPE, c_int
import sys, os, time, threading

sys.path.insert(0, os.path.dirname(__file__))


def main() -> None:
    # 尝试设置非DPI-aware
    try:
        r = ctypes.windll.user32.SetProcessDpiAwarenessContext(c_int(-1))
        print(f"SetProcessDpiAwarenessContext(-1) 返回: {r}")
    except Exception as e:
        print(f"设置失败: {e}")

    # 检查当前DPI
    hdc = ctypes.windll.user32.GetDC(0)
    dpi = ctypes.windll.gdi32.GetDeviceCaps(hdc, 88)
    ctypes.windll.user32.ReleaseDC(0, hdc)
    print(f"当前系统DPI: {dpi} (缩放比例: {dpi/96*100:.0f}%)")

    dll = ctypes.CDLL("./emoji_window.dll")
    dll.create_window_bytes_ex.restype = c_int
    dll.create_window_bytes_ex.argtypes = [ctypes.c_void_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int]
    dll.set_message_loop_main_window.argtypes = [c_int]
    dll.run_message_loop.restype = c_int
    dll.SetWindowResizeCallback.argtypes = [ctypes.c_void_p]

    RESIZE_CB = CFUNCTYPE(None, c_int, c_int, c_int)
    hwnd = 0

    def on_resize(wnd, w, h):
        if wnd != hwnd:
            return
        print(f"  回调: w={w}, h={h} (创建时: 800x600)")

    resize_cb = RESIZE_CB(on_resize)

    def do_resize():
        time.sleep(1)
        ctypes.windll.user32.MoveWindow(hwnd, 100, 100, 1000, 700, True)
        time.sleep(0.5)
        ctypes.windll.user32.PostMessageA(hwnd, 0x0010, 0, 0)

    tb = ctypes.create_string_buffer("test".encode("utf-8"))
    hwnd = dll.create_window_bytes_ex(tb, 4, -1, -1, 800, 600, 0, 0)
    print(f"窗口创建: 800x600, hwnd={hwnd}")

    dll.SetWindowResizeCallback(resize_cb)
    dll.set_message_loop_main_window(hwnd)

    t = threading.Thread(target=do_resize, daemon=True)
    t.start()
    dll.run_message_loop()


if __name__ == "__main__":
    main()
