#pragma once

#include "gargantuan/scripting/UserdataMethod.hpp"
#include "gargantuan/scripting/UserdataProperty.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"
#include <string_view>
#include <unordered_map>
namespace gargantuan {
	template <typename Self> struct UserdataDefinition {
		UserdataTag Tag = UserdataTag::None;
		std::string_view Type = "nil";
		std::unordered_map<std::string_view, UserdataProperty<Self>> Properties = {};
		std::unordered_map<std::string_view, UserdataMethod<Self>> Methods = {};
	};
}
