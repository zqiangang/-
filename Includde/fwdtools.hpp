#ifndef __FWD_TOOLS_HPP__
#define __FWD_TOOLS_HPP__
#include "enumTools.hpp"


// Mask Type ������������
 // �������� BUG --------FIXED
// Mask Type ������������ ʹ��λ�� 1 2 4 8
template <SectionWidth BSSTYLE>
struct MaskType
{
	using type = __int8;
};

// Mask Type ������������ ��ʹ�õ� λ���� 16 bit
template <>
struct MaskType<SectionWidth::SixteenBit>
{
	using type = __int16;
};

// Mask Type ������������ ��ʹ�õ� λ���� 32 bit
template <>
struct MaskType<SectionWidth::ThirtytwoBit>
{
	using type = __int32;
};

// Mask Type ������������ ��ʹ�õ� λ���� 64 bit
//template <>
//struct MaskType<SectionWidth::SixtyfourBit>
//{
//	using type = __int64;
//};


// ��� ��������
template <SectionWidth BSSTYLE>
using MaskType_t = typename MaskType<BSSTYLE>::type;

// λ�ƶ� , �����ƶ� SHIFT ��λ��
template <bool left, std::size_t shift, std::size_t value>
struct BitShift
{
	constexpr static std::size_t value = value << shift;
};

// Ϊ�ƶ� �����ƶ� SHIFT ��λ��
template <std::size_t shift, std::size_t value>
struct BitShift<false, shift, value>
{
	constexpr static std::size_t value = value >> shift;
};

// ���λ��ȡֵ
template <bool left, std::size_t shift, std::size_t value>
constexpr std::size_t BitShift_v = BitShift<left, shift, value>::value;


// λ and
template <std::size_t left, std::size_t right>
struct BitAnd
{
	constexpr static std::size_t value = (left & right);
};

//���λ and ����
template <std::size_t left, std::size_t right>
constexpr std::size_t BitAnd_v = BitAnd<left, right>::value;



// λ or
template <std::size_t left, std::size_t right>
struct BitOr
{
	constexpr static std::size_t value = (left | right);
};

//���λ or ����
template <std::size_t left, std::size_t right>
constexpr std::size_t BitOr_v = BitOr<left, right>::value;

// ����ĵݹ����
template <std::size_t shift, typename MASK_TYPE, MASK_TYPE V>
struct _MaskValue
{
	constexpr static MASK_TYPE value = _MaskValue<shift - 1, MASK_TYPE, BitOr_v<BitShift_v<true, 1, V>, 1>>::value;
};

// ����ĵݹ��ս᷵�ؽ������ֵ
template <typename MASK_TYPE, MASK_TYPE V>
struct _MaskValue<1, MASK_TYPE, V>
{
	constexpr static MASK_TYPE value = V;
};

// ����ֵ
template <typename MASK_TYPE, SectionWidth BSWIDTH>
struct MaskValue
{
	constexpr static MASK_TYPE value = _MaskValue<(int)BSWIDTH, MASK_TYPE, 1>::value;
};

// �������ֵ��ȡ
template <typename MASK_TYPE, SectionWidth BSWIDTH>
constexpr MASK_TYPE MaskValue_v = MaskValue<MASK_TYPE, BSWIDTH>::value;




// �Զ���Ӧ BitWidth ��λ��ͽ�λ����ʹ����λ����Ĭ�Ͻ�����һ����
// �� Ĭ�������λ�����Ӧһ���ڣ������һ��Ԫ��
// ��Ӧʧ�ܣ���ôһ��λ��Ӧһ���ڣ�һ��λ���� N ����
template <BitWidth BWIDTH, SectionWidth DEFAULT>
struct TryAlignBWMapSW
{
	constexpr static SectionWidth value = DEFAULT;
};
// һ��λ���Ӧһ����
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::OneBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::PerBit;
};

// ����λ�����һ����
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::TwoBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::TwoBit;
};
// �ĸ�λ�����һ����
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::FourBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::FourBit;
};

// �˸�λ�����һ����
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::EightBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::EightBit;
};
// ʮ����λ�����һ����
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::TenSixBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::SixteenBit;
};
// ��ʮ����λ���Ӧһ����
template <SectionWidth DEFAULT>
struct TryAlignBWMapSW<BitWidth::ThreetwoBit, DEFAULT>
{
	constexpr static SectionWidth value = SectionWidth::ThirtytwoBit;
};
// ��ʮ�ĸ�λ���Ӧһ����
//template <SectionWidth DEFAULT>
//struct TryAlignBWMapSW<BitWidth::SixfourBit, DEFAULT>
//{
//	constexpr static SectionWidth value = SectionWidth::SixtyfourBit;
//};

//��� v
template <BitWidth BWIDTH, SectionWidth DEFAULT = SectionWidth::PerBit>
constexpr SectionWidth TryAlignBWMapSW_v = TryAlignBWMapSW<BWIDTH, DEFAULT>::value;


// ���������λ��� ��Ӧ��ֵ���ͣ����������
template <BitWidth BWIDTH, typename VALUE_TYPE>
struct EventModelValueCount
{
private:
	constexpr static std::size_t bit_count = (static_cast<int>(BWIDTH) / (sizeof(VALUE_TYPE) * 8));
public:
	constexpr static std::size_t value = bit_count == 0 ? 1 : bit_count;
};

// ��� v
template <BitWidth BWIDTH, typename VALUE_TYPE>
constexpr std::size_t EventModelValueCount_v = EventModelValueCount<BWIDTH, VALUE_TYPE>::value;

// ��� SectionWidth �Ƿ�С�� BitWIdth
template <BitWidth BWIDTH,SectionWidth BSWIDTH>
struct IsSecWidthLessBitWidth
{
	constexpr static bool value = static_cast<int>(BSWIDTH) <= static_cast<int>(BWIDTH) ? true : false;
};
// ��� v
template <BitWidth BWIDTH, SectionWidth BSWIDTH>
constexpr bool IsSecWidthLessBitWidth_v = IsSecWidthLessBitWidth<BWIDTH, BSWIDTH>::value;



// �����Ӧ���͵�λ����
template <typename T>
struct BitCountOfValueType;

// ��� v
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
// ��� v
template <BitWidth BWIDTH, SectionWidth BSWIDTH>
constexpr std::size_t EventModelMaxElement_v = EventModelMaxElement<BWIDTH, BSWIDTH>::value;
#endif // !__FWD_TOOLS_HPP__
