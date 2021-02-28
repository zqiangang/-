#ifndef __EVENT_MODEL_HPP__
#define __EVENT_MODEL_HPP__
#include "CPPheader.hpp"
#include "baseTools.hpp"



#ifdef EVENT_MODEL_DEBUG

// TVALUE	EventModel 中值的类型，一般该类型与 BitWidth 相关
// TINDEX	EventModel 中存储的索引值
template <typename MANAGE,
	typename CHECK>
class EventModel
{
private:
	std::size_t count_tvalue;																// 已经存储的 TVALUE 类型的数量
	std::size_t count_vsection;																// 当下正在存储的 TVALUE 的 section 偏移
	std::size_t count_velement;																// 已经存储了的 section 元素个数
public:
	using INDEX_TYPE = typename MANAGE::INDEX_TYPE;											// 索引类型
	using VALUE_TYPE = typename MANAGE::VALUE_TYPE;											// 存储值得类型

	INDEX_TYPE index;																		// 存储 索引
	VALUE_TYPE value[MANAGE::EVENT_MODEL_TVALUE_COUNT]{0};									// 存储 数值


	void MemoryStyle()const;

	EventModel();

	EventModel(EventModel const & rhs);

	EventModel & operator=(EventModel const & rhs);						

	void Clear();

	void SetIndex(INDEX_TYPE const & _index);

	bool const Append(VALUE_TYPE const & _value);

	bool const Empty()const;
};

// 返回值	: 无返回值
// 参数		: 无参数
// 功能描述 : 默认构造函数
template <typename MANAGE, typename CHECK>
EventModel<MANAGE, CHECK>::EventModel()
	:count_tvalue(0), count_vsection(0), count_velement(0), index(0)
{}

// 返回值	: 无返回值
// 参数		: 无参数
// 功能描述 : 打印自身的内部状态
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


// 返回值	: 无返回值
// 参数		: 被拷贝的对象
// 功能描述 : 拷贝构造函数
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

// 返回值	: 自身对象的引用
// 参数		: 被拷贝的对象
// 功能描述 : 复制被拷贝对象的内容并更新自身
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

// 返回值	: 无返回值
// 参数		: 无参数
// 功能描述	: 重置当前容器对象到初始构造状态
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


// 返回值	: 无返回值
// 参数		: _index 当前容器存储的值在 page 中，对应的索引位置
// 功能描述	: 设置索引值
template <typename MANAGE,typename CHECK>
void EventModel<MANAGE, CHECK>::SetIndex(INDEX_TYPE const & _index)
{
	this->index = _index;
}

// 返回值	: true 当前容器已经插入为满存储,false 容器未满
// 参数		: _value 将要追加到当前容器的值
// 功能描述 : 把 _value 追加到容器中，如果已满则不进行追加并返回 true,反之进行追加并返回 false.
template <typename MANAGE,typename CHECK>
bool const EventModel<MANAGE, CHECK>::Append(VALUE_TYPE const & _value)
{
	if (MANAGE::EVENT_MODEL_TVALUE_COUNT == count_tvalue)												// 检查是否满，当 count_tvalue 和 VALUE_COUNT 相等时，表明存储满
		return true;																				// 防止多余的追加

	VALUE_TYPE tp = _value & MANAGE::BIT_MASK;														// 掩码位操作 
	//std::cout << "DEBUG : " << (BitCountOfValueType_v<VALUE_TYPE> -static_cast<int>(MANAGE::BIT_SECTION_WIDTH) - count_vsection) << std::endl;
	(tp <<= (BitCountOfValueType_v<VALUE_TYPE> -static_cast<int>(MANAGE::BIT_SECTION_WIDTH) - count_vsection));		// 位移操作

	value[count_tvalue] |= tp;																		// 追加数据
	++count_velement;

	count_vsection += static_cast<int>(MANAGE::BIT_SECTION_WIDTH);									// 指向下一个追加数据段

	if (count_velement == (int)MANAGE::EVENT_MODEL_MAX_ELEMENTS)
	{
		++count_tvalue;																				// 指向下一个  value type 段
	}

	if (BitCountOfValueType_v<VALUE_TYPE> == count_vsection)										// 切换到下一个元素组															// 当 count_vsection 和 mask_type 的位数相等时，表明当前 mask_type 数据段填满
	{
		++count_tvalue;																				// 指向下一个  value type 段
		count_vsection = 0;																			// 重置 count_vsection 段	
	}

	if (MANAGE::EVENT_MODEL_TVALUE_COUNT <= count_tvalue)												// 检查是否满，当 count_tvalue 和 VALUE_COUNT 相等时，表明存储满
		return true;																				// 当存储满时，返回 true

	return false;
}


// 返回值	: true 当前容器为空，false 当前容器不为空
// 参数		: 无参数
// 功能描述	: 返回 bool 值，只是当前对象是否为空
template <typename MANAGE,typename CHECK>
bool const EventModel<MANAGE,CHECK>::Empty()const
{
	return 0 == count_velement;
}

#endif // EVENT_MODEL_DEBUG

#endif // !__EVENT_MODEL_HPP__
