#ifndef __EVENT_MODEL_SET__
#define __EVENT_MODEL_SET__
#include "CPPheader.hpp"
#include "EventModel.hpp"
#include "baseTools.hpp"
#include "EventModelsFWD.hpp"

#ifdef EVENT_MODEL_DEBUG

// MANAGE	�ض��� Manage����
// TVALUE	EventModel �� value ���ͣ�Ĭ���� mask������
// TKEY		ʹ�� key �� Manage �л�ö�Ӧ�� idx ���Թ��� EventModel
// TINDEX	���� EventModel �е����������� Ĭ�� uint32_t
// CHECK	���������ͼ�飬��� Manage �Ƿ�Ϊ EventModelManage ĳ��ʵ�� �������� 
template <typename MANAGE, typename TVALUE, typename TKEY,typename TINDEX,
	typename CHECK>
class EventModelSet
{
public:
	using VALUE_TYPE = EventModel<MANAGE>;														// ��ǰ���������ֵ����
	using BASE_MANAGE = EventModelManage<TKEY, TINDEX, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>;			// Manage �Ļ���
	using SPEC_MANAGE = MANAGE;																			// Manage ��
	using SHARED_MANAGE = std::shared_ptr<BASE_MANAGE>;													// Manage �����ָ��
	using UNQIUE_VTYPE = std::unique_ptr<VALUE_TYPE[],std::default_delete<VALUE_TYPE[]>>;				// �洢���ݵ��ڴ��׵�ַ

public:
	EventModelSet(std::size_t _event_count);															// ���캯��
	bool Append(TVALUE const & _value, TKEY const & _key);												// ��������
	bool Push(EventModel<MANAGE> const &,TKEY const & _key);											// ����EventModel
	VALUE_TYPE const & GetEventModel(int N)const;														// ��ȡ����
	std::size_t const Count()const;																		// ��ȡ��ǰ�洢��������
	std::size_t const Capacity()const;																	// ��ȡ��ǰ����������
	void Clear(std::size_t _event_count);																// �����ǰ��������������ռ�
	~EventModelSet();																					// ��������

private:
	UNQIUE_VTYPE event_buf;
	std::size_t cur_event_count, capacity;
	SHARED_MANAGE event_manage;
};

// ����ֵ	: �޷���ֵ
// ����		: _event_count ��ǰ�����洢�� eventModel �¼�����
// �������� : ���캯��
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,
	typename UNNAME>
EventModelSet<MANAGE,TVALUE,TKEY,TINDEX,UNNAME>::EventModelSet(std::size_t _event_count)				// event_count	��ǰ�����ĳ�ʼ������
	: event_buf(nullptr)															// event_buf �洢��ַ��ָ���ʼ���������ڴ�
	, cur_event_count(0)																				// cur_event_count ���ݻ��ʣ���־λ
	, capacity(0)																						// capacity ��ǰ����������
	, event_manage(InstanceManage<SPEC_MANAGE>())														// event_manage Manage �����ָ��
{
	std::size_t buf_size = _event_count > event_manage->GetEventTypeCount()								// ����һ���������������,���Ϊ MANAGE::GetEventTypeCount() ����ֵ
		? event_manage->GetEventTypeCount() : _event_count;
	event_buf.reset(new VALUE_TYPE[buf_size]);															// �����ڴ�ռ�
	capacity = buf_size;																				// ��������ֵ
}

// ����ֵ	: ��ǰ�����Ƿ�����/�Ƿ�׷�ӳɹ�, true ׷�ӳɹ�/����δ��,false ׷��ʧ��/��������
// ����		: _value Ҫ׷�ӵ�ֵ��_key ��ֵ��Ӧ�� key ֵ
// �������� : ׷��ֵ��������
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
bool EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Append(TVALUE const & _value, TKEY const & _key) // _value ֵ   _key ��, ÿ�� _key Ӧ����"��һ�޶�"��
{
	if (cur_event_count >= capacity)																	// ��������Ƿ�����
		return false;																					// ���� false ����������,�����в���

	if (event_buf[cur_event_count].Append(_value))														// ������׷�ӵ�
	{
		event_buf[cur_event_count].SetIndex(event_manage->GetIndexFromKey(_key));						// �� eventModel �������ǣ������������
		++cur_event_count;	
	}
	return true;																						// ����ɹ�
}


