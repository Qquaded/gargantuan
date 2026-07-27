#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	G_UD_IMPL_PRELUDE(TweenInfo);
    G_UD_IMPL_PROPS(
        TweenInfo,
        G_UD_READONLY_PROP(TweenInfo, Time, float),
        G_UD_READONLY_PROP(TweenInfo, EasingStyle, Enums::EasingStyle),
        G_UD_READONLY_PROP(TweenInfo, EasingDirection, Enums::EasingDirection),
        G_UD_READONLY_PROP(TweenInfo, RepeatCount, int32_t),
        G_UD_READONLY_PROP(TweenInfo, Reverses, bool),
        G_UD_READONLY_PROP(TweenInfo, DelayTime, float)
    )
	G_UD_IMPL_METHODS(TweenInfo)
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