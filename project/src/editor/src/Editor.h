#include "Application.h"
#include <memory>
#include <unordered_map>

#include "EditorWindow.h"
namespace pengine
{
	class Editor final : public Application
	{
	public:
		Editor();
		void init() override;
		void onImGui() override;
		void onUpdate(const float &delta) override;
		void onRenderDebug() override;
		template <class T>
		void addSubWindow();
		template <class T>
		inline auto getEditorWindow()
		{
			return std::static_pointer_cast<T>(editorWindows[typeid(T).hash_code()]);
		}
	private:
		std::unordered_map<size_t, std::shared_ptr<EditorWindow>> editorWindows;

		
	};
	template <class T>
	auto Editor::addSubWindow() -> void
	{
		editorWindows.emplace(typeid(T).hash_code(), std::make_shared<T>());
	}

}