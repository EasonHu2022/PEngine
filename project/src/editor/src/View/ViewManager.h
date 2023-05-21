#pragma once
#include <memory>
#include <unordered_map>
#include "editorView.h"
namespace peditor
{
	class ViewManager
	{
	public:
		ViewManager();
		~ViewManager();
		void init();
		void onImGui();
		template<class T>
		inline void addView(char* _name, PEngineGUI::PEngineGUIWindowFlags _flags = 0)
		{
			editorViews.emplace(typeid(T).hash_code(), std::make_shared<T>(_name,_flags));
			editorViews[typeid(T).hash_code()]->onAdd();
		}
		template<class T>
		inline void removeView()
		{
			auto it = editorViews.find(typeid(T).hash_code());
			if (it != editorViews.end()) {
				editorViews.erase(it);
			}
			else
			{
				PLOGE("try to remove a unexist view, type name : {0}", typeid(T).name());
			}
		}
		template <class T>
		inline auto getView()
		{
			return std::static_pointer_cast<T>(editorViews[typeid(T).hash_code()]);
		}

	private:
		std::unordered_map<size_t, std::shared_ptr<EditorView>>  editorViews;

	};


};





