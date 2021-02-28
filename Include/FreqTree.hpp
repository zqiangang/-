#ifndef __FREQUENT_TREE_HPP__
#define __FREQUENT_TREE_HPP__
#include "CPPheader.hpp"
#include "baseTools.hpp"

template <typename MANAGE>
class FrequentTree
{
public:
	using INDEX_TYPE = typename MANAGE::INDEX_TYPE;
	using UNIQUE_INDEX_PTR = std::unique_ptr<INDEX_TYPE[], std::default_delete<INDEX_TYPE[]>>;

	FrequentTree();	

	FrequentTree(int level);
																												// 默认构造函数
	FrequentTree & operator=(FrequentTree const & rhs);															// 赋值构造函数

	void ReLevel(std::size_t level);																									// 重置 level

	std::size_t GetLevel()const;																										// 获取 freeq_tree 的 k level																										// 构造函数

	void Print(bool has_sum = false, bool line_feed = false, bool comma_split = true, char wrap_left = '{', char wrap_right = '}', bool space = false);		// 打印显示频繁子树

	void Push(INDEX_TYPE idx);																											// 推入子树

	INDEX_TYPE Get(std::size_t N)const;

	void SetFreqSum(std::size_t _sum);

	std::size_t GetFreqSum()const;

	void UniquePool();

	void ClearUniquePool();

public:
	static UNIQUE_INDEX_PTR unique_idx_pool;
	static std::size_t idx_pool_count;
private:
	std::size_t freq_level;
	UNIQUE_INDEX_PTR index_list;
	std::size_t freq_sum;
	std::size_t index_count;
};

template <typename MANAGE>
typename FrequentTree<MANAGE>::UNIQUE_INDEX_PTR FrequentTree<MANAGE>::unique_idx_pool(nullptr);

template <typename MANAGE>
std::size_t FrequentTree<MANAGE>::idx_pool_count = 0;

template <typename MANAGE>
FrequentTree<MANAGE>::FrequentTree()
	:freq_level(0), index_list(nullptr), index_count(0), freq_sum(0)
{
	if (unique_idx_pool == nullptr)
		unique_idx_pool.reset(new typename FrequentTree<MANAGE>::INDEX_TYPE[InstanceManage<MANAGE>()->GetEventTypeCount()]);
}


template <typename MANAGE>
FrequentTree<MANAGE> & FrequentTree<MANAGE>::operator=(FrequentTree const & rhs)						// 赋值构造函数
{
	ReLevel(rhs.GetLevel());
	freq_sum = rhs.freq_sum;
	for (int i = 0; i < freq_level; ++i)
		index_list[i] = rhs.index_list[i];
	index_count = rhs.index_count;
	return *this;	
}

template <typename MANAGE>
FrequentTree<MANAGE>::FrequentTree(int level)
	: freq_level(level), index_list(new INDEX_TYPE[level]), index_count(0), freq_sum(0)
{
	if (unique_idx_pool == nullptr)
		unique_idx_pool.reset(new typename FrequentTree<MANAGE>::INDEX_TYPE[InstanceManage<MANAGE>()->GetEventTypeCount()]);
}



template <typename MANAGE>
void FrequentTree<MANAGE>::ReLevel(std::size_t level)
{
	//std::cout << "DEBUG LEVEL : " << level << std::endl;
	freq_level = level;
	index_count = 0;
	index_list.reset(new INDEX_TYPE[freq_level]);
}

template <typename MANAGE>
void FrequentTree<MANAGE>::Print(bool has_sum,bool line_feed, bool comma_split, char wrap_left, char wrap_right, bool space)
{
	char const * _space = "";
	char const * _comma = "";
	if (comma_split)
		_comma = ",";
	if (space)
		_space = " ";

	std::cout << wrap_left << _space;
	for (int i = 0; i < freq_level; ++i)
	{
		std::cout << index_list[i];
		if (i != freq_level - 1)
			std::cout << _comma;
	}

	if (has_sum)
		std::cout << _comma << "sum : " << freq_sum;

	std::cout << wrap_right << _space;

	if (line_feed)
		std::cout << std::endl;
}

template <typename MANAGE>
void FrequentTree<MANAGE>::Push(INDEX_TYPE idx)
{
	index_list[index_count] = idx;
	++index_count;
}


template <typename MANAGE>
std::size_t FrequentTree<MANAGE>::GetLevel()const
{
	return this->freq_level;
}


template <typename MANAGE>
void FrequentTree<MANAGE>::SetFreqSum(std::size_t _sum)
{
	this->freq_sum = _sum;
}

// 
template <typename MANAGE>
std::size_t FrequentTree<MANAGE>::GetFreqSum()const
{
	return this->freq_sum;
}

template <typename MANAGE>
typename FrequentTree<MANAGE>::INDEX_TYPE FrequentTree<MANAGE>::Get(std::size_t N)const
{
	return index_list[N];
}


template <typename MANAGE>
void FrequentTree<MANAGE>::UniquePool()
{
	//if (freq_level == 1)
	//	return;

	if (index_count == 0)
		return;
	
	for (int i = 0; i < index_count; ++i)
	{
		bool is_exist = false;
		for (int j = 0; j < idx_pool_count; ++j)					// 检查是否存在
		{
			if (unique_idx_pool[j] == index_list[i])
			{
				is_exist = true;
				break;
			}
		}
		if (!is_exist)
		{
			unique_idx_pool[idx_pool_count] = index_list[i];
			++idx_pool_count;
		}
	}
}



template <typename MANAGE>
void FrequentTree<MANAGE>::ClearUniquePool()
{
	idx_pool_count = 0;
}

#endif // !__FREQ_TREE_HPP__
