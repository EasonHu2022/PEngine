#pragma once
#include "core/core.h"
#include "Plane.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <string>
namespace pengine
{
	class BoundingBox;

	class PENGINE_API Frustum
	{
	  public:
		enum FrustumPlane
		{
			PlaneNear = 0,
			PlaneLeft,
			PlaneRight,
			PlaneUp,
			PlaneDown,
			PlaneFar
		};

		friend class RenderGraph;
		friend class GeometryRenderer;
		static constexpr uint32_t FRUSTUM_VERTICES = 8;

		Frustum() {};

		auto from(const glm::mat4 &projection) -> void;

		auto isInside(const glm::vec3 &pos) const -> bool;
		auto isInside(const BoundingBox& box) const->bool;
		auto isInside(const std::shared_ptr<BoundingBox>& box) const->bool;

		inline auto &getPlane(FrustumPlane id) const
		{
			return planes[id];
		}

		inline auto getVertices() -> glm::vec3*
		{
			return vertices;
		}

	  private:
		Plane     planes[6];
		glm::vec3 vertices[FRUSTUM_VERTICES];
	};

};       
