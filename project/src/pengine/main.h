#pragma once
#include "Application.h"

extern pengine::Application* createApplication();//iml in editor
int32_t main()
{
	pengine::Application::app	= createApplication();
	
	auto retCode				= pengine::Application::app->start();

	delete pengine::Application::app;
	return retCode;

}