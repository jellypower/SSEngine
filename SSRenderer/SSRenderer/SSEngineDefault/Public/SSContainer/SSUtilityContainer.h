#pragma once
#pragma warning(disable: 4996)

#include "utility"

namespace SS {


#pragma region move, forward implementation

	template <class T>
	struct remove_reference_struct {
		using type = T;
	};

	template <class T>
	struct remove_reference_struct<T&> {
		using type = T;
	};

	template <class T>
	struct remove_reference_struct<T&&> {
		using type = T;
	};

	template <class T>
	using remove_reference = typename remove_reference_struct<T>::type;

	template <class T>
	constexpr remove_reference<T>&& move(T&& arg) noexcept {
		return static_cast<remove_reference<T>&&>(arg);
	}

	template <class T>
	constexpr T&& forward(remove_reference<T>& arg) noexcept {
		return static_cast<T&&>(arg);
	}

#pragma endregion


	template<typename T1, typename T2>
	class pair
	{
	public:
		T1 first;
		T2 second;
	public:
		pair(const T1& InFirst, const T2& InSecond) : first(InFirst), second(InSecond) { }
		pair(T1&& InFirst, T2&& InSecond) : first(SS::forward<T1>(InFirst)), second(SS::forward<T2>(InSecond)) {}
	};



	template<typename T1, typename T2>
	void MakePair(const T1& item1, const T2& item2)
	{
		return pair<T1, T2>(item1, item2);
	}

	template <typename T1, typename T2>
	constexpr pair<remove_reference<T1>, remove_reference<T2>> MakePair(T1&& item1, T2&& item2)
	{
		using Internal_pair = pair<remove_reference<T1>, remove_reference<T2>>;
		return Internal_pair(SS::forward<T1>(item1), SS::forward<T2>(item2));
	}
};