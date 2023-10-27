#pragma once
#include "Application.h"

extern pengine::Application* createApplication(std::string executablePath);//impl in editor
int32_t main(int argc, char* argv[])
{
	std::string exePath = argv[0];
	pengine::Application::app	= createApplication(exePath);
	
	auto retCode				= pengine::Application::app->start();

	delete pengine::Application::app;
	return retCode;

}