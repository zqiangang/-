#ifndef __PAGE_TOOLS_HPP__
#define __PAGE_TOOLS_HPP__
#include "EventModelManage.hpp"
#include <functional>

// 奇异递归模板
template <typename MANAGE,typename DERVIED_TYPE>
class CRTP
{
protected:
	DERVIED_TYPE * derived()
	{
		return static_cast<DERVIED_TYPE*>(this);
	}

	DERVIED_TYPE const * derived()const
	{
		return static_cast<DERVIED_TYPE const*>(this);
	}
public:
	// 使用移动语义合并其他 k 树
	using UNIQUE_FREQ_TREE_PTR = std::unique_ptr<typename MANAGE::FREQ_TREE_TYPE[], std::default_delete<typename MANAGE::FREQ_TREE_TYPE[]>>;
	using PAIR_COUNT_FREQ_LIST_TYPE = std::pair<std::size_t, UNIQUE_FREQ_TREE_PTR>;
	using SECTION_TYPE = typename MANAGE::MASK_TYPE;
};

// 当 一个位对应一个 eventModel 时的功能函数
template <typename MANAGE, 
	template<typename, typename = std::enable_if_t<
		std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE, typename MANAGE::INDEX_TYPE, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
	class DERVIED_TYPE>
class PerBitFreqTree : public CRTP<MANAGE,DERVIED_TYPE<MANAGE>>
{
public:
	
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees()
	{

	}
};

// 当多个位 对应一个eventModel 时的功能函数
template <typename MANAGE,
	template<typename, typename = std::enable_if_t<
		std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE, typename MANAGE::INDEX_TYPE, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
	class DERVIED_TYPE>
class MultiBitFreqTree : public CRTP<MANAGE,DERVIED_TYPE<MANAGE>>
{
public:
	// 普通方法
	template <bool = static_cast<std::size_t>(<MANAGE::BIW_WIDTH) == 1>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees()
	{

	}


	template <>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees<false>()
	{

	}

	virtual void SectionCal()	//sec 处理方式
	{}
	virtual void MultiRowCal() // row 交互 处理
	{}
	virtual void RowCal()	// row 单行处理方式
	{}
};



#endif // !__PAGE_TOOLS_HPP__