// ����ֵ	: ��ǰ�����Ƿ�����/�Ƿ�׷�ӳɹ�, true ׷�ӳɹ�/����δ��,false ׷��ʧ��/��������
// ����		: ��Ҫ׷�ӵ� eventModel �� ��eventModel ��Ӧ�� key ֵ
// �������� : �� eventModel Ԫ��׷�ӵ�������
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
bool EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Push(EventModel<MANAGE> const & em,TKEY const & _key)
{
	if (cur_event_count >= capacity)																	// ��������Ƿ�����
		return false;

	if (event_buf[cur_event_count].Empty())																// �����ĵ�ǰԪ��Ϊ��
	{
		event_buf[cur_event_count] = em;																// ֱ������Ԫ��
		event_buf[cur_event_count++].SetIndex(event_manage->GetIndexFromKey(_key));						// �������������ƶ� cur_event_count ����һ��
	}
	else
	{
		EventModel<MANAGE> tp(event_buf[cur_event_count]);												// ��֮����ǰ����δ����ʹ��ֵ
		event_buf[cur_event_count] = em;																// �²����ֵ���뵱ǰλ��
		event_buf[cur_event_count].SetIndex(event_manage->GetIndexFromKey(_key));						// ���µ�ǰλ�õ�����
		if (++cur_event_count >= capacity)																// ���� em �������ǰλ�ó����������ޣ���ֱ�ӷ���
			return true;
		event_buf[cur_event_count] = tp;																// ����,��ԭ���� eventModel ���뵱ǰλ�õĺ�һ��λ��	
	}

	return true; 
}


// ����ֵ	: ����λ�� N ���� EventModel
// ����		: λ�� N
// �������� : ��ȡָ��λ�õ� eventModel
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
typename EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::VALUE_TYPE const & 
	EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::GetEventModel(int _N)const						// _N ��ȡ�� N ��Ԫ��
{
	if (_N >= capacity)																					// ��鵱ǰ N �Ƿ�Խ��
		throw std::out_of_range("�ڴ�Խ�� Ret Get��int N��const, ");									// �׳�Խ���쳣
	return event_buf[_N];																				// ���ص� N ��Ԫ��
}	


// ����ֵ	: ���ص�ǰ�����ִ洢��Ԫ������
// ����		: �޲���
// �������� : ��ȡ��ǰ�����ĵ�ǰ�洢Ԫ������
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
std::size_t const EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Count()const									// ���ص�ǰ�����洢��ʵ��Ԫ����
{
	return cur_event_count;
}


// ����ֵ	: ���ص�ǰ����������
// ����		: �޲���
// �������� : ��ȡ��ǰ����������
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
std::size_t const EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Capacity()const								// ���ص�ǰ�����洢������
{
	return capacity;
}


// ����ֵ	: �޷���ֵ
// ����		: �µ�������С
// �������� : ���õ�ǰ����������,����������ռ�Ϊָ���Ĵ�С
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
void EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Clear(std::size_t _event_count)
{
	cur_event_count = 0;																				// ���� cur_event_count
	std::size_t buf_size = _event_count > event_manage->GetEventTypeCount()								// ����һ���������������,���Ϊ MANAGE::GetEventTypeCount() ����ֵ
		? event_manage->GetEventTypeCount() : _event_count;
	event_buf.reset(new VALUE_TYPE[buf_size]);															// ����ָ�� unique_ptr �Զ�����ԭ���Ŀռ䣬����������ָ����С�ռ�
	capacity = buf_size;																				// ����������С
}

// ����ֵ	: �޷���ֵ
// ����		: �޲���
// �������� : ��������
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::~EventModelSet()
{
}

#endif // DEBUG


#endif // !__EVENT_MODEL_SET__
