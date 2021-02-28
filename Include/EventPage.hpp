#ifndef __EVENT_PAGE_HPP__
#define __EVENT_PAGE_HPP__
#include "EventModelManage.hpp"
#include "CPPheader.hpp"
#include "FreqTree.hpp"

template <typename MANAGE,
	typename CHECK = std::enable_if_t<std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE,typename MANAGE::INDEX_TYPE,MANAGE::BIT_WIDTH,MANAGE::BIT_SECTION_WIDTH>,MANAGE>>>
class EventPage
{
public:
	using VALUE_TYPE = typename MANAGE::MASK_TYPE;
	using INDEX_TYPE = typename MANAGE::INDEX_TYPE;
	using FREQ_TREE_TYPE = FrequentTree<MANAGE>;			// 频繁树类型
	using UNIQUE_ROW_TYPE = std::unique_ptr<VALUE_TYPE[], std::default_delete<VALUE_TYPE[]>>;
	using UNIQUE_PAGE_TYPE = std::unique_ptr<UNIQUE_ROW_TYPE[],std::default_delete<UNIQUE_ROW_TYPE[]>>; 
	using UNIQUE_FREQ_TREE_PTR = std::unique_ptr<FREQ_TREE_TYPE[], std::default_delete<FREQ_TREE_TYPE[]>>;
	using PAIR_COUNT_FREQ_LIST_TYPE = std::pair<std::size_t, UNIQUE_FREQ_TREE_PTR>;
	using SECTION_TYPE = typename MANAGE::MASK_TYPE;



	// 返回值	: 无返回值
	// 参数		: 当前 page 能够存储
	// 功能描述 : 构造函数
	EventPage(std::size_t const& _set_count)											// 构造函数
		: page(nullptr)
		, event_count(InstanceManage<MANAGE>()->GetEventTypeCount())
		, set_count(_set_count)
		, count_vems(0)
		, count_tvalue_offset(0)
		, count_evalue_offset(0)
	{
		page.reset(new UNIQUE_ROW_TYPE[event_count]);
		for (int i = 0; i < event_count; ++i)
		{
			page[i].reset(new VALUE_TYPE[static_cast<int>(MANAGE::BIT_WIDTH) * set_count / BitCountOfValueType_v<VALUE_TYPE>]{ 0 });				// bit width 可能大于 value _type 所以  bitWidth * _set_count / sizeof_bit(value_type)
		}
	}

	// 返回值	: 无返回值
	// 参数		: 无参数
	// 功能描述 : DEBUG 函数 
	void DEBUG_TVALUE()
	{
		for (int i = 0; i < event_count; ++i)
		{
			for (int j = 0; j < set_count; ++j)
			{
				std::cout << (int)page[i][j] << " ";
			}std::cout << std::endl;
		}
	}
	
