#include "gargantuan/filesystem/BaseFilesystem.hpp"

#include <format>

namespace gargantuan {
	void BaseFilesystem::Copy(const std::filesystem::path &source, const std::filesystem::path &destination) {
		if (!Exists(source)) throw std::format("File {} does not exist", source.c_str());

		if (Type(source) == FileType::File) {
			if (Exists(destination) && Type(source) != FileType::File) {
				throw std::format("Cannot copy file {} to non-file {}", source.c_str(), destination.c_str());
			}

			auto sourceHandle = Open(source, FileOpen::Read);
			auto destinationHandle = Open(destination, FileOpen::Write);

			void *sourceContents;
			size_t sourceSize = sourceHandle.Size();
			sourceHandle.Read(sourceContents, sourceSize);
			destinationHandle.Write(sourceContents, sourceSize);

			sourceHandle.Close();
			destinationHandle.Close();
			// } else if (Type(source) == FileType::Directory) {
			// 	if (Exists(destination) && Type(source) != FileType::Directory) {
			// 		throw std::format("Cannot copy directory {} to non-directory {}", source.c_str(),
			// destination.c_str());
			// 	}
		} else {
			throw std::format("Unsupported source file type");
		}
	};

	void BaseFilesystem::Move(const std::filesystem::path &source, const std::filesystem::path &destination) {
		throw "not yet implemented";
	};

	std::string BaseFilesystem::ReadFileToString(const std::filesystem::path &path) {
		throw "not yet implemented";
	};

	void BaseFilesystem::WriteStringToFile(const std::filesystem::path &path, std::string) {
		throw "not yet implemented";
	};

	std::istringstream BaseFilesystem::ReadFileToStringStream(const std::filesystem::path &path) {
		throw "not yet implemented";
	};

	void BaseFilesystem::WriteStringStreamToFile(const std::filesystem::path &path, std::ostringstream output) {
		throw "not yet implemented";
	};
}
