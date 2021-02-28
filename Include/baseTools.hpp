#ifndef __BASE_TOOLS_HPP__
#define __BASE_TOOLS_HPP__
#include "CPPheader.hpp"
#include "EventModelsFWD.hpp"


// 检查是否期望值，默认检查 pWidth 是否为8的整数倍
template <std::size_t pWidth>
struct IsExpectWidth
{
	constexpr static bool value = pWidth % 8 == 0 ? true : false;
};

// 快捷 v
template <std::size_t pWidth>
constexpr bool IsExpectWidth_v = IsExpectWidth<pWidth>::value;

// 是否自动增长
template <typename T>
struct IsAutoIncrement
{
	static constexpr bool value = false;
};

template <>
struct IsAutoIncrement<uint32_t>
{
	static constexpr bool value = true;
};


// 是否支持 ++ 运算符
template <typename T>
struct HasOPlusPlus
{
private:
	template <typename U, typename = decltype(std::declval<U>().operator++(std::declval<int>()))>
	static std::true_type testf(int);

	template <typename U>
	static std::false_type testf(...);

	template <typename U, typename = decltype(std::declval<U>().operator++())>
	static std::true_type testb(int);

	template <typename U>
	static std::false_type testb(...);

public:
	constexpr static bool fvalue = decltype(testf<T>(0))::value;
	constexpr static bool bvalue = decltype(testb<T>(0))::value;
	constexpr static bool value = fvalue && bvalue ? true : false;
};



// 快捷 v
template <typename T>
constexpr bool IsAutoIncrement_v = IsAutoIncrement<T>::value;


// 检查位宽合法性,合法位宽 1 2 4 8 16 32
template <std::size_t bWidth>
struct IsFairBitWidth
{
	constexpr static bool value = bWidth / 2 % 2 == 0 && bWidth <= 32;
};

template <>
struct IsFairBitWidth<1>
{
	constexpr static bool value = true;
};

template <>
struct IsFairBitWidth<2>
{
	constexpr static bool value = true;
};

template <std::size_t bWidth>
constexpr bool IsFairBitWidth_v = IsFairBitWidth<bWidth>::value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////			待办			////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Manage单例函数
// 手动写入 Manage 对象
template <typename MANAGE, typename TKEY = typename MANAGE::KEY_TYPE,typename TINDEX = typename MANAGE::INDEX_TYPE, BitWidth BWIDTH = MANAGE::BIT_WIDTH, SectionWidth BSWidth = MANAGE::BIT_SECTION_WIDTH,
	typename = std::enable_if_t<std::is_base_of<EventModelManage<TKEY, TINDEX, BWIDTH, BSWidth>, MANAGE>::value>>
	std::shared_ptr<EventModelManage<TKEY, TINDEX, BWIDTH, BSWidth>>  InstanceManage(MANAGE * specManage)
{
	static std::shared_ptr<EventModelManage<TKEY, TINDEX, BWIDTH, BSWidth>> instance = nullptr;
	if (nullptr == instance)
	{
		instance.reset(specManage);
	}
	return instance;
}


// 自动写入 Manage 对象，要求Manage 对象存在默认构造函数
template <typename MANAGE, typename TKEY = typename MANAGE::KEY_TYPE, typename TINDEX = typename MANAGE::INDEX_TYPE, BitWidth BWIDTH = MANAGE::BIT_WIDTH, SectionWidth BSWidth = MANAGE::BIT_SECTION_WIDTH
	, typename = std::enable_if_t<std::is_default_constructible_v<MANAGE>>>
	std::shared_ptr<EventModelManage<TKEY, TINDEX, BWIDTH, BSWidth>>  InstanceManage()
{
	auto tp = InstanceManage<MANAGE, TKEY,TINDEX, BWIDTH, BSWidth>(static_cast<MANAGE*>(nullptr));
	if (nullptr == tp)
	{
		return InstanceManage(new MANAGE());
	}
	return tp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 阶乘
inline std::size_t LevelMulti(std::size_t value, std::size_t terminate = 0)
{
	if (value == terminate)
		return 1;
	return value * LevelMulti(value - 1);
}

// 组合数计算
inline std::size_t GroupMath(std::size_t total, std::size_t group_count)
{
	return LevelMulti(total) / (LevelMulti(group_count) * LevelMulti(total - group_count));
}

template <typename VALUE_TYPE>
std::size_t BitOneCount(VALUE_TYPE vt)
{
	int count = 0;
	while (vt)
	{
		vt = vt & (vt - 1);
		++count;
	}
	return count;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
// 有待验证	////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename MANAGE, typename RET, typename INDEX_TYPE>
RET GroupSplit(INDEX_TYPE * ay, std::size_t size, std::size_t group)
{
	// 初始化返回值
	RET ret;
	std::size_t count = 0;									// 存储了对少个元素
	ret.first = GroupMath(size, group);
	ret.second.reset(new FrequentTree<MANAGE>[ret.first]);


	std::size_t *group_point = new std::size_t[group * 2];						// 


	for (int i = 0; i < group * 2; i += 2)										// 初始化
	{
		group_point[i] = i / 2;
		group_point[i + 1] = size - group + i / 2;
	}
	std::size_t vernier = (group - 1) * 2;
	//GroupMath(size, 3);
   // 循环
	bool jump = false;
	while (true)
	{
		// 检查,游标所指向的位置如果达到最大值
		if (group_point[vernier] > group_point[vernier + 1])
		{
			if (vernier == 0)				// 首位达到最大值,结束运行
				break;
			// 移动游标到前一个索引位
			vernier -= 2;
			++group_point[vernier];				// 游标所在位的索引增加
			// 初始化游标后面的索引位
			for (int i = vernier + 2; i < group * 2; i += 2)
			{
				group_point[i] = group_point[vernier] + (i - vernier) / 2;
			}
			jump = true;
			continue;
		}

		if (jump)
		{
			vernier = (group - 1) * 2;
		}

		// 构造组合结构
		ret.second[count].ReLevel(group);
		for (int i = 0; i < group * 2; i += 2)
			ret.second[count].Push(ay[group_point[i]]);
		++count;

		// 推进
		++group_point[vernier];
	}

	return std::move(ret);
}




//
//void GroupSplit(int * ay, int size, int group)
//{
//	std::size_t *group_point = new std::size_t[group * 2];					// 
//	for (int i = 0; i < group * 2; i += 2)										// 初始化
//	{
//		group_point[i] = i / 2;
//		group_point[i + 1] = size - group + i / 2;
//	}
//	std::size_t vernier = (group - 1) * 2;
//	//GroupMath(size, 3);
//   // 循环
//	bool jump = false;
//	while (true)
//	{
//		// 检查,游标所指向的位置如果达到最大值
//		if (group_point[vernier] > group_point[vernier + 1])
//		{
//			if (vernier == 0)				// 首位达到最大值,结束运行
//				break;
//			// 移动游标到前一个索引位
//			vernier -= 2;
//			++group_point[vernier];				// 游标所在位的索引增加
//			// 初始化游标后面的索引位
//			for (int i = vernier + 2; i < group * 2; i += 2)
//			{
//				group_point[i] = group_point[vernier] + (i - vernier) / 2;
//			}
//			jump = true;
//			continue;
//		}
//
//		if (jump)
//		{
//			vernier = (group - 1) * 2;
//		}
//
//		// 循环打印结果
//		for (int i = 0; i < group * 2; i += 2)
//			std::cout << group_point[i] << ",";
//		std::cout << std::endl;
//
//
//
//		// 推进
//		++group_point[vernier];
//	}
//}

#endif // !__BASE_TOOLS_HPP__
