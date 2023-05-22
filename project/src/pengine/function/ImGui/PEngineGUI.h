#pragma once
#include "core/core.h"
#include "PEngineGUIDefs.h"
#include "glm/glm.hpp"
namespace pengine
{

	class PENGINE_API PEngineGUI
	{
	
	public:
		typedef int  PEngineGUIWindowFlags;
		typedef int  PEngineGUISliderFlags;
		typedef int  PEngineGUIInputTextFlags;
		typedef int  PEngineGUIInputTextCallback;
		typedef int  PEngineGUIHoveredFlags;
		typedef int  PEngineGUICond;

		typedef unsigned long long  PEngineGUITextureID;//may cause bug when not using vulkan cause may be other backend does not support u64

		public:
			/// <summary>
			/// begin window flag
			/// </summary>
			/// <param name="name">: window name </param>
			/// <param name="p_open">: if open</param>
			/// <param name="flags">: flags </param>
			/// <returns></returns>
			static auto beginWindow(const char* name, bool* p_open = NULL, PEngineGUIWindowFlags flags = 0) -> bool;
			static auto endWindow() -> void ;
			/// <summary>
			/// add a separator
			/// </summary>
			/// <returns></returns>
			static auto separator() -> void;
			/// <summary>
			/// let next element be rendered at the same line
			/// </summary>
			/// <param name="offset_from_start_x">same line offset</param>
			/// <param name="spacing">same line spacing</param>
			/// <returns></returns>
			static auto sameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f) -> void;
			static auto newLine() -> void;
			static auto spacing() -> void;
			static auto button(const char* label, const glm::vec2& size)-> bool;
			static auto checkbox(const char* label, bool* v)-> bool;
			static auto radioButton(const char* label, bool active)-> bool;
			static auto sliderFloat(const char* label, float* v, float v_min, float v_max, 
				const char* format = "%.3f", PEngineGUISliderFlags flags = 0)-> bool;
			static auto inputText(const char* label, char* buf, size_t buf_size, 
				PEngineGUIInputTextFlags flags = 0, PEngineGUIInputTextCallback callback = NULL, void* user_data = NULL)-> bool;
			static auto inputInt(const char* label, int* v, int step = 1, 
				int step_fast = 100, PEngineGUIInputTextFlags flags = 0)-> bool;
			static auto inputFloat(const char* label, float* v, float step = 0.1f,
				float step_fast = 1.0f, const char* format = "%.3f", PEngineGUIInputTextFlags flags = 0)-> bool;
			static auto text(const char* fmt, ...)-> void;
			static auto textColored(const glm::vec4 & col, const char* fmt, ...)-> void;
			static auto image(PEngineGUITextureID user_texture_id, const glm::vec2& size, const glm::vec2& uv0 = glm::vec2(0,0),
				const glm::vec2& uv1 = glm::vec2(1,1), const glm::vec4& tint_col = glm::vec4(1,1,1,1), 
				const glm::vec4& border_col = glm::vec4(0,0,0,0))-> void;
			static auto openPopup(const char* str_id)-> void;
			static auto beginPopupModal(const char* name, bool* p_open = NULL, PEngineGUIWindowFlags flags = 0)-> bool;
			static auto endPopup()-> void;
			static auto isItemHovered(PEngineGUIHoveredFlags flags = 0) -> bool;
			static auto getMousePos() -> glm::vec2;
			static auto setNextWindowPos(glm::vec2& pos, PEngineGUICond cond = 0,glm::vec2& pivot = glm::vec2(0,0)) -> void;
			static auto setNextWindowSize(glm::vec2& size, PEngineGUICond cond = 0) -> void;
	};

	
};