	// bit section < 8 组合存储
	// 返回值	: 无返回值
	// 参数		: 无参数
	// 功能描述 : 打印显示-当前page 中的所有元素
	template <bool = (static_cast<int>(MANAGE::BIT_SECTION_WIDTH) < 8)>
	void ShowDatas()const
	{
		bool flag = true;
		//std::cout << "VALUE TYPE : " << typeid(VALUE_TYPE).name() << std::endl;
		for (int i = 0; i < event_count; ++i)
		{
			int bwoffset = 0;
			int move_offset = BitCountOfValueType_v<typename MANAGE::MASK_TYPE> -static_cast<int>(MANAGE::BIT_SECTION_WIDTH);
			if(i == 0)
				std::cout << std::setw(3) << "\\";
			for (int j = 0; j < set_count; ++j)							// BUG  UNDO
			{
				//std::cout << "set_count : " << set_count << std::endl;
				if (i == 0 && j == 0) {
					for (int x = 0; x < set_count; ++x)					// 列序号
						std::cout << std::setw(3 * MANAGE::EVENT_MODEL_MAX_ELEMENTS) << x;
					std::cout << std::endl;
				}
				if (flag)												// 行序号
				{
					flag = false;
					std::cout << std::setw(3) << i;
				}

				for (int tvoffset = 0; tvoffset < MANAGE::EVENT_MODEL_TVALUE_COUNT; ++tvoffset)	// tvalue count of one event
				{
					//std::cout << MANAGE::EVENT_MODEL_TVALUE_COUNT <<  "  " << tvoffset << std::endl;
					if (move_offset < 0)
					{
						move_offset = BitCountOfValueType_v<typename MANAGE::MASK_TYPE> - static_cast<int>(MANAGE::BIT_SECTION_WIDTH);
					}

					// tvalue's elemtnt count
					int tvelcount = BitCountOfValueType_v<typename MANAGE::MASK_TYPE> / static_cast<typename MANAGE::MASK_TYPE>(MANAGE::BIT_SECTION_WIDTH);
					int maxelcount = tvelcount > MANAGE::EVENT_MODEL_MAX_ELEMENTS ? MANAGE::EVENT_MODEL_MAX_ELEMENTS : tvelcount;

					//std::cout << "BitWidth and " << tvelcount << " BitSection" << std::endl;
					for (int evoffset = 0; evoffset < maxelcount; ++evoffset)
					{
						auto mask_value = MANAGE::BIT_MASK;
						mask_value <<= move_offset;
						int index = bwoffset * MANAGE::EVENT_MODEL_TVALUE_COUNT + tvoffset;
						auto v = (typename MANAGE::MASK_TYPE)page[i][index];							// UNDO  GGG
						mask_value &= v;
						mask_value >>= move_offset;
						mask_value &= MANAGE::BIT_MASK;
						std::cout << std::setw(3) << (int)mask_value;
						move_offset -= static_cast<int>(MANAGE::BIT_SECTION_WIDTH);
						//if (e != ((BitCountOfValueType_v<typename MANAGE::MASK_TYPE>) / (static_cast<typename MANAGE::MASK_TYPE>(MANAGE::BIT_SECTION_WIDTH))))
						//	{
						//		std::cout << ",";
						//	}
					}
				}
				if(move_offset < 0)
					++bwoffset;
				//std::cout << std::setw(3) << (int)page[i][j];
			}
			flag = true;
			std::cout << std::endl;
		}
	}
	



	// bit section >= 8 独立存储
	// 返回值	: 无返回值
	// 参数		: 无参数
	// 功能描述 : 打印显示-当前page 中的所有元素
	template <>
	void ShowDatas<false>()const
	{
		bool flag = true;

		for (int i = 0; i < event_count; ++i)
		{
			if (i == 0)
				std::cout << std::setw(3) << "\\";
			for (int j = 0; j < set_count; j++)							// BUG  UNDO
			{
				if (i == 0 && j == 0) {
					for (int x = 0; x < set_count; ++x)
						std::cout << std::setw(3 * MANAGE::EVENT_MODEL_TVALUE_COUNT) << x;
					std::cout << std::endl;
				}
				if (flag)
				{
					flag = false;
					std::cout << std::setw(3) << i;
				}
				for (int offset = 0; offset < MANAGE::EVENT_MODEL_TVALUE_COUNT; ++offset)
				{
					std::cout << std::setw(3) << (int)page[i][j * MANAGE::EVENT_MODEL_TVALUE_COUNT + offset];
				}

			}
			flag = true;
			std::cout << std::endl;
		}
	}


