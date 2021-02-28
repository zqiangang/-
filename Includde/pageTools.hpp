#ifndef __PAGE_TOOLS_HPP__
#define __PAGE_TOOLS_HPP__
#include "EventModelManage.hpp"
#include <functional>

// ����ݹ�ģ��
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
	// ʹ���ƶ�����ϲ����� k ��
	using UNIQUE_FREQ_TREE_PTR = std::unique_ptr<typename MANAGE::FREQ_TREE_TYPE[], std::default_delete<typename MANAGE::FREQ_TREE_TYPE[]>>;
	using PAIR_COUNT_FREQ_LIST_TYPE = std::pair<std::size_t, UNIQUE_FREQ_TREE_PTR>;
	using SECTION_TYPE = typename MANAGE::MASK_TYPE;
};

// �� һ��λ��Ӧһ�� eventModel ʱ�Ĺ��ܺ���
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

// �����λ ��Ӧһ��eventModel ʱ�Ĺ��ܺ���
template <typename MANAGE,
	template<typename, typename = std::enable_if_t<
		std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE, typename MANAGE::INDEX_TYPE, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
	class DERVIED_TYPE>
class MultiBitFreqTree : public CRTP<MANAGE,DERVIED_TYPE<MANAGE>>
{
public:
	// ��ͨ����
	template <bool = static_cast<std::size_t>(<MANAGE::BIW_WIDTH) == 1>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees()
	{

	}


	template <>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees<false>()
	{

	}

	virtual void SectionCal()	//sec ����ʽ
	{}
	virtual void MultiRowCal() // row ���� ����
	{}
	virtual void RowCal()	// row ���д���ʽ
	{}
};



#endif // !__PAGE_TOOLS_HPP__
