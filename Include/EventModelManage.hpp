  #ifndef __EVENT_MODEL_MANAGE_HPP__
#define __EVENT_MODEL_MANAGE_HPP__
#include "CPPheader.hpp"
#include "enumTools.hpp"
#include "EventModelsFWD.hpp"
#include "FreqTree.hpp"

#ifdef EVENT_MODEL_DEBUG

// TKEY		key 类型，对于key值 要求其在所有的key 中各自唯一
// TINDEX	与 key 对应存储的 index 值， index值要求在当前 eventModel 下所有 index 各自唯一
// BWIDTH	位宽，指明单个值或一系列值的存在占用的空间宽度 单位 bit,默认 1bit 存储空间占用
// BSWIDTH	节空间大小,一个 BWIDTH 中可以包含多个 BSWIDTH,一个BWIDTH 尽可能的对应一个 BSWIDTH，
// 如何 BSWIDTH 可以和 BWIDTH 对应，那么一个 BWIDTH 中含有一个 BSWIDTH,反之一个 一个 256bit的 BWIDTH 则包含
// 256 个 BSWIDTH,或由用户指定的 SectionWidth 中的单个节尺寸，计算出 N = BWIDTH / BSWIDTH
// CHECK 检查 BSWIDTH 是否小于 BWIDTH,小于则检查通过
template <typename TKEY, typename TINDEX, BitWidth BWIDTH, SectionWidth BSWIDTH,typename CHECK>
class EventModelManage
{
public:
	using KEY_TYPE = TKEY;
	using INDEX_TYPE = TINDEX;																		
	using MASK_TYPE = MaskType_t<BSWIDTH>;																// 单个节的存放数据类型
	using VALUE_TYPE = MASK_TYPE;																		// 默认值类型等同于掩码类型
	using UNIQUE_INDEX_PTR = std::unique_ptr<INDEX_TYPE[],std::default_delete<INDEX_TYPE>>;

	constexpr static BitWidth BIT_WIDTH = BWIDTH;														// 总位宽 
	constexpr static SectionWidth BIT_SECTION_WIDTH = BSWIDTH;											// 数据的节位宽
	constexpr static MASK_TYPE BIT_MASK = MaskValue_v<MaskType_t<BSWIDTH>, BIT_SECTION_WIDTH>;			// 根据节的长度选择节掩码类型
	constexpr static std::size_t EVENT_MODEL_TVALUE_COUNT = EventModelValueCount_v<BWIDTH, VALUE_TYPE>; // EventModel 中的  Tvalue 个数
	constexpr static std::size_t EVENT_MODEL_MAX_ELEMENTS = EventModelMaxElement_v<BWIDTH, BSWIDTH>;	// 最大的元素数量
public:
	EventModelManage() = default;
	virtual std::size_t const GetEventTypeCount() const = 0;											// 返回事务中所有可能出现的事件的总数量
	virtual uint32_t const GetIndexFromKey(KEY_TYPE const& keyCode) const = 0;							// 通过数据编码获取数据的索引 
	virtual UNIQUE_INDEX_PTR GetIndexList() const = 0;													// 获取索引列表,此索引列表的顺序应当与 GetIndexFromKey 返回的索引顺序一致
	virtual ~EventModelManage() { }
}; 

#endif // DEBUG


#endif // !__EVENT_MODEL_MANAGE_HPP__
