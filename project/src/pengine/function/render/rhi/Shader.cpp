#include "Shader.h"
#include "core/utils/StringUtils.h"
namespace pengine
{
	namespace
	{
		auto getShaderTypeByName(const std::string& name) -> ShaderType
		{
			if (StringUtils::contains(name, "Vertex"))
			{
				return ShaderType::Vertex;
			}

			if (StringUtils::contains(name, "Fragment"))
			{
				return ShaderType::Fragment;
			}

			if (StringUtils::contains(name, "Geometry"))
			{
				return ShaderType::Geometry;
			}
			if (StringUtils::contains(name, "Compute"))
			{
				return ShaderType::Compute;
			}
			return ShaderType::Unknown;
		}
	}        // namespace
	auto Shader::parseSource(const std::vector<std::string>& lines, std::unordered_map<ShaderType, std::string>& shaders) -> void
	{
		for (uint32_t i = 0; i < lines.size(); i++)
		{
			std::string str = lines[i];
			if (StringUtils::startWith(str, "#"))
			{
				auto path = StringUtils::split(str, " ");
				auto type = getShaderTypeByName(path[0]);
				if (type != ShaderType::Unknown)
				{
					StringUtils::trim(path[1], "\r");
					shaders[type] = path[1];
					PLOGI("{0} : {1}", path[0], path[1]);
				}
			}
		}
	}
};