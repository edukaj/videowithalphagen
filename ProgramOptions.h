#pragma once

#include <string>
#include <memory>
#include <iosfwd>
#include <boost/filesystem.hpp>

class ProgramOptions {
public:
	ProgramOptions(int argc, char* argv[]);
	~ProgramOptions();

	ProgramOptions(const ProgramOptions&) = delete;
	ProgramOptions(ProgramOptions&&) = delete;

	ProgramOptions& operator = (const ProgramOptions&) = delete;
	ProgramOptions& operator = (const ProgramOptions&&) = delete;

	bool shouldDisplayOnlyHelp() const noexcept;
	bool shouldDisplayOnlyVersion() const noexcept;
	const std::string& prefix() const noexcept;
	double fps() const noexcept;
	int fourcc() const noexcept;
	int verbose() const noexcept;
	const std::string& videoName() const noexcept;
	const std::string& videoExtension() const noexcept;

    int videoMode() const noexcept;

	friend std::ostream& operator << (std::ostream& os, const ProgramOptions& options);

private:
	struct Impl;
	std::unique_ptr<Impl> m_Impl;
};