	// BIT WIDTH 小于8 是可用
	// 返回值	: 指示当前 page 是否已经追加满，true 表示当前容易已满,false 当前容器未满
	// 参数		: 要追加到当前 page 的事件集
	// 功能描述 : 把 ems 追加到当前 page 中,返回值指示追加是否成功/容器是否已满
	template <bool = (static_cast<int>(MANAGE::BIT_WIDTH) < 8)>
	bool Push(EventModelSet<MANAGE> const * ems)
	{
		if (count_vems == set_count)
			return true;

		auto mask_value = MaskValue_v<VALUE_TYPE, static_cast<SectionWidth>(MANAGE::BIT_WIDTH)>;					// ok
		for (int i = 0; i < ems->Count(); ++i)
		{
			if (count_evalue_offset == BitCountOfValueType_v<VALUE_TYPE>)
			{
				count_evalue_offset = 0;
				++count_tvalue_offset;
			}
			auto mask_value = MaskValue_v<VALUE_TYPE, static_cast<SectionWidth>(MANAGE::BIT_WIDTH)>;
			auto& em = ems->GetEventModel(i);
			VALUE_TYPE tp = em.value[0];
			tp >>= count_evalue_offset;
			mask_value <<= (BitCountOfValueType_v<VALUE_TYPE> - static_cast<VALUE_TYPE>(MANAGE::BIT_WIDTH) - count_evalue_offset);
			tp &= mask_value;
			page[em.index][count_tvalue_offset] |= tp;
		}
		//std::cout << std::endl;
		count_evalue_offset += static_cast<VALUE_TYPE>(MANAGE::BIT_WIDTH);
		++count_vems;

		if (count_vems == set_count)
			return true;
		return false;
	}

	
	// BIT WIDTH >= 8 时，可用
	// 返回值	: 指示当前 page 是否已经追加满，true 表示当前容易已满,false 当前容器未满
	// 参数		: 要追加到当前 page 的事件集
	// 功能描述 : 把 ems 追加到当前 page 中,返回值指示追加是否成功/容器是否已满
	template <>
	bool Push<false>(EventModelSet<MANAGE> const * ems)
	{
		if (count_vems == set_count)
			return true;
		for (int i = 0; i < ems->Count(); ++i)
		{
			auto& em = ems->GetEventModel(i);
			for (int j = 0; j < MANAGE::EVENT_MODEL_TVALUE_COUNT; ++j)
			{
				page[em.index][count_vems * MANAGE::EVENT_MODEL_TVALUE_COUNT + count_tvalue_offset] = em.value[count_tvalue_offset];
				++count_tvalue_offset;
			}
			count_tvalue_offset = 0;
		}
		//std::cout << std::endl;
		if (++count_vems == set_count)
			return true;
		return false;
	}


	// BIT WIDTH 小于8 是可用
	// 返回值	: 指示当前 page 是否已经追加满，true 表示当前容易已满,false 当前容器未满
	// 参数		: emss 要追加到当前 page 的事件集列表,count 事件集列表中包含多少个事件集
	// 功能描述 : 把 ems 追加到当前 page 中,返回值指示追加是否成功/容器是否已满
	template <bool = (static_cast<int>(MANAGE::BIT_WIDTH) < 8)>
	bool Push(EventModelSet<MANAGE>  ** const emss,int count)
	{
		for (int i = 0; i < count; ++i)
		{
			bool stat = Push(emss[i]);
			if (stat)
			{
				return true;
			}
		}
		return false;
	}

	// BIT WIDTH 大于8 是可用
	// 返回值	: 指示当前 page 是否已经追加满，true 表示当前容易已满,false 当前容器未满
	// 参数		: emss 要追加到当前 page 的事件集列表,count 事件集列表中包含多少个事件集
	// 功能描述 : 把 ems 追加到当前 page 中,返回值指示追加是否成功/容器是否已满
	template <>
	bool Push<false>(EventModelSet<MANAGE> ** const emss, int count)
	{
		for (int i = 0; i < count; ++i)
		{
			if (Push(emss[i]))
			{
				return true;
			}
		}
		return false;
	}

	
	// 返回值	: 是否成功重置
	// 参数		: 无参数
	// 功能描述 : 当page 已满是，可以成功的重置容器,非清空
	bool Clear()
	{
		if (count_vems < set_count)
			return false;
		else
		{
			count_vems = 0;
			count_tvalue_offset = 0;
			count_evalue_offset = 0;
		}
		return true;
	}

	// 返回值	: 无返回值
	// 参数		: 新的容器容量 
	// 功能描述 : 初始化清除当前容器，并从新申请指定大小的空间
	void InitClear(std::size_t const& _set_count)
	{
		page.reset(new UNIQUE_ROW_TYPE[event_count]);
		for (int i = 0; i < event_count; ++i)
		{
			page[i].reset(new VALUE_TYPE[static_cast<int>(MANAGE::BIT_WIDTH) * set_count / BitCountOfValueType_v<VALUE_TYPE>]{ 0 });				// bit width 可能大于 value _type 所以  bitWidth * _set_count / sizeof_bit(value_type)
		}

		event_count = InstanceManage<MANAGE>()->GetEventTypeCount();
		set_count = _set_count;
		count_vems = 0;
		count_tvalue_offset(0);
		count_evalue_offset(0);
	}

