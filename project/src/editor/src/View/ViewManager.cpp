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

		addView<SceneView>("test test test",PEngineGUIWindowFlags_::PEngineGUIWindowFlags_Modal);

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
};