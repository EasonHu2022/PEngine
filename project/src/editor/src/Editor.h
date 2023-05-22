#include "Application.h"
#include <memory>
#include <unordered_map>

using namespace pengine;
namespace peditor
{
	class ViewManager;
	class Editor final : public Application
	{
	public:
		Editor();
		void init() override;
		void onImGui() override;
		void onUpdate(const float &delta) override;
		void onRenderDebug() override;
	private:
		std::unique_ptr<ViewManager> viewManager;

		
	};
}