#pragma once
#include "core/core.h"
#include "IResource.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace pengine
{
	class  File
	{
	  public:
		File();
		explicit File(const std::string &file, bool write = false);
		~File();
		auto exists() -> bool;
		auto getMd5() -> std::string;
		auto isDirectory() -> bool;

		inline auto getFileSize() const
		{
			return fileSize;
		}
		inline auto getOffset() const
		{
			return pos;
		}

		auto readBytes(int32_t size) -> std::unique_ptr<int8_t[]>;
		auto cache(const std::vector<uint8_t> &buffer) -> void;
		auto getBuffer() -> std::unique_ptr<int8_t[]>;
		auto write(const std::string &file) -> void;

		static auto getFileName(const std::string &file) -> const std::string;
		static auto fileExists(const std::string &file) -> bool;
		static auto removeExtension(const std::string &file) -> std::string;
		static auto read(const std::string &name) -> std::unique_ptr<std::vector<uint8_t>>;
		static auto list(const std::function<bool(const std::string &)> &predict = nullptr) -> const std::vector<std::string>;
		static auto list(std::vector<std::string> &out, const std::function<bool(const std::string &)> &predict = nullptr) -> void;
		static auto listFolder(const std::string &path, std::vector<std::string> &out, const std::function<bool(const std::string &)> &predict = nullptr) -> void;
		static auto getFileType(const std::string &path) -> FileType;
		static auto create(const std::string &file) -> void;
		static auto openFile(const std::function<void(int32_t, const std::string &)> &) -> void;

		inline static auto isKindOf(const std::string &file, FileType type) -> bool
		{
			return getFileType(file) == type;
		}

	  private:
		std::string file;
		FILE *      filePtr = nullptr;
		int64_t     pos     = 0;
		size_t      fileSize;
	};
};       
