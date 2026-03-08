# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A Windows DLL written in C++ using Direct2D/DirectWrite to provide UI controls with **color Emoji support** for 易语言 (EPL) applications. The DLL exports C-style APIs with `stdcall` calling convention, passing all text as UTF-8 byte arrays.

## Build Commands

### Visual Studio (Recommended)
1. Open `emoji_window.sln` in Visual Studio 2019+
2. Select `Release | x64` configuration
3. Build: `Ctrl+Shift+B`
4. Output: `x64\Release\emoji_window.dll`

### Command Line (MSBuild)
```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" emoji_window.sln /p:Configuration=Release /p:Platform=x64
```

## Architecture

### Core Components
- **emoji_window.cpp** - Main implementation with all control logic and rendering
- **emoji_window.h** - Public API declarations, state structures, callback types
- **emoji_window.def** - DLL export definitions
- **dllmain.cpp** - DLL entry point (initializes COM, D2D, DWrite factories)

### Technology Stack
- **Direct2D** - Hardware-accelerated 2D rendering
- **DirectWrite** - Text rendering with Emoji support
- **WIC (Windows Imaging Component)** - Image loading (PNG, JPEG, etc.)
- **C++17** with `/utf-8` compiler flag

### State Management Pattern
All controls use a state struct pattern with global maps:
```cpp
extern std::map<HWND, ControlState*> g_controls;  // HWND → State mapping
```

Controls include: `WindowState`, `EditBoxState`, `LabelState`, `CheckBoxState`, `ProgressBarState`, `PictureBoxState`, `RadioButtonState`, `ListBoxState`, `ComboBoxState`, `HotKeyState`, `GroupBoxState`, `TabControlState`, `MsgBoxState`

### Rendering Pattern
Each control type has a `Draw*()` function that uses D2D render target and DirectWrite:
```cpp
void DrawCheckBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, CheckBoxState* state);
```

## API Design Patterns

### Text Encoding
All text parameters use UTF-8 byte arrays with explicit length:
```cpp
__declspec(dllexport) HWND __stdcall CreateLabel(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes,  // UTF-8 bytes
    int text_len,                      // Byte length
    ...
);
```

Internal conversion: `Utf8ToWide()` converts to `std::wstring` for Windows APIs.

### Callbacks
All callbacks use `stdcall` convention:
```cpp
typedef void (__stdcall *CheckBoxCallback)(HWND hCheckBox, BOOL checked);
typedef void (__stdcall *ButtonClickCallback)(int button_id);
```

### Color Format
Colors use `UINT32` in ARGB format (`0xAARRGGBB`).

## Available Controls

| Control | Create Function | Key Features |
|---------|-----------------|--------------|
| Window | `create_window()` | D2D rendered, custom callbacks |
| Button | `create_emoji_button_bytes()` | Emoji support |
| Label | `CreateLabel()` | Word wrap, alignment |
| EditBox | `CreateEditBox()` / `CreateColorEmojiEditBox()` | RichEdit for emoji |
| CheckBox | `CreateCheckBox()` | Custom colors |
| ProgressBar | `CreateProgressBar()` | Animation, indeterminate mode |
| PictureBox | `CreatePictureBox()` | File/memory loading, scale modes |
| RadioButton | `CreateRadioButton()` | Group-based mutual exclusion |
| ListBox | `CreateListBox()` | Multi-select, custom rendering |
| ComboBox | `CreateComboBox()` / `CreateD2DComboBox()` | Dropdown with emoji support |
| HotKey | `CreateHotKeyControl()` | Hotkey capture |
| GroupBox | `CreateGroupBox()` | Container with title |
| TabControl | `CreateTabControl()` | Multi-tab container |

## 易语言 Integration

### Text Handling
易语言 IDE doesn't support Unicode emoji display - all emoji/Unicode text must be passed as byte arrays:
```易语言
' Emoji as UTF-8 bytes
字节集 变量 = { 240, 159, 152, 128 }  ' 😀 emoji
```

### Conditional Syntax (Critical)
```
.如果真 (条件)      ' Single branch - NO else
    ' code
.如果真结束

.如果 (条件)        ' Double branch - WITH else
    ' code
.否则
    ' code
.如果结束
```

### Key Rules
- Arrays are 1-indexed (not 0)
- Constants must be prefixed with `#` when used: `#CONSTANT_NAME`
- GDI objects must be freed with `DeleteObject()`
- Use `传址` keyword for pass-by-reference parameters

## Adding New Controls

1. Define state struct in `emoji_window.h`
2. Add global map: `extern std::map<HWND, NewControlState*> g_newcontrols;`
3. Create export function with UTF-8 bytes pattern
4. Implement `DrawNewControl()` rendering function
5. Add to `emoji_window.def` exports
6. Handle WM_PAINT and input messages in window proc
