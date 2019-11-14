#ifndef CmrPackItemTypeHelper_h__
#define CmrPackItemTypeHelper_h__

#include <stdint.h>
#include "CmrPackType.h"
#include <complex>

namespace cmr
{
	template<typename T>
	struct PackItemTypeHelper
	{
		static const unsigned short value = 0xFFFF;
	};

	template<>
	struct PackItemTypeHelper<char>
	{
		static const unsigned short value = mptChar;
	};

	template<>
	struct PackItemTypeHelper<unsigned char>
	{
		static const unsigned short value = mptUChar;
	};

	template<>
	struct PackItemTypeHelper<int16_t>
	{
		static const unsigned short value = mptInt16;
	};

	template<>
	struct PackItemTypeHelper<uint16_t>
	{
		static const unsigned short value = mptUInt16;
	};

	template<>
	struct PackItemTypeHelper<int32_t>
	{
		static const unsigned short value = mptInt32;
	};

	template<>
	struct PackItemTypeHelper<uint32_t>
	{
		static const unsigned short value = mptUInt32;
	};

	template<>
	struct PackItemTypeHelper<int64_t>
	{
		static const unsigned short value = mptInt64;
	};

	template<>
	struct PackItemTypeHelper<uint64_t>
	{
		static const unsigned short value = mptUInt64;
	};

	template<>
	struct PackItemTypeHelper<float>
	{
		static const unsigned short value = mptFloat;
	};

	template<>
	struct PackItemTypeHelper<double>
	{
		static const unsigned short value = mptDouble;
	};

	template<>
	struct PackItemTypeHelper<std::complex<float>>
	{
		static const unsigned short value = 0x102;
	};

	template<>
	struct PackItemTypeHelper<wchar_t>
	{
		static const unsigned short value = mptWChar;
	};
}

#endif // CmrPackItemTypeHelper_h__
