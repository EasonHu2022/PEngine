#include "PEngineGUI.h"
#include "imgui.h"
namespace pengine
{
	auto PEngineGUI::beginWindow(const char* name, bool* p_open, PEngineGUIWindowFlags flags) -> bool
	{
		return ImGui::Begin(name,p_open,flags);
	}
	auto PEngineGUI::endWindow() -> void
	{
		ImGui::End();
	}
	auto PEngineGUI::separator() -> void
	{
		ImGui::Separator();
	}
	auto PEngineGUI::sameLine(float offset_from_start_x, float spacing) -> void
	{
		ImGui::SameLine(offset_from_start_x, spacing);
	}
	auto PEngineGUI::newLine() -> void
	{
		ImGui::NewLine();
	}
	auto PEngineGUI::spacing() -> void
	{
		ImGui::Spacing();
	}
	auto PEngineGUI::button(const char* label, const glm::vec2& size) -> bool
	{
		return ImGui::Button(label, size);
	}
	auto PEngineGUI::checkbox(const char* label, bool* v) -> bool
	{
		return ImGui::Checkbox(label, v);
	}
	auto PEngineGUI::radioButton(const char* label, bool active) -> bool
	{
		return ImGui::RadioButton(label, active);
	}
	auto PEngineGUI::sliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, PEngineGUISliderFlags flags) -> bool
	{
		return ImGui::SliderFloat(label, v,  v_min,  v_max, format , flags);
	}
	auto PEngineGUI::inputText(const char* label, char* buf, size_t buf_size, PEngineGUIInputTextFlags flags, PEngineGUIInputTextCallback callback, void* user_data) -> bool
	{
		if (callback != NULL)
		{
			PLOGE("PEngine doesn't support callback yet!");
			return false;
		}
		return ImGui::InputText(label,  buf, buf_size, flags ,  NULL ,  user_data);
	}
	auto PEngineGUI::inputInt(const char* label, int* v, int step, int step_fast, PEngineGUIInputTextFlags flags) -> bool
	{
		return ImGui::InputInt( label, v, step ,  step_fast,  flags );
	}
	auto PEngineGUI::inputFloat(const char* label, float* v, float step, float step_fast, const char* format, PEngineGUIInputTextFlags flags) -> bool
	{
		return ImGui::InputFloat(label, v,  step , step_fast ,  format,  flags );
	}
	auto PEngineGUI::text(const char* fmt, ...) -> void
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}
	auto PEngineGUI::textColored(const glm::vec4& col, const char* fmt, ...) -> void
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextColoredV(col, fmt, args);
		va_end(args);
	}
	auto PEngineGUI::image(PEngineGUITextureID user_texture_id, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& tint_col, const glm::vec4& border_col) -> void
	{
		ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
	}
	
	auto PEngineGUI::openPopup(const char* str_id) -> void
	{
		ImGui::OpenPopup(str_id);
	}
	auto PEngineGUI::beginPopupModal(const char* name, bool* p_open, PEngineGUIWindowFlags flags) -> bool
	{
		return ImGui::BeginPopupModal(name,  p_open , flags );
	}
	auto PEngineGUI::endPopup() -> void
	{
		ImGui::EndPopup();
	}
	auto PEngineGUI::isItemHovered(PEngineGUIHoveredFlags flags) -> bool
	{
		return ImGui::IsItemHovered(flags);
	}
	auto PEngineGUI::getMousePos() -> glm::vec2
	{
		return ImGui::GetMousePos();
	}
};