	// 返回值	: 当前 page 所能存储的 EventModelSet 的数量
	// 参数		: 无参数
	// 功能描述 : 获取当前对象所能存储的 EventModelSet 的数量
	std::size_t EMSetCount()const
	{
		return set_count;
	}

	// 返回频繁子树节点列表
	// pair.first 当前存储了 n 个子树
	// pair.second 存储节点的地址指针
	// 普通方法
	template <bool = static_cast<std::size_t>(MANAGE::BIT_WIDTH) == 1>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees(float min_sup)
	{
		std::vector<PAIR_COUNT_FREQ_LIST_TYPE> tmp;
		PAIR_COUNT_FREQ_LIST_TYPE tmp_kn;
		tmp_kn.first = InstanceManage<MANAGE>()->GetEventTypeCount();
		tmp_kn.second.reset(new FrequentTree<MANAGE>[1]);
		tmp_kn.second[0].ReLevel(1);
		int kn = 1;
		// 循环计算
		//std::cout << tmp_kn.first << "KN" << std::endl;
		while (true)
		{
			//std::cout << tmp_kn.first << std::endl;
			if (0 == tmp_kn.first)
				break;

			if (!tmp.empty())
			{
				if (0 == tmp.back().first)
				{
					break;
				}
			}
			//   有问题  DEBUG  UNDO  fixed
			tmp.push_back(std::move(CheckKTreeOBW(tmp_kn, min_sup)));

			// 检查是否为空或当前的 为空直接结束
			// DEBUG
			//std::cout << tmp.back().first << std::endl;
			if (0 == tmp.back().first)
			{
				break;
			}

			//std::cout << "K" << kn << " : " << tmp.back().first << std::endl;
			++kn;
			//if(tmp.back().first)

			// 调用 UniquePool 之前先清空
			tmp.back().second[0].ClearUniquePool();
			// 对上一次计算的所有子树调用 unique_pool 方法
			for (int i = 0; i < tmp.back().first; ++i)
			{
				tmp.back().second[i].UniquePool();
			}

			if (tmp.back().second[0].idx_pool_count < kn)		// 元素数量不足及进行再次的组合拆分时，返回
				break;

			//GroupSplit()
			// 构造 k n 完全频繁子树
			tmp_kn = GroupSplit<MANAGE, PAIR_COUNT_FREQ_LIST_TYPE,INDEX_TYPE>(FrequentTree<MANAGE>::unique_idx_pool.get(), FrequentTree<MANAGE>::idx_pool_count,kn);
		}



		// 构造返回值
		PAIR_COUNT_FREQ_LIST_TYPE ret;
		int pair_count = 0;
		// 初始化 first
		for (int i = 0; i < tmp.size(); ++i)
			ret.first += tmp[i].first;

		ret.second.reset(new FrequentTree<MANAGE>[ret.first]);

		for (int i = 0; i < tmp.size(); ++i)
		{
			for (int j = 0; j < tmp[i].first; ++j)
			{
				ret.second[pair_count].ReLevel(tmp[i].second[j].GetLevel());
				for (int l = 0; l < tmp[i].second[j].GetLevel(); ++l)				// level
				{
					ret.second[pair_count] = tmp[i].second[j];
				}
				++pair_count;
			}
		}

		return std::move(ret);
	}

	// 特化方法
	template <>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees<false>(float min_sup)
	{
		std::cout << "该方法未实现" << std::endl;
		PAIR_COUNT_FREQ_LIST_TYPE ret;
		ret.first = 0;
		ret.second.reset(nullptr);
		return ret;
	}

	virtual void SectionCal()	//sec 处理方式
	{}
	virtual void MultiRowCal() // row 交互 处理
	{}
	virtual void RowCal()	// row 单行处理方式
	{}

	

	//VALUE_TYPE const & GetTValue(std::size_t row,bool & end_stat)const;

