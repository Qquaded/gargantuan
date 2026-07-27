#include "gargantuan/classes/Tween.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	const Tween::ClassDefinition Tween::DEFINITION = {
		.Name = "Tween",
		.Superclass = "Instance",
		// Constructed via TweenService
		.Properties =
			{G_UD_READONLY_PROP(Tween, instance, Instance::Pointer),
			 G_UD_READONLY_PROP(Tween, tweenInfo, TweenInfo),
			 G_UD_READONLY_PROP(Tween, playbackState, Enums::PlaybackState)},
		.Methods = {
			G_UD_METHOD(Tween, Play),
			G_UD_METHOD(Tween, Pause),
			G_UD_METHOD(Tween, Cancel),
		}
	};

	void Tween::Play() {
		if (playbackState == Enums::PlaybackState::Playing) {
			return; // we're already playing
		}

		isPaused = false;
		isCancelled = false;

		if (t <= 0.0f && tweenInfo.DelayTime > 0.0f) {
			playbackState = Enums::PlaybackState::Delayed;
		} else {
			playbackState = Enums::PlaybackState::Playing;
		}

		if (startProperties.empty()) {
			for (auto &[name, goalValue] : goalProperties) {
				startProperties[name];
			}
		}
	}

	void Tween::Pause() {
		if (playbackState != Enums::PlaybackState::Playing) {
			return;
		}

		isPaused = true;
		playbackState = Enums::PlaybackState::Paused;
	}

	void Tween::Cancel() {
		isCancelled = true;
		isPaused = false;
		t = 0.0f;
		playbackState = Enums::PlaybackState::Cancelled;

		Completed->Fire(playbackState);
	}

	void Tween::Step(float deltaTime) {
		if (playbackState == Enums::PlaybackState::Delayed) {
			delayElapsed += deltaTime;
			return;
		} else if (playbackState != Enums::PlaybackState::Playing) {
			return;
		}

		t += deltaTime;
		// do stuff
	}
}