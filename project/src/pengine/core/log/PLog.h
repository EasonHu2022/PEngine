#pragma once
#include <iostream>
#include <string>
#include <stdarg.h>
#include "spdlog/spdlog.h"
#include "core/core.h"
namespace pengine
{
	class PENGINE_API PLogger
	{
	public:
		static auto init() -> void;
		static auto& getLogger() { return logger; }
	private:
		static std::shared_ptr<spdlog::logger> logger;
	};
};
#define PLOGV(...)      pengine::PLogger::getLogger()->trace(__VA_ARGS__)
#define PLOGI(...)      pengine::PLogger::getLogger()->info(__VA_ARGS__)
#define PLOGW(...)      pengine::PLogger::getLogger()->warn(__VA_ARGS__)
#define PLOGE(...)      pengine::PLogger::getLogger()->error(__VA_ARGS__)
#define PLOGC(...)      pengine::PLogger::getLogger()->critical(__VA_ARGS__)