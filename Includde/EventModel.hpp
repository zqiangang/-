#ifndef __EVENT_MODEL_HPP__
#define __EVENT_MODEL_HPP__
#include "CPPheader.hpp"
#include "baseTools.hpp"



#ifdef EVENT_MODEL_DEBUG

// TVALUE	EventModel ��ֵ�����ͣ�һ��������� BitWidth ���
// TINDEX	EventModel �д洢������ֵ
template <typename MANAGE,
	typename CHECK>
class EventModel
{
private:
	std::size_t count_tvalue;																// �Ѿ��洢�� TVALUE ���͵�����
	std::size_t count_vsection;																// �������ڴ洢�� TVALUE �� section ƫ��
	std::size_t count_velement;																// �Ѿ��洢�˵� section Ԫ�ظ���
public:
	using INDEX_TYPE = typename MANAGE::INDEX_TYPE;											// ��������
	using VALUE_TYPE = typename MANAGE::VALUE_TYPE;											// �洢ֵ������

	INDEX_TYPE index;																		// �洢 ����
	VALUE_TYPE value[MANAGE::EVENT_MODEL_TVALUE_COUNT]{0};									// �洢 ��ֵ


	void MemoryStyle()const;

	EventModel();

	EventModel(EventModel const & rhs);

	EventModel & operator=(EventModel const & rhs);						

	void Clear();

	void SetIndex(INDEX_TYPE const & _index);

	bool const Append(VALUE_TYPE const & _value);

	bool const Empty()const;
};

// ����ֵ	: �޷���ֵ
// ����		: �޲���
// �������� : Ĭ�Ϲ��캯��
template <typename MANAGE, typename CHECK>
EventModel<MANAGE, CHECK>::EventModel()
	:count_tvalue(0), count_vsection(0), count_velement(0), index(0)
{}

// ����ֵ	: �޷���ֵ
// ����		: �޲���
// �������� : ��ӡ������ڲ�״̬
template <typename MANAGE, typename CHECK>
void EventModel<MANAGE, CHECK>::MemoryStyle()const
{
	std::cout << "VALUE TYPE : " << typeid(VALUE_TYPE).name() << std::endl;
	std::cout << "INDEX TYPE : " << typeid(INDEX_TYPE).name() << std::endl;
	std::cout << "ARRAY LENGTH : " << MANAGE::EVENT_MODEL_TVALUE_COUNT << std::endl;
	std::cout << "SECTION LENGTH : " << static_cast<int>(MANAGE::BIT_SECTION_WIDTH) << std::endl;
	std::cout << "BITWIDTH LENGTH : " << static_cast<int>(MANAGE::BIT_WIDTH) << std::endl;
	std::cout << "MASK VALUE : " << (int)MANAGE::BIT_MASK << std::endl;
	std::cout << "MAX ELEMENT : " << (int)MANAGE::EVENT_MODEL_MAX_ELEMENTS << std::endl;
}


// ����ֵ	: �޷���ֵ
// ����		: �������Ķ���
// �������� : �������캯��
template <typename MANAGE,typename CHECK>
EventModel<MANAGE, CHECK>::EventModel(EventModel const & rhs)
{
	for (int i = 0; i < MANAGE::EVENT_MODEL_TVALUE_COUNT; ++i)
	{
		this->value[i] = rhs.value[i];
	}
	this->index = rhs.index;
	this->count_vsection = rhs.count_vsection;
	this->count_tvalue = rhs.count_tvalue;
	this->count_velement = rhs.count_velement;
}

// ����ֵ	: ������������
// ����		: �������Ķ���
// �������� : ���Ʊ�������������ݲ���������
template <typename MANAGE,typename CHECK>
EventModel<MANAGE, CHECK> & EventModel<MANAGE, CHECK>::operator=(EventModel const & rhs)
{

	for (int i = 0; i < MANAGE::EVENT_MODEL_TVALUE_COUNT; ++i)
	{
		this->value[i] = rhs.value[i];
	}
	this->index = rhs.index;
	this->count_vsection = rhs.count_vsection;
	this->count_tvalue = rhs.count_tvalue;
	this->count_velement = rhs.count_velement;
	return *this;
}

