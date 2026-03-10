import pefile

dll_path = r'T:/易语言源码/API创建窗口/emoji_window_cpp/emoji_window/Release/emoji_window.dll'
print(f"Analyzing: {dll_path}")

try:
    pe = pefile.PE(dll_path)

    if hasattr(pe, 'DIRECTORY_ENTRY_EXPORT'):
        print("\n=== Exported Functions ===")
        for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
            name = exp.name.decode() if exp.name else ""
            print(f"  {exp.ordinal}: {name}")
    else:
        print("No exports found!")
except Exception as e:
    print(f"Error: {e}")
