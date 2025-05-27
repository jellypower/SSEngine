#pragma once

class INoncopyable
{
public:
	INoncopyable() = default;
	INoncopyable(const INoncopyable& rhs) = delete;
	INoncopyable& operator=(const INoncopyable& rhs) = delete;
	virtual ~INoncopyable() = default;
};
