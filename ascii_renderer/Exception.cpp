#include "Exception.h"
#include <sstream>
#include <format>

Exception::Exception(int line, const char* file) noexcept
	:
	line_(line),
	file_(file)
{}

const char* Exception::what() const noexcept
{
	whatBuffer_ = std::format(
		"File: {}\n Line: {}",
		file_, 
		line_);

	return whatBuffer_.c_str();
}

const char* Exception::GetType() const noexcept
{
	return "Base Exception";
}

HrException::HrException(HRESULT hr, int line, const char* file) noexcept
	:
	Exception(line, file),
	hr_(hr)
{
}

std::string HrException::TranslateErrorCode(HRESULT hr)
{
	std::string buffer;
	buffer.resize_and_overwrite(
		256,
		[&](char* p, size_t cap) -> size_t {
			DWORD count = FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
				NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), p, static_cast<DWORD>(cap), NULL
			);
				
			if (count == 0) {
				return 0;
			}

			return static_cast<size_t>(count);
		}
	);

	return buffer;
}

const char* HrException::what() const noexcept
{
	whatBuffer_ = std::format(
		"File: {}\n Line: {}\n Description: {}",
		file_,
		line_,
		TranslateErrorCode(hr_));

	return whatBuffer_.c_str();
}

const char* HrException::GetType() const noexcept
{
	return "HResult Exception";
}
