#pragma once

#include "gargantuan/classes/ServiceProvider.hpp"

namespace gargantuan {
	class DataModel : public ServiceProvider {
	  public:
		G_INSTANCE_DECL(DataModel);

		const ServiceConstructors &GetServiceConstructors() const override;
	};
} // namespace gargantuan
