#pragma once


// Template
namespace SS
{
	template<typename T>
	T Clamp(T In, T Min, T Max)
	{
		In = In < Min ? Min : In;
		In = In > Max ? Max : In;
		return In;
	}
}