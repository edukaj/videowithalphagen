#pragma once

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include <iosfwd>

class ProgramOptions;

struct Frame
{
	std::string absolutePath;
	std::string name;
	std::string ext;
	int index;

	bool operator < (const Frame& rhs ) const noexcept
	{
		return index < rhs.index;
	}

	friend std::ostream& operator << (std::ostream& os, const Frame& f);
};

class VideoConverter {
public:
	explicit VideoConverter(const ProgramOptions& po);
	~VideoConverter();

	VideoConverter(const VideoConverter&) = delete;
	VideoConverter& operator = (const VideoConverter&) = delete;
	VideoConverter(VideoConverter&&) = delete;
	VideoConverter& operator = (VideoConverter&&) = delete;

	void generateVideo();

	const std::vector<boost::filesystem::path>& foundImages() const noexcept;
	const std::vector<Frame>& frames() const noexcept;

private:
	struct Impl;
	std::unique_ptr<Impl> m_Impl;
};
