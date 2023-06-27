#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace pengine
{
	enum class FileType : int32_t
	{
		Normal,
		Folder,
		Texture,
		Model,
		FBX,
		OBJ,
		Text,
		Script,
		Dll,
		Scene,
		RenderGraph,
		MP3,
		OGG,
		AAC,
		WAV,
		TTF,
		C_SHARP,
		Shader,
		Material,
		//for gizmo
		Lighting,
		Camera,
		Length
	};

	static std::unordered_map<std::string, FileType> ExtensionsToType = {
		{"mp3", FileType::MP3},
		{"ogg", FileType::OGG},
		{"wav", FileType::WAV},
		{"aac", FileType::AAC},
		{"jpg", FileType::Texture},
		{"png", FileType::Texture},
		{"tga", FileType::Texture},
		{"lua", FileType::Script},
		{"cs", FileType::C_SHARP},
		{"rg", FileType::RenderGraph},
		{"glsl", FileType::Text},
		{"shader", FileType::Text},
		{"vert", FileType::Text},
		{"frag", FileType::Text},
		{"text", FileType::Text},
		{"scene", FileType::Scene},
		{"obj", FileType::Model},
		{"fbx", FileType::Model},
		{"glb", FileType::Model},
		{"gltf", FileType::Model},
		{"dll", FileType::Dll},
		{"material", FileType::Material},
		{"so", FileType::Dll} };

	class IResource
	{
	public:
		virtual auto getResourceType() const->FileType = 0;
		virtual auto getPath() const->std::string = 0;

	};
};      
