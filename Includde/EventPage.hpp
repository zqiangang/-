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
	using FREQ_TREE_TYPE = FrequentTree<MANAGE>;			// Ƶ��������
	using UNIQUE_ROW_TYPE = std::unique_ptr<VALUE_TYPE[], std::default_delete<VALUE_TYPE[]>>;
	using UNIQUE_PAGE_TYPE = std::unique_ptr<UNIQUE_ROW_TYPE[],std::default_delete<UNIQUE_ROW_TYPE[]>>; 
	using UNIQUE_FREQ_TREE_PTR = std::unique_ptr<FREQ_TREE_TYPE[], std::default_delete<FREQ_TREE_TYPE[]>>;
	using PAIR_COUNT_FREQ_LIST_TYPE = std::pair<std::size_t, UNIQUE_FREQ_TREE_PTR>;
	using SECTION_TYPE = typename MANAGE::MASK_TYPE;



	// ����ֵ	: �޷���ֵ
	// ����		: ��ǰ page �ܹ��洢
	// �������� : ���캯��
	EventPage(std::size_t const& _set_count)											// ���캯��
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
			page[i].reset(new VALUE_TYPE[static_cast<int>(MANAGE::BIT_WIDTH) * set_count / BitCountOfValueType_v<VALUE_TYPE>]{ 0 });				// bit width ���ܴ��� value _type ����  bitWidth * _set_count / sizeof_bit(value_type)
		}
	}

	// ����ֵ	: �޷���ֵ
	// ����		: �޲���
	// �������� : DEBUG ���� 
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
	
	// bit section < 8 ��ϴ洢
	// ����ֵ	: �޷���ֵ
	// ����		: �޲���
	// �������� : ��ӡ��ʾ-��ǰpage �е�����Ԫ��
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
					for (int x = 0; x < set_count; ++x)					// �����
						std::cout << std::setw(3 * MANAGE::EVENT_MODEL_MAX_ELEMENTS) << x;
					std::cout << std::endl;
				}
				if (flag)												// �����
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
	



	// bit section >= 8 �����洢
	// ����ֵ	: �޷���ֵ
	// ����		: �޲���
	// �������� : ��ӡ��ʾ-��ǰpage �е�����Ԫ��
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


	// BIT WIDTH С��8 �ǿ���
	// ����ֵ	: ָʾ��ǰ page �Ƿ��Ѿ�׷������true ��ʾ��ǰ��������,false ��ǰ����δ��
	// ����		: Ҫ׷�ӵ���ǰ page ���¼���
	// �������� : �� ems ׷�ӵ���ǰ page ��,����ֵָʾ׷���Ƿ�ɹ�/�����Ƿ�����
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

	
	// BIT WIDTH >= 8 ʱ������
	// ����ֵ	: ָʾ��ǰ page �Ƿ��Ѿ�׷������true ��ʾ��ǰ��������,false ��ǰ����δ��
	// ����		: Ҫ׷�ӵ���ǰ page ���¼���
	// �������� : �� ems ׷�ӵ���ǰ page ��,����ֵָʾ׷���Ƿ�ɹ�/�����Ƿ�����
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


	// BIT WIDTH С��8 �ǿ���
	// ����ֵ	: ָʾ��ǰ page �Ƿ��Ѿ�׷������true ��ʾ��ǰ��������,false ��ǰ����δ��
	// ����		: emss Ҫ׷�ӵ���ǰ page ���¼����б�,count �¼����б��а������ٸ��¼���
	// �������� : �� ems ׷�ӵ���ǰ page ��,����ֵָʾ׷���Ƿ�ɹ�/�����Ƿ�����
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

	// BIT WIDTH ����8 �ǿ���
	// ����ֵ	: ָʾ��ǰ page �Ƿ��Ѿ�׷������true ��ʾ��ǰ��������,false ��ǰ����δ��
	// ����		: emss Ҫ׷�ӵ���ǰ page ���¼����б�,count �¼����б��а������ٸ��¼���
	// �������� : �� ems ׷�ӵ���ǰ page ��,����ֵָʾ׷���Ƿ�ɹ�/�����Ƿ�����
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

	
	// ����ֵ	: �Ƿ�ɹ�����
	// ����		: �޲���
	// �������� : ��page �����ǣ����Գɹ�����������,�����
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

	// ����ֵ	: �޷���ֵ
	// ����		: �µ��������� 
	// �������� : ��ʼ�������ǰ����������������ָ����С�Ŀռ�
	void InitClear(std::size_t const& _set_count)
	{
		page.reset(new UNIQUE_ROW_TYPE[event_count]);
		for (int i = 0; i < event_count; ++i)
		{
			page[i].reset(new VALUE_TYPE[static_cast<int>(MANAGE::BIT_WIDTH) * set_count / BitCountOfValueType_v<VALUE_TYPE>]{ 0 });				// bit width ���ܴ��� value _type ����  bitWidth * _set_count / sizeof_bit(value_type)
		}

		event_count = InstanceManage<MANAGE>()->GetEventTypeCount();
		set_count = _set_count;
		count_vems = 0;
		count_tvalue_offset(0);
		count_evalue_offset(0);
	}

	// ����ֵ	: ��ǰ page ���ܴ洢�� EventModelSet ������
	// ����		: �޲���
	// �������� : ��ȡ��ǰ�������ܴ洢�� EventModelSet ������
	std::size_t EMSetCount()const
	{
		return set_count;
	}

	// ����Ƶ�������ڵ��б�
	// pair.first ��ǰ�洢�� n ������
	// pair.second �洢�ڵ�ĵ�ַָ��
	// ��ͨ����
	template <bool = static_cast<std::size_t>(MANAGE::BIT_WIDTH) == 1>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees(float min_sup)
	{
		std::vector<PAIR_COUNT_FREQ_LIST_TYPE> tmp;
		PAIR_COUNT_FREQ_LIST_TYPE tmp_kn;
		tmp_kn.first = InstanceManage<MANAGE>()->GetEventTypeCount();
		tmp_kn.second.reset(new FrequentTree<MANAGE>[1]);
		tmp_kn.second[0].ReLevel(1);
		int kn = 1;
		// ѭ������
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
			//   ������  DEBUG  UNDO  fixed
			tmp.push_back(std::move(CheckKTreeOBW(tmp_kn, min_sup)));

			// ����Ƿ�Ϊ�ջ�ǰ�� Ϊ��ֱ�ӽ���
			// DEBUG
			//std::cout << tmp.back().first << std::endl;
			if (0 == tmp.back().first)
			{
				break;
			}

			//std::cout << "K" << kn << " : " << tmp.back().first << std::endl;
			++kn;
			//if(tmp.back().first)

			// ���� UniquePool ֮ǰ�����
			tmp.back().second[0].ClearUniquePool();
			// ����һ�μ���������������� unique_pool ����
			for (int i = 0; i < tmp.back().first; ++i)
			{
				tmp.back().second[i].UniquePool();
			}

			if (tmp.back().second[0].idx_pool_count < kn)		// Ԫ���������㼰�����ٴε���ϲ��ʱ������
				break;

			//GroupSplit()
			// ���� k n ��ȫƵ������
			tmp_kn = GroupSplit<MANAGE, PAIR_COUNT_FREQ_LIST_TYPE,INDEX_TYPE>(FrequentTree<MANAGE>::unique_idx_pool.get(), FrequentTree<MANAGE>::idx_pool_count,kn);
		}



		// ���췵��ֵ
		PAIR_COUNT_FREQ_LIST_TYPE ret;
		int pair_count = 0;
		// ��ʼ�� first
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

	// �ػ�����
	template <>
	PAIR_COUNT_FREQ_LIST_TYPE FreqTrees<false>(float min_sup)
	{
		std::cout << "�÷���δʵ��" << std::endl;
		PAIR_COUNT_FREQ_LIST_TYPE ret;
		ret.first = 0;
		ret.second.reset(nullptr);
		return ret;
	}

	virtual void SectionCal()	//sec ����ʽ
	{}
	virtual void MultiRowCal() // row ���� ����
	{}
	virtual void RowCal()	// row ���д���ʽ
	{}

	

	//VALUE_TYPE const & GetTValue(std::size_t row,bool & end_stat)const;

	//VALUE_TYPE GetSectionValue(std::size_t row,bool & end_stat)const;
	void RowSum(PAIR_COUNT_FREQ_LIST_TYPE & pair_idx_list)
	{
		for (int i = 0; i < pair_idx_list.first; ++i)
		{
			VALUE_TYPE tmp;
			std::size_t row_sum = 0;
			for (int t = 0; t < set_count / BitCountOfValueType_v<VALUE_TYPE>; ++t)			// �¼��е�ÿһ�� tvalue
			{
				tmp = page[pair_idx_list.second[i].Get(0)][t];
				for (int idx = 1; idx < pair_idx_list.second[i].GetLevel(); ++idx)			// pair �д洢����
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
		ret.second.reset(new FREQ_TREE_TYPE[pair_idx_list.first]);				// ����һ���ȴ�С�Ķ��������ڴ�ŷ���������Ƶ������
		std::size_t freq_tree_count = 0;
		
		// ret.second->reset Ĭ�Ϲ��첻����ռ䣬���ü�����
		// pair.first = MANAGE.GetEventTypeCount() // ���� k1 ���� 
		float SUP = min_sup * set_count;
		for (int i = 0; i < pair_idx_list.first; ++i)												// 
		{
			if (pair_idx_list.second[0].GetLevel() == 1)										// k1 ȫ��������
			{
				std::size_t row_sum = 0;
				for (int t = 0; t < set_count / BitCountOfValueType_v<VALUE_TYPE>; ++t)				// �¼��е�ÿһ�� tvalue
				{
					row_sum += BitOneCount<VALUE_TYPE>(page[i][t]);
				}

				if (row_sum >= SUP)
				{
					++ret.first;
					ret.second[freq_tree_count].ReLevel(pair_idx_list.second[0].GetLevel());		// ����ռ�
					ret.second[freq_tree_count].Push(i);											// ��������Ԫ��
					ret.second[freq_tree_count].SetFreqSum(row_sum);								// ���� row sum
					++freq_tree_count;																// ָ����һ��λ��
				}
				row_sum = 0;
			}
			else
			{
				VALUE_TYPE tmp;
				std::size_t row_sum= 0;
				for (int t = 0; t < set_count / BitCountOfValueType_v<VALUE_TYPE>; ++t)			// �¼��е�ÿһ�� tvalue
				{
					tmp = page[pair_idx_list.second[i].Get(0)][t];
					for (int idx = 1; idx < pair_idx_list.second[i].GetLevel(); ++idx)			// pair �д洢����
					{
						tmp &= page[pair_idx_list.second[i].Get(idx)][t];
					}
					row_sum += BitOneCount<VALUE_TYPE>(tmp);
				}

				if (row_sum >= SUP)
				{
					++ret.first;
					ret.second[freq_tree_count].ReLevel(pair_idx_list.second[i].GetLevel());		// ����ռ�
					for(int r = 0; r < pair_idx_list.second[i].GetLevel(); ++r)
						ret.second[freq_tree_count].Push(pair_idx_list.second[i].Get(r));			// ��������Ԫ��
					ret.second[freq_tree_count].SetFreqSum(row_sum);								// ���� row sum
					++freq_tree_count;																// ָ����һ��λ��
				}

				row_sum = 0;
			}
		}

		return std::move(ret);
	}

private:
	UNIQUE_PAGE_TYPE page;
	std::size_t event_count;								// page ���Դ洢������¼���
	std::size_t set_count;									// page ���Դ洢���¼�������	
	std::size_t count_vems;									// page �Ѿ��洢���¼�������
	std::size_t count_tvalue_offset;						// page �� tvalue �Ĵ洢����
	std::size_t count_evalue_offset;						// page �� tavlue �� evalue ��ƫ��λ��
};

#endif // !__EVENT_PAGE_HPP__
