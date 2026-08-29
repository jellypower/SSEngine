#pragma once

class ISSNoncopyable
{
public:
	ISSNoncopyable() = default;
	ISSNoncopyable& operator=(const ISSNoncopyable& rhs) = delete;
	ISSNoncopyable(const ISSNoncopyable& rhs) = delete;
	virtual ~ISSNoncopyable() = default;
};