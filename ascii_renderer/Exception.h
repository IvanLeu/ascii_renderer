#pragma once

#include <exception>
#include <string>
#include <Windows.h>

class Exception : public std::exception {
public:
	Exception(int line, const char* file) noexcept;
	virtual ~Exception() = default;
	virtual const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
protected:
	int line_;
	std::string file_;
	mutable std::string whatBuffer_;
};

class HrException final : public Exception {
public:
	HrException(HRESULT hr, int line, const char* file) noexcept;
	static std::string TranslateErrorCode(HRESULT hr);
	virtual const char* what() const noexcept override;
	virtual const char* GetType() const noexcept override;
private:
	HRESULT hr_;
};

#define HR_EXCEPTION(hr) HrException::HrException((hr), __LINE__, __FILE__)
#define HR_EXCEPTION_LAST HrException::HrException(GetLastError(), __LINE__, __FILE__)