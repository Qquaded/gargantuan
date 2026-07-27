#pragma once

#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"
#include <ext/vector_float3.hpp>
#include <lua.h>
#include <string>
#include <unordered_map>
#include <variant>

namespace gargantuan {
	G_ENUM(
		PlaybackState,

		Begin,
		Delayed,
		Playing,
		Paused,
		Completed,
		Cancelled
	)

	class Tween : public Instance {
	  public:
		static const ClassDefinition DEFINITION;

		Instance::Pointer instance;
		TweenInfo TweenInfo;

		using TweenableValue = std::variant<float, Color3, Vector2, glm::vec3>;
		using GoalPropertyMap = std::unordered_map<std::string, TweenableValue>;
		GoalPropertyMap goalProperties;

		Enums::PlaybackState PlaybackState;

		Tween(Instance::Pointer instance, gargantuan::TweenInfo tweenInfo, GoalPropertyMap goalProperties);

		void Play();
		void Cancel();
		void Pause();
		void Step(float deltaTime);

		G_SIGNAL(Completed, Enums::PlaybackState)
	  private:
		float Elapsed = 0.0f;
		float DelayElapsed = 0.0f;
		bool Paused = false;
		bool Cancelled = false;
		GoalPropertyMap InitialProperties;
	};
}
