#pragma once

#include "gargantuan/assets/InstanceSerialization.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace gargantuan {
	class Project {
	  public:
		std::filesystem::path Root;
		std::filesystem::path RootConfiguration;
		std::filesystem::path InstanceFilePath;
		InstanceSerialization::InstanceFormat InstanceFileFormat;

		static Project fromInit(
			std::filesystem::path root,
			std::string projectName = "Untitled",
			Instance::Pointer instance = nullptr,
			InstanceSerialization::InstanceFormat format = InstanceSerialization::InstanceFormat ::Json
		);

		static Project fromExisting(std::filesystem::path root);

		std::shared_ptr<DataModel> DeserializeGame();
		void SerializeGame(std::shared_ptr<DataModel> game);

	  private:
		Project(std::filesystem::path root);
	};
}
