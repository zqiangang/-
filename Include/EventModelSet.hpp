#ifndef __EVENT_MODEL_SET__
#define __EVENT_MODEL_SET__
#include "CPPheader.hpp"
#include "EventModel.hpp"
#include "baseTools.hpp"
#include "EventModelsFWD.hpp"

#ifdef EVENT_MODEL_DEBUG

// MANAGE	特定的 Manage的类
// TVALUE	EventModel 的 value 类型，默认是 mask的类型
// TKEY		使用 key 从 Manage 中获得对应的 idx 用以构造 EventModel
// TINDEX	构造 EventModel 中的索引的类型 默认 uint32_t
// CHECK	编译器类型检查，检查 Manage 是否为 EventModelManage 某个实例 的派生类 
template <typename MANAGE, typename TVALUE, typename TKEY,typename TINDEX,
	typename CHECK>
class EventModelSet
{
public:
	using VALUE_TYPE = EventModel<MANAGE>;														// 当前容器所需的值类型
	using BASE_MANAGE = EventModelManage<TKEY, TINDEX, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>;			// Manage 的基类
	using SPEC_MANAGE = MANAGE;																			// Manage 类
	using SHARED_MANAGE = std::shared_ptr<BASE_MANAGE>;													// Manage 基类的指针
	using UNQIUE_VTYPE = std::unique_ptr<VALUE_TYPE[],std::default_delete<VALUE_TYPE[]>>;				// 存储数据的内存首地址

public:
	EventModelSet(std::size_t _event_count);															// 构造函数
	bool Append(TVALUE const & _value, TKEY const & _key);												// 推入数据
	bool Push(EventModel<MANAGE> const &,TKEY const & _key);											// 推入EventModel
	VALUE_TYPE const & GetEventModel(int N)const;														// 获取数据
	std::size_t const Count()const;																		// 获取当前存储的数据量
	std::size_t const Capacity()const;																	// 获取当前容器的容量
	void Clear(std::size_t _event_count);																// 清楚当前容器并重新申请空间
	~EventModelSet();																					// 析构函数

private:
	UNQIUE_VTYPE event_buf;
	std::size_t cur_event_count, capacity;
	SHARED_MANAGE event_manage;
};

// 返回值	: 无返回值
// 参数		: _event_count 当前容器存储的 eventModel 事件个数
// 功能描述 : 构造函数
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,
	typename UNNAME>
EventModelSet<MANAGE,TVALUE,TKEY,TINDEX,UNNAME>::EventModelSet(std::size_t _event_count)				// event_count	当前容器的初始化容量
	: event_buf(nullptr)															// event_buf 存储地址的指针初始化和申请内存
	, cur_event_count(0)																				// cur_event_count 数据基质，标志位
	, capacity(0)																						// capacity 当前容器的容量
	, event_manage(InstanceManage<SPEC_MANAGE>())														// event_manage Manage 基类的指针
{
	std::size_t buf_size = _event_count > event_manage->GetEventTypeCount()								// 计算一个合理的容器容量,最大为 MANAGE::GetEventTypeCount() 返回值
		? event_manage->GetEventTypeCount() : _event_count;
	event_buf.reset(new VALUE_TYPE[buf_size]);															// 申请内存空间
	capacity = buf_size;																				// 设置容量值
}

// 返回值	: 当前容器是否已满/是否追加成功, true 追加成功/容器未满,false 追加失败/容器已满
// 参数		: _value 要追加的值，_key 该值对应的 key 值
// 功能描述 : 追加值到容器中
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
bool EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Append(TVALUE const & _value, TKEY const & _key) // _value 值   _key 键, 每个 _key 应当是"独一无二"的
{
	if (cur_event_count >= capacity)																	// 检查容器是否已满
		return false;																					// 返回 false 当容器已满,不进行插入

	if (event_buf[cur_event_count].Append(_value))														// 把数据追加到
	{
		event_buf[cur_event_count].SetIndex(event_manage->GetIndexFromKey(_key));						// 在 eventModel 存放完成是，设置其的索引
		++cur_event_count;	
	}
	return true;																						// 插入成功
}


// 返回值	: 当前容器是否已满/是否追加成功, true 追加成功/容器未满,false 追加失败/容器已满
// 参数		: 将要追加的 eventModel 和 该eventModel 对应的 key 值
// 功能描述 : 把 eventModel 元素追加到容器中
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
bool EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Push(EventModel<MANAGE> const & em,TKEY const & _key)
{
	if (cur_event_count >= capacity)																	// 检查容器是否已满
		return false;

	if (event_buf[cur_event_count].Empty())																// 容器的当前元素为空
	{
		event_buf[cur_event_count] = em;																// 直接设置元素
		event_buf[cur_event_count++].SetIndex(event_manage->GetIndexFromKey(_key));						// 设置索引。并移动 cur_event_count 到下一个
	}
	else
	{
		EventModel<MANAGE> tp(event_buf[cur_event_count]);												// 反之，当前容器未满，使用值
		event_buf[cur_event_count] = em;																// 新插入的值放入当前位置
		event_buf[cur_event_count].SetIndex(event_manage->GetIndexFromKey(_key));						// 更新当前位置的索引
		if (++cur_event_count >= capacity)																// 放入 em 后，如果当前位置超出容量界限，则直接返回
			return true;
		event_buf[cur_event_count] = tp;																// 否则,把原来的 eventModel 放入当前位置的后一个位置	
	}

	return true; 
}


// 返回值	: 返回位置 N 处的 EventModel
// 参数		: 位置 N
// 功能描述 : 获取指定位置的 eventModel
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
typename EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::VALUE_TYPE const & 
	EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::GetEventModel(int _N)const						// _N 获取第 N 个元素
{
	if (_N >= capacity)																					// 检查当前 N 是否越界
		throw std::out_of_range("内存越界 Ret Get（int N）const, ");									// 抛出越界异常
	return event_buf[_N];																				// 返回第 N 个元素
}	


// 返回值	: 返回当前容器现存储的元素数量
// 参数		: 无参数
// 功能描述 : 获取当前容器的当前存储元素数量
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
std::size_t const EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Count()const									// 返回当前容器存储的实际元素数
{
	return cur_event_count;
}


// 返回值	: 返回当前容器的容量
// 参数		: 无参数
// 功能描述 : 获取当前容器的容量
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
std::size_t const EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Capacity()const								// 返回当前容器存储的容量
{
	return capacity;
}


// 返回值	: 无返回值
// 参数		: 新的容量大小
// 功能描述 : 重置当前容器的内容,并重新申请空间为指定的大小
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
void EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::Clear(std::size_t _event_count)
{
	cur_event_count = 0;																				// 重置 cur_event_count
	std::size_t buf_size = _event_count > event_manage->GetEventTypeCount()								// 计算一个合理的容器容量,最大为 MANAGE::GetEventTypeCount() 返回值
		? event_manage->GetEventTypeCount() : _event_count;
	event_buf.reset(new VALUE_TYPE[buf_size]);															// 智能指针 unique_ptr 自动析构原来的空间，并从新申请指定大小空间
	capacity = buf_size;																				// 设置容量大小
}

// 返回值	: 无返回值
// 参数		: 无参数
// 功能描述 : 析构函数
template <typename MANAGE, typename TVALUE, typename TKEY, typename TINDEX,typename UNNAME>
EventModelSet<MANAGE, TVALUE, TKEY, TINDEX, UNNAME>::~EventModelSet()
{
}

#endif // DEBUG


#endif // !__EVENT_MODEL_SET__
