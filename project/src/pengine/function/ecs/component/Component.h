#pragma once
namespace pengine
{
	class Component
	{
	public:
		Component();
		~Component();
	private:
		bool b_IsActive = true;
	};
}