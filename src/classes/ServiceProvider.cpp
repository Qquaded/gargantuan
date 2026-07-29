#include "gargantuan/classes/ServiceProvider.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL_log.h>
#include <format>
#include <stdexcept>
#include <string_view>

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(
		ServiceProvider,
		.Methods = {
			{"FindService", Method::fromMember<&ServiceProvider::FindService>()},
			{"GetService", Method::fromMember<&ServiceProvider::GetService>()},
		},
	);

	Instance::Pointer ServiceProvider::FindService(std::string_view name) {
		auto it = Services.find(std::string(name));
		if (it != Services.end()) {
			return it->second;
		}
		return nullptr;
	}

	Instance::Pointer ServiceProvider::GetService(std::string_view nameView) {
		auto name = std::string(nameView);
		auto it = Services.find(name);
		if (it == Services.end()) {
			const ServiceConstructors &constructors = GetServiceConstructors();
			if (auto constructor = constructors.find(name); constructor != constructors.end()) {
				if (!constructor->second) {
					throw std::runtime_error("Missing constructor for service " + std::string(name));
				}
				auto service = constructor->second();
				service->SetParent(this->shared_from_this());
				Services.emplace(name, service);
				return service;
			} else {
				throw std::runtime_error(std::format("Unknown service named '{}'", name));
			}
		}
		return it->second;
	}
} // namespace gargantuan
