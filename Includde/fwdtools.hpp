#ifndef __FWD_TOOLS_HPP__
#define __FWD_TOOLS_HPP__
#include "enumTools.hpp"


// Mask Type 返回掩码类型
 // 类型提升 BUG --------FIXED
// Mask Type 返回掩码类型 使用位宽 1 2 4 8
template <SectionWidth BSSTYLE>
struct MaskType
{
	using type = __int8;
};

// Mask Type 返回掩码类型 当使用的 位宽是 16 bit
template <>
struct MaskType<SectionWidth::SixteenBit>
{
	using type = __int16;
};

// Mask Type 返回掩码类型 当使用的 位宽是 32 bit
template <>
struct MaskType<SectionWidth::ThirtytwoBit>
{
	using type = __int32;
};

// Mask Type 返回掩码类型 当使用的 位宽是 64 bit
//template <>
//struct MaskType<SectionWidth::SixtyfourBit>
//{
//	using type = __int64;
//};


// 快捷 掩码类型
template <SectionWidth BSSTYLE>
using MaskType_t = typename MaskType<BSSTYLE>::type;

// 位移动 , 向左移动 SHIFT 个位置
template <bool left, std::size_t shift, std::size_t value>
struct BitShift
{
	constexpr static std::size_t value = value << shift;
};

// 为移动 向右移动 SHIFT 个位置
template <std::size_t shift, std::size_t value>
struct BitShift<false, shift, value>
{
	constexpr static std::size_t value = value >> shift;
};

// 快捷位移取值
template <bool left, std::size_t shift, std::size_t value>
constexpr std::size_t BitShift_v = BitShift<left, shift, value>::value;


// 位 and
template <std::size_t left, std::size_t right>
struct BitAnd
{
	constexpr static std::size_t value = (left & right);
};

//快捷位 and 操作
template <std::size_t left, std::size_t right>
constexpr std::size_t BitAnd_v = BitAnd<left, right>::value;



// 位 or
template <std::size_t left, std::size_t right>
struct BitOr
{
	constexpr static std::size_t value = (left | right);
};

//快捷位 or 操作
template <std::size_t left, std::size_t right>
constexpr std::size_t BitOr_v = BitOr<left, right>::value;

// 掩码的递归计算
template <std::size_t shift, typename MASK_TYPE, MASK_TYPE V>
struct _MaskValue
{
	constexpr static MASK_TYPE value = _MaskValue<shift - 1, MASK_TYPE, BitOr_v<BitShift_v<true, 1, V>, 1>>::value;
};

// 掩码的递归终结返回结果掩码值
template <typename MASK_TYPE, MASK_TYPE V>
struct _MaskValue<1, MASK_TYPE, V>
{
	constexpr static MASK_TYPE value = V;
};

// 掩码值
template <typename MASK_TYPE, SectionWidth BSWIDTH>
struct MaskValue
{
	constexpr static MASK_TYPE value = _MaskValue<(int)BSWIDTH, MASK_TYPE, 1>::value;
};

// 快捷掩码值获取
template <typename MASK_TYPE, SectionWidth BSWIDTH>
constexpr MASK_TYPE MaskValue_v = MaskValue<MASK_TYPE, BSWIDTH>::value;




// 自动对应 BitWidth 的位宽和节位宽，以使单个位宽内默认仅存在一个节
// 即 默认情况下位宽进对应一个节，仅存放一个元素
// 对应失败，那么一个位对应一个节，一个位宽有 N 个节
template <BitWidth BWIDTH, SectionWidth DEFAULT>
struct TryAlignBWMapSW
{
	constexpr static SectionWidth value = DEFAULT;
};
// 一个位宽对应一个节
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::OneBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::PerBit;
};

// 两个位宽存在一个节
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::TwoBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::TwoBit;
};
// 四个位宽存在一个节
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::FourBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::FourBit;
};

// 八个位宽存在一个节
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::EightBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::EightBit;
};
// 十六各位宽存在一个节
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::TenSixBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::SixteenBit;
};
// 三十二个位宽对应一个节
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::ThreetwoBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::ThirtytwoBit;
};
// 六十四个位宽对应一个节
//template <SectionWidth DEFAULT>
//struct TryAlignBWMapSW<BitWidth::SixfourBit, DEFAULT>
//{
//	constexpr static SectionWidth value = SectionWidth::SixtyfourBit;
//};

//快捷 v
template <BitWidth BWIDTH, SectionWidth DEFAULT = SectionWidth::PerBit>
constexpr SectionWidth TryAlignBWMapSW_v = TryAlignBWMapSW<BWIDTH, DEFAULT>::value;


// 计算给定的位宽和 对应的值类型，所需的数量
template <BitWidth BWIDTH, typename VALUE_TYPE>
struct EventModelValueCount
{
private:
	constexpr static std::size_t bit_count = (static_cast<int>(BWIDTH) / (sizeof(VALUE_TYPE) * 8));
public:
	constexpr static std::size_t value = bit_count == 0 ? 1 : bit_count;
};

// 快捷 v
template <BitWidth BWIDTH, typename VALUE_TYPE>
constexpr std::size_t EventModelValueCount_v = EventModelValueCount<BWIDTH, VALUE_TYPE>::value;

// 检查 SectionWidth 是否小于 BitWIdth
template <BitWidth BWIDTH,SectionWidth BSWIDTH>
struct IsSecWidthLessBitWidth
{
	constexpr static bool value = static_cast<int>(BSWIDTH) <= static_cast<int>(BWIDTH) ? true : false;
};
// 快捷 v
template <BitWidth BWIDTH, SectionWidth BSWIDTH>
constexpr bool IsSecWidthLessBitWidth_v = IsSecWidthLessBitWidth<BWIDTH, BSWIDTH>::value;



// 计算对应类型的位数量
template <typename T>
struct BitCountOfValueType;

// 快捷 v
template <typename T>
constexpr std::size_t BitCountOfValueType_v = BitCountOfValueType<T>::value;

template <>
struct BitCountOfValueType<__int8>
{
	constexpr static std::size_t value = 8;
};

template <>
struct BitCountOfValueType<__int16>
{
	constexpr static std::size_t value = 16;
};

template <>
struct BitCountOfValueType<__int32>
{
	constexpr static std::size_t value = 32;
};

template <>
struct BitCountOfValueType<__int64>
{
	constexpr static std::size_t value = 64;
};

// max element
template <BitWidth BWIDTH,SectionWidth BSWIDTH>
struct EventModelMaxElement
{
	constexpr static std::size_t value = static_cast<int>(BWIDTH) / static_cast<int>(BSWIDTH);
};
// 快捷 v
template <BitWidth BWIDTH, SectionWidth BSWIDTH>
constexpr std::size_t EventModelMaxElement_v = EventModelMaxElement<BWIDTH, BSWIDTH>::value;
#endif // !__FWD_TOOLS_HPP__
