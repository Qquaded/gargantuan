#pragma once

#include <box3d/box3d.h>
#include <box3d/math_functions.h>
#include <ext/quaternion_float.hpp>
#include <glm.hpp>
#include <gtc/quaternion.hpp>

namespace gargantuan {
	inline b3Vec3 ToBox3(const glm::vec3 &vector) {
		return b3Vec3{vector.x, vector.y, vector.z};
	}

	inline b3Quat ToBox3(const glm::quat &quaternion) {
		return b3NormalizeQuat(b3Quat{{quaternion.x, quaternion.y, quaternion.z}, quaternion.w});
	}

	inline glm::vec3 FromBox3(const b3Vec3 &vector) {
		return glm::vec3(vector.x, vector.y, vector.z);
	}

	inline glm::quat FromBox3(const b3Quat &quaternion) {
		return glm::quat(quaternion.s, quaternion.v.x, quaternion.v.y, quaternion.v.z);
	}
} // namespace gargantuan
