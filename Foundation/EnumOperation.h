#pragma once
#include <type_traits>

template<typename Enum>  
struct EnableBitMaskOperators  
{
    static const bool enable = false;
};

#define ENUM_OPERATOR_OVERLOAD(o) \
template<typename Enum> \
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type \
operator##o##(Enum lhs, Enum rhs) \
{\
	static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type"); \
\
	using underlying = typename std::underlying_type<Enum>::type; \
\
	return static_cast<Enum> ( \
		static_cast<underlying>(lhs) ##o## \
		static_cast<underlying>(rhs) \
		);\
}\
template<typename Enum> \
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type \
operator##o##=(Enum& lhs, Enum rhs) \
{\
	static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type"); \
\
	using underlying = typename std::underlying_type<Enum>::type; \
\
	lhs = static_cast<Enum> ( \
		static_cast<underlying>(lhs) ##o## \
		static_cast<underlying>(rhs) \
		); \
	return lhs; \
}

#define ENABLE_ENUM_BITMASK_OPERATORS(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
};

ENUM_OPERATOR_OVERLOAD(&)
ENUM_OPERATOR_OVERLOAD(|)
ENUM_OPERATOR_OVERLOAD(^)