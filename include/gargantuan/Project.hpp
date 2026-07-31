#pragma once

#include "gargantuan/assets/InstanceSerialization.hpp"
#include <filesystem>
#include <string>
#include <tuple>

namespace gargantuan {

	class Project {
		std::filesystem::path Root;

		static Project fromRoot(std::filesystem::path root) {}

		std::tuple<std::string, InstanceSerialization::InstanceFormat> ResolvePlaceFile() {}
	};
}
