#pragma once
#include "core/core.h"
namespace pengine
{
	enum PENGINE_API PEngineGUIWindowFlags_
	{
        PEngineGUIWindowFlags_None = 0,
        PEngineGUIWindowFlags_NoTitleBar = 1 << 0,   // Disable title-bar
        PEngineGUIWindowFlags_NoResize = 1 << 1,   // Disable user resizing with the lower-right grip
        PEngineGUIWindowFlags_NoMove = 1 << 2,   // Disable user moving the window
        PEngineGUIWindowFlags_NoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        PEngineGUIWindowFlags_NoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        PEngineGUIWindowFlags_NoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        PEngineGUIWindowFlags_AlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
        PEngineGUIWindowFlags_NoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        PEngineGUIWindowFlags_NoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
        PEngineGUIWindowFlags_NoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
        PEngineGUIWindowFlags_MenuBar = 1 << 10,  // Has a menu-bar
        PEngineGUIWindowFlags_HorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        PEngineGUIWindowFlags_NoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        PEngineGUIWindowFlags_NoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        PEngineGUIWindowFlags_AlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        PEngineGUIWindowFlags_AlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        PEngineGUIWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
        PEngineGUIWindowFlags_NoNavInputs = 1 << 18,  // No gamepad/keyboard navigation within the window
        PEngineGUIWindowFlags_NoNavFocus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
        PEngineGUIWindowFlags_UnsavedDocument = 1 << 20,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        PEngineGUIWindowFlags_NoNav = PEngineGUIWindowFlags_NoNavInputs | PEngineGUIWindowFlags_NoNavFocus,
        PEngineGUIWindowFlags_NoDecoration = PEngineGUIWindowFlags_NoTitleBar | PEngineGUIWindowFlags_NoResize | PEngineGUIWindowFlags_NoScrollbar | PEngineGUIWindowFlags_NoCollapse,
        PEngineGUIWindowFlags_NoInputs = PEngineGUIWindowFlags_NoMouseInputs | PEngineGUIWindowFlags_NoNavInputs | PEngineGUIWindowFlags_NoNavFocus,

        // [Internal]
        PEngineGUIWindowFlags_NavFlattened = 1 << 23,  // [BETA] On child window: allow gamepad/keyboard navigation to cross over parent border to this child or between sibling child windows.
        PEngineGUIWindowFlags_ChildWindow = 1 << 24,  // Don't use! For internal use by BeginChild()
        PEngineGUIWindowFlags_Tooltip = 1 << 25,  // Don't use! For internal use by BeginTooltip()
        PEngineGUIWindowFlags_Popup = 1 << 26,  // Don't use! For internal use by BeginPopup()
        PEngineGUIWindowFlags_Modal = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
        PEngineGUIWindowFlags_ChildMenu = 1 << 28,  // Don't use! For internal use by BeginMenu()
	};

    enum PENGINE_API PEngineGUISliderFlags_
    {
        PEngineGUISliderFlags_None = 0,
        PEngineGUISliderFlags_AlwaysClamp = 1 << 4,       // Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
        PEngineGUISliderFlags_Logarithmic = 1 << 5,       // Make the widget logarithmic (linear otherwise). Consider using ImGuiSliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
        PEngineGUISliderFlags_NoRoundToFormat = 1 << 6,       // Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits)
        PEngineGUISliderFlags_NoInput = 1 << 7,       // Disable CTRL+Click or Enter key allowing to input text directly into the widget
        PEngineGUISliderFlags_InvalidMask_ = 0x7000000F,   // [Internal] We treat using those bits as being potentially a 'float power' argument from the previous API that has got miscast to this enum, and will trigger an assert if needed.

    };

    enum PENGINE_API PEngineGUIInputTextFlags_
    {
        PEngineGUIInputTextFlags_None = 0,
        PEngineGUIInputTextFlags_CharsDecimal = 1 << 0,   // Allow 0123456789.+-*/
        PEngineGUIInputTextFlags_CharsHexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
        PEngineGUIInputTextFlags_CharsUppercase = 1 << 2,   // Turn a..z into A..Z
        PEngineGUIInputTextFlags_CharsNoBlank = 1 << 3,   // Filter out spaces, tabs
        PEngineGUIInputTextFlags_AutoSelectAll = 1 << 4,   // Select entire text when first taking mouse focus
        PEngineGUIInputTextFlags_EnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
        PEngineGUIInputTextFlags_CallbackCompletion = 1 << 6,   // Callback on pressing TAB (for completion handling)
        PEngineGUIInputTextFlags_CallbackHistory = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
        PEngineGUIInputTextFlags_CallbackAlways = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
        PEngineGUIInputTextFlags_CallbackCharFilter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
        PEngineGUIInputTextFlags_AllowTabInput = 1 << 10,  // Pressing TAB input a '\t' character into the text field
        PEngineGUIInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
        PEngineGUIInputTextFlags_NoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
        PEngineGUIInputTextFlags_AlwaysOverwrite = 1 << 13,  // Overwrite mode
        PEngineGUIInputTextFlags_ReadOnly = 1 << 14,  // Read-only mode
        PEngineGUIInputTextFlags_Password = 1 << 15,  // Password mode, display all characters as '*'
        PEngineGUIInputTextFlags_NoUndoRedo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
        PEngineGUIInputTextFlags_CharsScientific = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
        PEngineGUIInputTextFlags_CallbackResize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
        PEngineGUIInputTextFlags_CallbackEdit = 1 << 19,  // Callback on any edit (note that InputText() already returns true on edit, the callback is useful mainly to manipulate the underlying buffer while focus is active)
        PEngineGUIInputTextFlags_EscapeClearsAll = 1 << 20,  // Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)
    };

