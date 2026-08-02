// Copyright (c) 2026 Team Fireworks
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <string>

namespace gargantuan {
	class FileLink : public Instance {
		G_INSTANCE_DECL(FileLink);

		std::string Path;
		bool IgnoreUnknownDescendants = false;

		void Synchronize();
	};
}
