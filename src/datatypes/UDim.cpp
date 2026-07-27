#include "gargantuan/datatypes/UDim.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	G_UD_IMPL_PRELUDE(UDim);
	G_UD_IMPL_PROPS(
		UDim,

		G_UD_READONLY_PROP(UDim, Scale, float),
		G_UD_READONLY_PROP(UDim, Offset, int)
	)
	G_UD_IMPL_METHODS(UDim)

	UDim::UDim(float scale, int offset) : Scale(scale), Offset(offset) {};

} // namespace gargantuan