    enum PENGINE_API PEngineGUIHoveredFlags_
    {
        PEngineGUIHoveredFlags_None = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
        PEngineGUIHoveredFlags_ChildWindows = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
        PEngineGUIHoveredFlags_RootWindow = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
        PEngineGUIHoveredFlags_AnyWindow = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
        PEngineGUIHoveredFlags_NoPopupHierarchy = 1 << 3,   // IsWindowHovered() only: Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
        //ImGuiHoveredFlags_DockHierarchy               = 1 << 4,   // IsWindowHovered() only: Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
        PEngineGUIHoveredFlags_AllowWhenBlockedByPopup = 1 << 5,   // Return true even if a popup window is normally blocking access to this item/window
        //ImGuiHoveredFlags_AllowWhenBlockedByModal     = 1 << 6,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
        PEngineGUIHoveredFlags_AllowWhenBlockedByActiveItem = 1 << 7,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
        PEngineGUIHoveredFlags_AllowWhenOverlapped = 1 << 8,   // IsItemHovered() only: Return true even if the position is obstructed or overlapped by another window
        PEngineGUIHoveredFlags_AllowWhenDisabled = 1 << 9,   // IsItemHovered() only: Return true even if the item is disabled
        PEngineGUIHoveredFlags_NoNavOverride = 1 << 10,  // Disable using gamepad/keyboard navigation state when active, always query mouse.
        PEngineGUIHoveredFlags_RectOnly = PEngineGUIHoveredFlags_AllowWhenBlockedByPopup | PEngineGUIHoveredFlags_AllowWhenBlockedByActiveItem | PEngineGUIHoveredFlags_AllowWhenOverlapped,
        PEngineGUIHoveredFlags_RootAndChildWindows = PEngineGUIHoveredFlags_RootWindow | PEngineGUIHoveredFlags_ChildWindows,

        // Hovering delays (for tooltips)
        PEngineGUIHoveredFlags_DelayNormal = 1 << 11,  // Return true after io.HoverDelayNormal elapsed (~0.30 sec)
        PEngineGUIHoveredFlags_DelayShort = 1 << 12,  // Return true after io.HoverDelayShort elapsed (~0.10 sec)
        PEngineGUIHoveredFlags_NoSharedDelay = 1 << 13,  // Disable shared delay system where moving from one item to the next keeps the previous timer for a short time (standard for tooltips with long delays)
    };

    enum PENGINE_API PEngineGUICond_
    {
        PEngineGUICond_None = 0,        // No condition (always set the variable), same as _Always
        PEngineGUICond_Always = 1 << 0,   // No condition (always set the variable), same as _None
        PEngineGUICond_Once = 1 << 1,   // Set the variable once per runtime session (only the first call will succeed)
        PEngineGUICond_FirstUseEver = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
        PEngineGUICond_Appearing = 1 << 3,   // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
    };

    // Flags for ImGui::IsWindowFocused()
    enum PEngineGuiFocusedFlags_
    {
        PEngineGuiFocusedFlags_None = 0,
        PEngineGuiFocusedFlags_ChildWindows = 1 << 0,   // Return true if any children of the window is focused
        PEngineGuiFocusedFlags_RootWindow = 1 << 1,   // Test from root window (top most parent of the current hierarchy)
        PEngineGuiFocusedFlags_AnyWindow = 1 << 2,   // Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
        PEngineGuiFocusedFlags_NoPopupHierarchy = 1 << 3,   // Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
        //ImGuiFocusedFlags_DockHierarchy               = 1 << 4,   // Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
        PEngineGuiFocusedFlags_RootAndChildWindows = PEngineGuiFocusedFlags_RootWindow | PEngineGuiFocusedFlags_ChildWindows,
    };
};