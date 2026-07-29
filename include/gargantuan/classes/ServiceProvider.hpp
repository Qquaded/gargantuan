#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <functional>
#include <string_view>
#include <unordered_map>

namespace gargantuan {
	class ServiceProvider : public Instance {
	  public:
		G_INSTANCE_DECL(ServiceProvider);

		typedef std::unordered_map<std::string, std::function<Instance::Pointer()>> ServiceConstructors;
		std::unordered_map<std::string, Instance::Pointer> Services;

		virtual Instance::Pointer FindService(std::string_view name);
		virtual Instance::Pointer GetService(std::string_view name);
		virtual const ServiceConstructors &GetServiceConstructors() const = 0;
	};
} // namespace gargantuan
