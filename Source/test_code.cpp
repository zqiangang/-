#ifndef __EVENT_MODEL_HPP__
#define __EVENT_MODEL_HPP__
#include "CPPheader.hpp"
#include "baseTools.hpp"

#define DISABLE_THIS_FILE
#ifndef DISABLE_THIS_FILE // �ر� DEBUG

enum BitWidth :char
{
	OneBit = 1,
	TwoBit = 2,
	FourBit = 4,
	EightBit = 8,
	SixteenBit = 16,
	ThirtytwoBit = 32
};

enum SectionWidth
{
	PerBit,
	PerBit
};


//template <typename TKEY, BitWidth BWIDTH = BitWidth::OneBit, SectionWidth BSWidth = SectionWidth::PerBit>
//class EventModelManage;


template <typename TVALUE>
struct EventModel
{
	uint32_t index;
	TVALUE value;

	using VALUE_TYPE = TVALUE;

	EventModel(uint32_t const & _idx, TVALUE const& _value)
		:index(_idx), value(_value)
	{}

	EventModel() {}
};



template <typename MANAGE, typename TVALUE, typename TKEY = typename MANAGE::KEY_TYPE,
	typename = std::enable_if<std::is_base_of_v<EventModelManage<TKEY, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
	class EventModelSet
{
public:
	using VALUE_TYPE = EventModel<TVALUE>;
	using BASE_MANAGE = EventModelManage<TKEY, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>;
	using SPEC_MANAGE = MANAGE;
	using SHARED_MANAGE = std::shared_ptr<BASE_MANAGE>;
	using UNQIUE_VTYPE = std::unique_ptr<VALUE_TYPE[]>;

public:
	EventModelSet(std::size_t _event_count)
		: event_buf(new VALUE_TYPE[_event_count])
		, cur_event_count(0)
		, capacity(_event_count)
		, event_manage(InstanceManage<SPEC_MANAGE>())
	{}

	void Push(TVALUE const & _value, TKEY const & _key)
	{
		if (cur_event_count >= capacity)
			return;
		event_buf[cur_event_count++] = VALUE_TYPE(event_manage->GetIndexFromKey(_key), _value);
	}

	template <typename = std::enable_if_t<!std::is_same_v<TVALUE, TKEY>>>
	void Push(TKEY const & _key, TVALUE const & _value)
	{
		if (cur_event_count >= capacity)
			return;
		push(_value, _key);
	}

	VALUE_TYPE const & Get(int N)const
	{
		if (N >= capacity)
			throw std::out_of_range("�ڴ�Խ�� Ret Get��int N��const, ");
		return event_buf[N];
	}

	std::size_t const Capacity()const
	{
		return capacity;
	}

	~EventModelSet()
	{

	}

private:
	UNQIUE_VTYPE event_buf;
	std::size_t cur_event_count, capacity;
	SHARED_MANAGE event_manage;
};


template <typename MANAGE, typename TVALUE, typename TKEY = typename MANAGE::KEY_TYPE,
	typename = std::enable_if<std::is_base_of_v<EventModelManage<TKEY, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
	class EventModelSet
{
public:
	using VALUE_TYPE = EventModel<TVALUE>;
	using BASE_MANAGE = EventModelManage<TKEY, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>;
	using SPEC_MANAGE = MANAGE;
	using SHARED_MANAGE = std::shared_ptr<BASE_MANAGE>;
	using UNQIUE_VTYPE = std::unique_ptr<VALUE_TYPE[]>;

public:
	EventModelSet(std::size_t _event_count)
		: event_buf(new VALUE_TYPE[_event_count])
		, cur_event_count(0)
		, capacity(_event_count)
		, event_manage(InstanceManage<SPEC_MANAGE>())
	{}

	void Push(TVALUE const & _value, TKEY const & _key)
	{
		if (cur_event_count >= capacity)
			return;
		event_buf[cur_event_count++] = VALUE_TYPE(event_manage->GetIndexFromKey(_key), _value);
	}

	template <typename = std::enable_if_t<!std::is_same_v<TVALUE, TKEY>>>
	void Push(TKEY const & _key, TVALUE const & _value)
	{
		if (cur_event_count >= capacity)
			return;
		push(_value, _key);
	}

	VALUE_TYPE const & Get(int N)const
	{
		if (N >= capacity)
			return -1;
		return event_buf[N];
	}

	std::size_t const Capacity()const
	{
		return capacity;
	}

	~EventModelSet()
	{

	}

private:
	UNQIUE_VTYPE event_buf;
	std::size_t cur_event_count, capacity;
	SHARED_MANAGE event_manage;
};


template <typename TKEY, BitWidth BWIDTH = BitWidth::OneBit, SectionWidth BSWidth = SectionWidth::PerBit>
class EventModelManage
{
public:
	using KEY_TYPE = TKEY;
	using MASK_TYPE = MaskType_t<BIT_WIDTH>;
	constexpr static BitWidth BIT_WIDTH = BWIDTH;														// λ��
	constexpr static SectionWidth BIT_SECTION_WIDTH = BSWidth;														// ���ݵİڷŷ�ʽ
	constexpr static MASK_TYPE BIT_MASK = MaskValue_v<MaskType_t<BIT_WIDTH>, BIT_WIDTH>;	// ����λ�ĳ���ѡ����������
public:
	EventModelManage() = default;
	virtual std::size_t const GetEventTypeCount() const = 0;							// �������������п��ܳ��ֵ��¼���������
	virtual uint32_t const GetIndexFromKey(KEY_TYPE const& keyCode) const = 0;		// ͨ�����ݱ����ȡ���ݵ����� 
};

#endif // !1



#endif // !DISABLE_THIS_FILE
