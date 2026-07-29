#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

namespace gargantuan {
	G_USERDATA_IMPL(
		TweenInfo,
		.Tag = UserdataTag::TweenInfo,
		.Type = "TweenInfo",
		.Properties = {
			{"TweenInfo", Property::fromReadonlyMember<&TweenInfo::Time>()},
			{"EasingStyle", Property::fromReadonlyMember<&TweenInfo::EasingStyle>()},
			{"EasingDirection", Property::fromReadonlyMember<&TweenInfo::EasingDirection>()},
			{"RepeatCount", Property::fromReadonlyMember<&TweenInfo::RepeatCount>()},
			{"Reverses", Property::fromReadonlyMember<&TweenInfo::Reverses>()},
			{"DelayTime", Property::fromReadonlyMember<&TweenInfo::DelayTime>()},
			{"TweenInfo", Property::fromReadonlyMember<&TweenInfo::Time>()},
		}
	);

	TweenInfo::TweenInfo(
		float time,
		Enums::EasingStyle easingStyle,
		Enums::EasingDirection easingDirection,
		int32_t repeatCount,
		bool reverses,
		float delayTime
	)
		: Time(time), EasingStyle(easingStyle), EasingDirection(easingDirection), RepeatCount(repeatCount),
		  Reverses(reverses), DelayTime(delayTime) {};
}
