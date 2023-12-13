#include "viewManager.h"
#include "SceneView.h"
#include "InspectorView.h"
namespace peditor
{
	ViewManager::ViewManager()
	{
	}
	ViewManager::~ViewManager()
	{
	}
	void ViewManager::init()
	{

		addView<SceneView>("Scene");
		addView<InspectorView>("Inspector");
		for (auto it = editorViews.begin(); it != editorViews.end(); ++it) {
			it->second->init();
		}
	}
	void ViewManager::onImGui()
	{
		for (auto it = editorViews.begin(); it != editorViews.end(); ++it) {
			it->second->drawView();
		}
	}
	auto ViewManager::release() -> void
	{
		for (auto it = editorViews.begin(); it != editorViews.end(); ++it) {
			it->second->release();
		}
		editorViews.clear();
	}
	void ViewManager::onUpdate(const Timestep& delta)
	{
		for (auto& view : editorViews)
		{		
			view.second->onUpdate(delta);		
		}
	}
};