	//VALUE_TYPE GetSectionValue(std::size_t row,bool & end_stat)const;
	void RowSum(PAIR_COUNT_FREQ_LIST_TYPE & pair_idx_list)
	{
		for (int i = 0; i < pair_idx_list.first; ++i)
		{
			VALUE_TYPE tmp;
			std::size_t row_sum = 0;
			for (int t = 0; t < set_count / BitCountOfValueType_v<VALUE_TYPE>; ++t)			// 事件行的每一个 tvalue
			{
				tmp = page[pair_idx_list.second[i].Get(0)][t];
				for (int idx = 1; idx < pair_idx_list.second[i].GetLevel(); ++idx)			// pair 中存储的行
				{
					tmp &= page[pair_idx_list.second[i].Get(idx)][t];
				}
				row_sum += BitOneCount<VALUE_TYPE>(tmp);
			}

			pair_idx_list.second[i].SetFreqSum(row_sum);
		}
	}

protected:
	PAIR_COUNT_FREQ_LIST_TYPE CheckKTreeOBW(PAIR_COUNT_FREQ_LIST_TYPE const & pair_idx_list ,float min_sup)
	{
		PAIR_COUNT_FREQ_LIST_TYPE ret;
		ret.first = 0;
		ret.second.reset(new FREQ_TREE_TYPE[pair_idx_list.first]);				// 申请一个等大小的堆数组用于存放符合条件的频繁子树
		std::size_t freq_tree_count = 0;
		
		// ret.second->reset 默认构造不申请空间，即用即申请
		// pair.first = MANAGE.GetEventTypeCount() // 计算 k1 子树 
		float SUP = min_sup * set_count;
		for (int i = 0; i < pair_idx_list.first; ++i)												// 
		{
			if (pair_idx_list.second[0].GetLevel() == 1)										// k1 全遍历计算
			{
				std::size_t row_sum = 0;
				for (int t = 0; t < set_count / BitCountOfValueType_v<VALUE_TYPE>; ++t)				// 事件行的每一个 tvalue
				{
					row_sum += BitOneCount<VALUE_TYPE>(page[i][t]);
				}

				if (row_sum >= SUP)
				{
					++ret.first;
					ret.second[freq_tree_count].ReLevel(pair_idx_list.second[0].GetLevel());		// 申请空间
					ret.second[freq_tree_count].Push(i);											// 推入索引元素
					ret.second[freq_tree_count].SetFreqSum(row_sum);								// 设置 row sum
					++freq_tree_count;																// 指向下一个位置
				}
				row_sum = 0;
			}
			else
			{
				VALUE_TYPE tmp;
				std::size_t row_sum= 0;
				for (int t = 0; t < set_count / BitCountOfValueType_v<VALUE_TYPE>; ++t)			// 事件行的每一个 tvalue
				{
					tmp = page[pair_idx_list.second[i].Get(0)][t];
					for (int idx = 1; idx < pair_idx_list.second[i].GetLevel(); ++idx)			// pair 中存储的行
					{
						tmp &= page[pair_idx_list.second[i].Get(idx)][t];
					}
					row_sum += BitOneCount<VALUE_TYPE>(tmp);
				}

				if (row_sum >= SUP)
				{
					++ret.first;
					ret.second[freq_tree_count].ReLevel(pair_idx_list.second[i].GetLevel());		// 申请空间
					for(int r = 0; r < pair_idx_list.second[i].GetLevel(); ++r)
						ret.second[freq_tree_count].Push(pair_idx_list.second[i].Get(r));			// 推入索引元素
					ret.second[freq_tree_count].SetFreqSum(row_sum);								// 设置 row sum
					++freq_tree_count;																// 指向下一个位置
				}

				row_sum = 0;
			}
		}

		return std::move(ret);
	}

private:
	UNIQUE_PAGE_TYPE page;
	std::size_t event_count;								// page 可以存储的最大事件量
	std::size_t set_count;									// page 可以存储的事件集数量	
	std::size_t count_vems;									// page 已经存储的事件集数量
	std::size_t count_tvalue_offset;						// page 中 tvalue 的存储索引
	std::size_t count_evalue_offset;						// page 的 tavlue 的 evalue 的偏移位置
};

#endif // !__EVENT_PAGE_HPP__
