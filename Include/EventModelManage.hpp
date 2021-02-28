  #ifndef __EVENT_MODEL_MANAGE_HPP__
#define __EVENT_MODEL_MANAGE_HPP__
#include "CPPheader.hpp"
#include "enumTools.hpp"
#include "EventModelsFWD.hpp"
#include "FreqTree.hpp"

#ifdef EVENT_MODEL_DEBUG

// TKEY		key ���ͣ�����keyֵ Ҫ���������е�key �и���Ψһ
// TINDEX	�� key ��Ӧ�洢�� index ֵ�� indexֵҪ���ڵ�ǰ eventModel ������ index ����Ψһ
// BWIDTH	λ��ָ������ֵ��һϵ��ֵ�Ĵ���ռ�õĿռ��� ��λ bit,Ĭ�� 1bit �洢�ռ�ռ��
// BSWIDTH	�ڿռ��С,һ�� BWIDTH �п��԰������ BSWIDTH,һ��BWIDTH �����ܵĶ�Ӧһ�� BSWIDTH��
// ��� BSWIDTH ���Ժ� BWIDTH ��Ӧ����ôһ�� BWIDTH �к���һ�� BSWIDTH,��֮һ�� һ�� 256bit�� BWIDTH �����
// 256 �� BSWIDTH,�����û�ָ���� SectionWidth �еĵ����ڳߴ磬����� N = BWIDTH / BSWIDTH
// CHECK ��� BSWIDTH �Ƿ�С�� BWIDTH,С������ͨ��
template <typename TKEY, typename TINDEX, BitWidth BWIDTH, SectionWidth BSWIDTH,typename CHECK>
class EventModelManage
{
public:
	using KEY_TYPE = TKEY;
	using INDEX_TYPE = TINDEX;																		
	using MASK_TYPE = MaskType_t<BSWIDTH>;																// �����ڵĴ����������
	using VALUE_TYPE = MASK_TYPE;																		// Ĭ��ֵ���͵�ͬ����������
	using UNIQUE_INDEX_PTR = std::unique_ptr<INDEX_TYPE[],std::default_delete<INDEX_TYPE>>;

	constexpr static BitWidth BIT_WIDTH = BWIDTH;														// ��λ�� 
	constexpr static SectionWidth BIT_SECTION_WIDTH = BSWIDTH;											// ���ݵĽ�λ��
	constexpr static MASK_TYPE BIT_MASK = MaskValue_v<MaskType_t<BSWIDTH>, BIT_SECTION_WIDTH>;			// ���ݽڵĳ���ѡ�����������
	constexpr static std::size_t EVENT_MODEL_TVALUE_COUNT = EventModelValueCount_v<BWIDTH, VALUE_TYPE>; // EventModel �е�  Tvalue ����
	constexpr static std::size_t EVENT_MODEL_MAX_ELEMENTS = EventModelMaxElement_v<BWIDTH, BSWIDTH>;	// ����Ԫ������
public:
	EventModelManage() = default;
	virtual std::size_t const GetEventTypeCount() const = 0;											// �������������п��ܳ��ֵ��¼���������
	virtual uint32_t const GetIndexFromKey(KEY_TYPE const& keyCode) const = 0;							// ͨ�����ݱ����ȡ���ݵ����� 
	virtual UNIQUE_INDEX_PTR GetIndexList() const = 0;													// ��ȡ�����б�,�������б��˳��Ӧ���� GetIndexFromKey ���ص�����˳��һ��
	virtual ~EventModelManage() { }
}; 

#endif // DEBUG


#endif // !__EVENT_MODEL_MANAGE_HPP__