// ����ֵ	: �޷���ֵ
// ����		: �޲���
// ��������	: ���õ�ǰ�������󵽳�ʼ����״̬
template <typename MANAGE,typename CHECK>
void EventModel<MANAGE, CHECK>::Clear() 
{
	this->index = 0;
	this->count_vsection = 0;
	this->count_tvalue = 0;
	this->count_velement = 0;
	for (int i = 0; i < MANAGE::EVENT_MODEL_TVALUE_COUNT; ++i)
	{
		this->value[i] = 0;
	}
}


// ����ֵ	: �޷���ֵ
// ����		: _index ��ǰ�����洢��ֵ�� page �У���Ӧ������λ��
// ��������	: ��������ֵ
template <typename MANAGE,typename CHECK>
void EventModel<MANAGE, CHECK>::SetIndex(INDEX_TYPE const & _index)
{
	this->index = _index;
}

// ����ֵ	: true ��ǰ�����Ѿ�����Ϊ���洢,false ����δ��
// ����		: _value ��Ҫ׷�ӵ���ǰ������ֵ
// �������� : �� _value ׷�ӵ������У���������򲻽���׷�Ӳ����� true,��֮����׷�Ӳ����� false.
template <typename MANAGE,typename CHECK>
bool const EventModel<MANAGE, CHECK>::Append(VALUE_TYPE const & _value)
{
	if (MANAGE::EVENT_MODEL_TVALUE_COUNT == count_tvalue)												// ����Ƿ������� count_tvalue �� VALUE_COUNT ���ʱ�������洢��
		return true;																				// ��ֹ�����׷��

	VALUE_TYPE tp = _value & MANAGE::BIT_MASK;														// ����λ���� 
	//std::cout << "DEBUG : " << (BitCountOfValueType_v<VALUE_TYPE> -static_cast<int>(MANAGE::BIT_SECTION_WIDTH) - count_vsection) << std::endl;
	(tp <<= (BitCountOfValueType_v<VALUE_TYPE> -static_cast<int>(MANAGE::BIT_SECTION_WIDTH) - count_vsection));		// λ�Ʋ���

	value[count_tvalue] |= tp;																		// ׷������
	++count_velement;

	count_vsection += static_cast<int>(MANAGE::BIT_SECTION_WIDTH);									// ָ����һ��׷�����ݶ�

	if (count_velement == (int)MANAGE::EVENT_MODEL_MAX_ELEMENTS)
	{
		++count_tvalue;																				// ָ����һ��  value type ��
	}

	if (BitCountOfValueType_v<VALUE_TYPE> == count_vsection)										// �л�����һ��Ԫ����															// �� count_vsection �� mask_type ��λ�����ʱ��������ǰ mask_type ���ݶ�����
	{
		++count_tvalue;																				// ָ����һ��  value type ��
		count_vsection = 0;																			// ���� count_vsection ��	
	}

	if (MANAGE::EVENT_MODEL_TVALUE_COUNT <= count_tvalue)												// ����Ƿ������� count_tvalue �� VALUE_COUNT ���ʱ�������洢��
		return true;																				// ���洢��ʱ������ true

	return false;
}


// ����ֵ	: true ��ǰ����Ϊ�գ�false ��ǰ������Ϊ��
// ����		: �޲���
// ��������	: ���� bool ֵ��ֻ�ǵ�ǰ�����Ƿ�Ϊ��
template <typename MANAGE,typename CHECK>
bool const EventModel<MANAGE,CHECK>::Empty()const
{
	return 0 == count_velement;
}

#endif // EVENT_MODEL_DEBUG

#endif // !__EVENT_MODEL_HPP__
