#ifndef __ENUM_TOOLS_HPP__
#define __ENUM_TOOLS_HPP__

// 一系列值占用空间的长度
enum struct BitWidth :short
{
	OneBit = 1,
	TwoBit = 2,
	FourBit = 4,
	EightBit = 8,
	TenSixBit = 16,
	ThreetwoBit = 32,
	SixfourBit = 64,
	OnetwoeightBit = 128,
	TwofivesixBit = 256,
	FiveonetwoBit = 512,
	OnezerotwofourBit = 1024,
	TwozerofoureightBit = 2048,
	FourzeroninesizBit = 4096,
	EightoneninetwoBit = 8192
};

// 单个值的位宽
enum struct SectionWidth
{
	PerBit = 1,
	TwoBit = 2,
	FourBit = 4,
	EightBit = 8,
	SixteenBit = 16,
	ThirtytwoBit = 32,
	//SixtyfourBit = 64
};

#endif // !__ENUM_TOOLS_HPP__
