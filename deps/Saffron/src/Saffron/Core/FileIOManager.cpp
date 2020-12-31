#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"

namespace Se
{

const Window *FileIOManager::_window = nullptr;

FileIOManager::Filter FileIOManager::Filter::Empty()
{
	return Filter("All Files", { "*.*" });
}

void FileIOManager::Init(const Window &window)
{
	_window = &window;
}

ArrayList<DirectoryEntry> FileIOManager::GetFiles(const Filepath &directoryPath,
												  const String &extension)
{
	ArrayList<DirectoryEntry> output;

	try
	{
		std::copy_if(fs::directory_iterator(directoryPath), fs::directory_iterator{}, std::back_inserter(output), [&](const DirectoryEntry &entry)
					 {
						 return entry.path().extension() == extension;
					 });
	}
	catch ( fs::filesystem_error &fe )
	{
		SE_CORE_WARN("Failed to get files from directory: {} with file extension: {}. What: ", directoryPath.string(), extension, fe.what());
	}

	return output;
}

size_t FileIOManager::GetFileCount(const Filepath &directoryPath, const String &extension)
{
	// Return early if no extension is given
	if ( extension.empty() )
	{
		return std::distance(fs::directory_iterator(directoryPath), fs::directory_iterator{});
	}

	try
	{
		return std::count_if(fs::directory_iterator(directoryPath), fs::directory_iterator{}, [&](const DirectoryEntry &entry)
							 {
								 return entry.path().extension() == extension;
							 });
	}
	catch ( fs::filesystem_error &fe )
	{
		SE_CORE_WARN("Failed to get file count from directory: {} with file extension: {}. What: ", directoryPath.string(), extension, fe.what());
	}
	return 0;
}

size_t FileIOManager::Write(const Uint8 *data, size_t size, const Filepath &filepath, bool overwrite)
{
	const bool fileExists = FileExists(filepath);
	if ( !fileExists || fileExists && overwrite )
	{
		OutputStream ofstream;
		ofstream.open(filepath);
		if ( ofstream.good() )
		{
			const auto start = ofstream.tellp();
			ofstream.write(reinterpret_cast<const Int8 *>(data), size);
			return ofstream.tellp() - start;
		}
		SE_CORE_WARN("Failed to open file: " + filepath.string());
	}
	return 0;
}

size_t FileIOManager::Write(const Buffer buffer, const Filepath &filepath, bool overwrite)
{
	return Write(buffer.Data(), buffer.Size(), filepath, overwrite);
}

bool FileIOManager::CreateDirectories(const Filepath &filepath)
{
	std::error_code errorCode;
	return fs::create_directories(filepath, errorCode);
}

bool FileIOManager::FileExists(const Filepath &filepath)
{
	std::error_code errorCode;
	return fs::exists(filepath, errorCode);
}

bool FileIOManager::Copy(const Filepath &source, const Filepath &destination)
{
	std::error_code errorCode;
	fs::copy_file(source, destination, errorCode);
	return static_cast<bool>(errorCode);
}
}
