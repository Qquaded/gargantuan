#include "gargantuan/scripting/Userdata.hpp"
#include <glm/glm.hpp>
#include <lua.h>

namespace gargantuan {
	G_USERDATA_DECL(
		UDim,

		float Scale = 0.0f;
		int Offset = 0;

		UDim(float scale = 0.0f, int offset = 0);
	)
}
