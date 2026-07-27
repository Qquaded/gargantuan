#include "gargantuan/classes/Tween.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/math/EasingCurves.hpp"
#include "gargantuan/scripting/Userdata.hpp"

// Todo: make play into a LPlay luau function cuz we need lua_State to
// read/write stuff
namespace gargantuan {
	const Tween::ClassDefinition Tween::DEFINITION = {
		.Name = "Tween",
		.Superclass = "Instance",
		.Properties =
			{
				G_UD_READONLY_PROP(Tween, instance, Instance::Pointer),
				G_UD_READONLY_PROP(Tween, TweenInfo, gargantuan::TweenInfo),
				G_UD_READONLY_PROP(Tween, PlaybackState, Enums::PlaybackState),
			},
		.Methods = {
			G_UD_METHOD(Tween, Play),
			G_UD_METHOD(Tween, Pause),
			G_UD_METHOD(Tween, Cancel),
		}
	};

	void Tween::Play() {
		if (PlaybackState == Enums::PlaybackState::Playing) return;

		Paused = false;
		Cancelled = false;

		if (Elapsed <= 0.0f && TweenInfo.DelayTime > 0.0f) {
			PlaybackState = Enums::PlaybackState::Delayed;
		} else {
			PlaybackState = Enums::PlaybackState::Playing;
		}

		if (InitialProperties.empty()) {
			for (auto &[name, goalValue] : goalProperties) {
				InitialProperties[name];
			}
		}
	}

	void Tween::Pause() {
		if (PlaybackState != Enums::PlaybackState::Playing) {
			return;
		}

		Paused = true;
		PlaybackState = Enums::PlaybackState::Paused;
	}

	void Tween::Cancel() {
		Cancelled = true;
		Paused = false;
		Elapsed = 0.0f;
		PlaybackState = Enums::PlaybackState::Cancelled;

		Completed->Fire(PlaybackState);
	}

	void Tween::Step(float deltaTime) {
		if (PlaybackState == Enums::PlaybackState::Delayed) {
			DelayElapsed += deltaTime;
			return;
		} else if (PlaybackState != Enums::PlaybackState::Playing) {
			return;
		}

		Elapsed += deltaTime;

		auto endTime = Elapsed + TweenInfo.Time;
		auto progress = Elapsed / endTime;
		auto alpha = EasingCurves::CalculateAlpha(progress);

		for (auto &[name, goalValue] : goalProperties) {
			InitialProperties[name];
		}
	}
}
