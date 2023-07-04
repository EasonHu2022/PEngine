#pragma once
namespace pengine
{
	class Scene;
	class ISystem
	{
	public:
		ISystem() = default;
		virtual ~ISystem() = default;
		virtual auto onInit() -> void = 0;
		/// <summary>
		/// used for system draw ui (maybe need) remove later
		/// </summary>
		/// <returns></returns>
		virtual auto onImGui() -> void = 0;
		virtual auto onUpdate(float dt, Scene* scene) -> void = 0;
		virtual auto release() -> void = 0;
	};

	
};