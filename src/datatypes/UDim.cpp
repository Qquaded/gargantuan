#include "gargantuan/datatypes/UDim.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

namespace gargantuan {
	G_USERDATA_IMPL(
		UDim,
		.Tag = UserdataTag::UDim,
		.Type = "UDim",
		.Properties = {
			{"Scale", Property::fromReadonlyMember<&UDim::Scale>()},
			{"Offset", Property::fromReadonlyMember<&UDim::Offset>()},
		}
	);

	UDim::UDim(float scale, int offset) : Scale(scale), Offset(offset) {};

} // namespace gargantuan
