#ifndef __SUPER_MARK_EVENT_MODEL_HPP__
#define __SUPER_MARK_EVENT_MODEL_HPP__
#include "EventModelManage.hpp"

// test vec
#include <vector>

class SuperMarkEventModel : public EventModelManage<uint32_t,uint32_t,BitWidth::OneBit,SectionWidth::PerBit>
{
public:
	virtual std::size_t const GetEventTypeCount() const override						// 返回事务中所有可能出现的事件的总数量
	{
		//std::cout << "再来一次 百货超市" << std::endl;
		return 16;
	}
	virtual uint32_t const GetIndexFromKey(KEY_TYPE const & keyCode)const override			// 通过数据编码获取数据的索引
	{
		//std::cout << "再来一次 百货超市 当前商品号 : " << keyCode << std::endl;
		for (int i = 0; i < 16; ++i)
		{
			if (con[i].first == keyCode)
				return i;
		}
	}

	~SuperMarkEventModel()
	{
		std::cout << "超市被析构了" << std::endl;
	}

	SuperMarkEventModel()
		:EventModelManage()
	{
		con.resize(16);
		for (int i = 0; i < 16; ++i)
		{
			con[i] = std::make_pair(i, i);
		}
	}

	virtual UNIQUE_INDEX_PTR GetIndexList()const override
	{
		UNIQUE_INDEX_PTR idx_list(new INDEX_TYPE[GetEventTypeCount()]);
		for (int i = 0; i < GetEventTypeCount(); ++i)
			idx_list[i] = i;
		return std::move(idx_list);
	}

private:
	mutable int idx = 0;
	std::vector<std::pair<KEY_TYPE, INDEX_TYPE>> con;
	//virtual uint32_t const GetIndexFromKey(KEY_TYPE const& keyCode) const = 0;
};

#endif // !__SUPER_MARK_EVENT_MODEL_HPP__
