#include "viewManager.h"
#include "SceneView.h"
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

		addView<SceneView>("Scene",PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoResize |
			PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoScrollWithMouse | PEngineGUIWindowFlags_ ::PEngineGUIWindowFlags_NoScrollbar|PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoMove | PEngineGUIWindowFlags_::PEngineGUIWindowFlags_NoCollapse);

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
	void ViewManager::onUpdate(const Timestep& delta)
	{
		for (auto& view : editorViews)
		{		
			view.second->onUpdate(delta);		
		}
	}
};