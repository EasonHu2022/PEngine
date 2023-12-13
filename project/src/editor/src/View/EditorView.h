#pragma once
#include "imgui.h"
#include "function/engine/Timestep.h"
#include "imgui.h"
using namespace pengine;
namespace peditor
{
	/// <summary>
	/// it's editor view which mean in a same layer in the editor, don;t mix with editorWindow, which has dif layers decided by user.
	/// each view will just have 1 instance in the editor.
	/// </summary>
	class EditorView
	{
	public:
		EditorView(char* _name, ImGuiWindowFlags _flags = 0)
			: name(_name), flags(_flags) {};
		virtual ~EditorView() = default;
		virtual void init() = 0;
		virtual void onAdd() = 0;
		virtual void onRemove() = 0;
		virtual void onUpdate(const Timestep& dt) = 0;
		virtual auto release() -> void = 0;

		/// <summary>
		/// override when you want to set other GUICond or pivot or something
		/// </summary>
		virtual void drawSizeAndPos()
		{
			//set size and pos
			//ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(size);
		};
		virtual void drawView() final 
		{
			drawSizeAndPos();
			ImGui::Begin(name, &p_open, flags);
			onImGui();
			ImGui::End();
		}
		virtual char* getName() { return name; };
		virtual void setName(char* newName) { name = newName; };
		virtual bool isOpen() { return p_open; }
		virtual void show() { p_open = true; }
		virtual void hide() { p_open = false; }

	private:
		virtual void onImGui() = 0;
	protected:
		char* name;
		bool p_open = true;
		//default value
		//if you don't override updateSizeAndPos() the view will be drawed in these params
		glm::vec2 pos = glm::vec2(0,0);
		glm::vec2 size = glm::vec2(10,10);
		ImGuiWindowFlags flags = 0;
	};


}