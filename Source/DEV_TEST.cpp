
#ifndef DISABLE_THIS_FILE
#define DISABLE_THIS_FILE
#endif // !CLOSE_THIS_FILE

//#include "CPPheader.hpp"
#include <iostream>
//#include "baseTools.hpp"




#ifndef DISABLE_THIS_FILE // DEBUG

enum BitWidth :char
{
	OneBit = 1,
	TwoBit = 2,
	FourBit = 4,
	EightBit = 8,
	SixteenBit = 16,
	ThirtytwoBit = 32
};

enum BitStyle
{
	PerBit, BlockBitWidth
};

// Mask Type 返回掩码类型
 // 类型提升 BUG --------FIXED
// Mask Type 返回掩码类型 使用位宽 1 2 4 8
template <BitWidth BWIDTH>
struct MaskType
{
	using type = char;
};

// Mask Type 返回掩码类型 当使用的 位宽是 16 bit
template <>
struct MaskType<BitWidth::SixteenBit>
{
	using type = char16_t;
};

// Mask Type 返回掩码类型 当使用的 位宽是 32 bit
template <>
struct MaskType<BitWidth::ThirtytwoBit>
{
	using type = char32_t;
};




// 快捷 掩码类型
template <BitWidth BWIDTH>
using MaskType_t = typename MaskType<BWIDTH>::type;

// 位移动 , 向左移动 SHIFT 个位置
template <bool left, std::size_t shift,std::size_t value>
struct BitShift
{
	constexpr static std::size_t value = value << shift;
};

// 为移动 向右移动 SHIFT 个位置
template <std::size_t shift, std::size_t value>
struct BitShift<false, shift, value>
{
	constexpr static std::size_t value = value >> shift;
};

// 快捷位移取值
template <bool left, std::size_t shift, std::size_t value>
constexpr std::size_t BitShift_v = BitShift<left, shift, value>::value;


// 位 and
template <std::size_t left,std::size_t right>
struct BitAnd
{
	constexpr static std::size_t value = (left & right);
};

//快捷位 and 操作
template <std::size_t left, std::size_t right>
constexpr std::size_t BitAnd_v = BitAnd<left, right>::value;



// 位 or
template <std::size_t left, std::size_t right>
struct BitOr
{
	constexpr static std::size_t value = (left | right);
};

//快捷位 or 操作
template <std::size_t left, std::size_t right>
constexpr std::size_t BitOr_v = BitOr<left, right>::value;

// 掩码的递归计算
template <std::size_t shift, typename MASK_TYPE, MASK_TYPE V>
struct _MaskValue
{
	constexpr static MASK_TYPE value = _MaskValue<shift - 1, MASK_TYPE, BitOr_v<BitShift_v<true, 1, V>, 1>>::value;
};

// 掩码的递归终结返回结果掩码值
template <typename MASK_TYPE, MASK_TYPE V>
struct _MaskValue<1, MASK_TYPE, V>
{
	constexpr static MASK_TYPE value = V;
};


// 掩码值
template <typename MASK_TYPE ,BitWidth BWIDTH>
struct MaskValue
{
	constexpr static MASK_TYPE value = _MaskValue<BWIDTH,MASK_TYPE,1>::value;
};

// 快捷掩码值获取
template <typename MASK_TYPE, BitWidth BWIDTH>
constexpr MASK_TYPE MaskValue_v = MaskValue<MASK_TYPE, BWIDTH>::value;




template <typename TKEY, BitWidth BWIDTH = BitWidth::OneBit, BitStyle BSTYLE = BitStyle::BlockBitWidth>
class EventModelManage;


// Manage单例函数
// 手动写入 Manage 对象
template <typename MANAGE, typename TKEY = typename MANAGE::KEY_TYPE, BitWidth BWIDTH = MANAGE::BIT_WIDTH, BitStyle BSTYLE = MANAGE::BIT_STYLE,
	typename READ = std::false_type,
	typename = std::enable_if_t<std::disjunction<std::is_base_of<EventModelManage<TKEY, BWIDTH, BSTYLE>, MANAGE>, READ>::value>>
	std::shared_ptr<EventModelManage<TKEY, BWIDTH, BSTYLE>>  InstanceManage(MANAGE * specManage)
{
	static std::shared_ptr<EventModelManage<TKEY, BWIDTH, BSTYLE>> instance{ nullptr };
	if (nullptr == instance)
	{
		instance.reset(specManage);
	}
	return instance;
}


// 自动写入 Manage 对象，要求Manage 对象存在默认构造函数
template <typename MANAGE, typename TKEY = typename MANAGE::KEY_TYPE, BitWidth BWIDTH = MANAGE::BIT_WIDTH, BitStyle BSTYLE = MANAGE::BIT_STYLE
	, typename = std::enable_if_t<std::is_default_constructible_v<MANAGE>>>
	std::shared_ptr<EventModelManage<TKEY, BWIDTH, BSTYLE>>  InstanceManage()
{
	auto tp = InstanceManage<MANAGE, TKEY, BWIDTH, BSTYLE, std::true_type>(nullptr);
	if (nullptr == tp)
	{
		return InstanceManage(new MANAGE());
	}
	return tp;
}



// 已复制至 EventModel.hpp
template <typename TVALUE>
struct EventModel
{
	uint32_t index;
	TVALUE value;

	using VALUE_TYPE = TVALUE;

	EventModel(uint32_t const & _idx, TVALUE const& _value)
		:index(_idx), value(_value)
	{}

	EventModel() {}
};

// 已复制至 EventModelSet.hpp
template <typename MANAGE, typename TVALUE, typename TKEY = typename MANAGE::KEY_TYPE,
	typename = std::enable_if<std::is_base_of_v<EventModelManage<TKEY, MANAGE::BIT_WIDTH, MANAGE::BIT_STYLE>, MANAGE>>>
	class EventModelSet
{
public:
	using VALUE_TYPE = EventModel<TVALUE>;
	using BASE_MANAGE = EventModelManage<TKEY, MANAGE::BIT_WIDTH, MANAGE::BIT_STYLE>;
	using SPEC_MANAGE = MANAGE;
	using SHARED_MANAGE = std::shared_ptr<BASE_MANAGE>;
	using UNQIUE_VTYPE = std::unique_ptr<VALUE_TYPE[]>;

public:
	EventModelSet(std::size_t _event_count)
		: event_buf(new VALUE_TYPE[_event_count])
		, cur_event_count(0)
		, capacity(_event_count)
		, event_manage(InstanceManage<SPEC_MANAGE>())
	{}

	void Push(TVALUE const & _value, TKEY const & _key)
	{
		if (cur_event_count >= capacity)
			return;
		event_buf[cur_event_count++] = VALUE_TYPE(event_manage->GetIndexFromKey(_key), _value);
	}

	template <typename = std::enable_if_t<!std::is_same_v<TVALUE, TKEY>>>
	void Push(TKEY const & _key, TVALUE const & _value)
	{
		if (cur_event_count >= capacity)
			return;
		push(_value, _key);
	}

	VALUE_TYPE const & Get(int N)const
	{
		if (N >= capacity)
			throw std::out_of_range("内存越界 Ret Get（int N）const, ");
		return event_buf[N];
	}

	std::size_t const Capacity()const
	{
		return capacity;
	}

	~EventModelSet()
	{
		
	}

private:
	UNQIUE_VTYPE event_buf;
	std::size_t cur_event_count,capacity;
	SHARED_MANAGE event_manage;
};

// 已复制至 EventModelManage.hpp
template <typename TKEY, BitWidth BWIDTH = BitWidth::OneBit, BitStyle BSTYLE = BitStyle::BlockBitWidth>
class EventModelManage
{
public:
	using KEY_TYPE = TKEY;
	using MASK_TYPE = MaskType_t<BIT_WIDTH>;
	constexpr static BitWidth BIT_WIDTH = BWIDTH;														// 位宽
	constexpr static BitStyle BIT_STYLE = BSTYLE;														// 数据的摆放方式
	constexpr static MASK_TYPE BIT_MASK = MaskValue_v<MaskType_t<BIT_WIDTH>, BIT_WIDTH>;	// 根据位的长度选择掩码类型
public:
	EventModelManage() = default;
	virtual std::size_t const EventTypeCount() const = 0;							// 返回事务中所有可能出现的事件的总数量
	virtual uint32_t const GetIndexFromKey(KEY_TYPE const& keyCode) const = 0;		// 通过数据编码获取数据的索引 
};




//class myManage : public EventModelManage<int,BitWidth::EightBit>
//{
//public:
//	myManage() = default;
//	virtual std::size_t const EventTypeCount() const						// 返回事务中所有可能出现的事件的总数量
//	{
//		std::cout << "EVENT TYPE COUNT" << std::endl;
//		return 0;
//	}
//
//	virtual uint32_t const GetIndexFromKey(KEY_TYPE const& keyCode) const
//	{
//		std::cout << "GET INDEX FROM KEY" << std::endl;
//		return 0;
//	}
//};

template <std::size_t N>
struct N64Type
{
	constexpr static __int64 value[N];
};

enum struct BitWidth :short
{
	OneBit = 1,
	TwoBit = 2,
	FourBit = 4,
	EightBit = 8,
	TenSixBit = 16,
	ThreetwoBit = 32,
	SixfourBit = 64,
	OnetwoeightBit = 128,
	TwofivesixBit = 256,
	FiveonetwoBit = 512,
	OnezerotwofourBit = 1024,
	TwozerofoureightBit = 2048,
	FourzeroninesizBit = 4096,
	EightoneninetwoBit = 8192
};

enum struct SectionWidth
{
	PerBit = 1,
	TwoBit = 2,
	FourBit = 4,
	EightBit = 8,
	SixteenBit = 16,
	ThirtytwoBit = 32,
	SixtyfourBit = 64
};
//
//template <BitWidth BWIDTH,SectionStyle DEFAULT = SectionStyle::PerBit>
//struct TryAlignBWMapSS
//{
//	constexpr static SectionStyle value = DEFAULT;
//};
//
//template <SectionStyle DEFAULT>
//struct TryAlignBWMapSS<BitWidth::OneBit,DEFAULT>
//{
//	constexpr static SectionStyle value = SectionStyle::PerBit;
//};
//
//template <SectionStyle DEFAULT>
//struct TryAlignBWMapSS<BitWidth::TwoBit, DEFAULT>
//{
//	constexpr static SectionStyle value = SectionStyle::TwoBit;
//};
//
//template <SectionStyle DEFAULT>
//struct TryAlignBWMapSS<BitWidth::FourBit, DEFAULT>
//{
//	constexpr static SectionStyle value = SectionStyle::FourBit;
//};
//
//template <SectionStyle DEFAULT>
//struct TryAlignBWMapSS<BitWidth::EightBit, DEFAULT>
//{
//	constexpr static SectionStyle value = SectionStyle::EightBit;
//};
//
//template <SectionStyle DEFAULT>
//struct TryAlignBWMapSS<BitWidth::TenSixBit, DEFAULT>
//{
//	constexpr static SectionStyle value = SectionStyle::SixteenBit;
//};
//
//template <SectionStyle DEFAULT>
//struct TryAlignBWMapSS<BitWidth::ThreetwoBit, DEFAULT>
//{
//	constexpr static SectionStyle value = SectionStyle::ThirtytwoBit;
//};
//
//template <SectionStyle DEFAULT>
//struct TryAlignBWMapSS<BitWidth::SixfourBit, DEFAULT>
//{
//	constexpr static SectionStyle value = SectionStyle::SixtyfourBit;
//};
//
//// Mask Type 返回掩码类型
// // 类型提升 BUG --------FIXED
//// Mask Type 返回掩码类型 使用位宽 1 2 4 8
//template <BitWidth BWIDTH>
//struct MaskType
//{
//	using type = char;
//};
//
//// Mask Type 返回掩码类型 当使用的 位宽是 16 bit
//template <>
//struct MaskType<BitWidth::TenSixBit>
//{
//	using type = char16_t;
//};
//
//// Mask Type 返回掩码类型 当使用的 位宽是 32 bit
//template <>
//struct MaskType<BitWidth::ThreetwoBit>
//{
//	using type = char32_t;
//};
//
//
//
//
//// 快捷 掩码类型
//template <BitWidth BWIDTH>
//using MaskType_t = typename MaskType<BWIDTH>::type;
//
//
////template <BitWidth b,SectionStyle s = TryAlignBWMapSS<b>::value>
////struct mytest
////{
////
////};
//
//// 计算给定的位宽和 对应的值类型，所需的数量
//template <BitWidth BWIDTH,typename VALUE_TYPE>
//struct EventModelCount
//{
//private:
//	constexpr static std::size_t bit_count = (static_cast<int>(BWIDTH) / (sizeof(VALUE_TYPE) * 8));
//public:
//	constexpr static std::size_t value = bit_count == 0 ? 1 : bit_count;
//};
//// 计算出
//
//
//template <BitWidth BWIDTH, SectionWidth BSWIDTH>
//struct IsSecWidthLessBitWidth
//{
//	constexpr static bool value = static_cast<int>(BSWIDTH) <= static_cast<int>(BWIDTH) ? true : false;
//};
//
//template <BitWidth BWIDTH, SectionWidth BSWIDTH>
//constexpr bool IsSecWidthLessBitWidth_v = IsSecWidthLessBitWidth<BWIDTH, BSWIDTH>::value;
//
//template <typename TKEY, typename TINDEX, BitWidth BWIDTH, SectionWidth BSWIDTH, 
//	typename CHECK = std::enable_if_t<IsSecWidthLessBitWidth_v<BWIDTH, BSWIDTH>>>
//class EventModelManage
//{
//public:
//	using KEY_TYPE = TKEY;
//	using INDEX_TYPE = TINDEX;
//	using MASK_TYPE = MaskType_t<BSWIDTH>;																// 单个节的存放数据类型
//	using VALUE_TYPE = MASK_TYPE;																		// 默认值类型等同于掩码类型
//	constexpr static BitWidth BIT_WIDTH = BWIDTH;														// 总位宽 
//	constexpr static SectionWidth BIT_SECTION_WIDTH = BSWIDTH;											// 数据的节位宽
//	constexpr static MASK_TYPE BIT_MASK = MaskValue_v<MaskType_t<BSWIDTH>, BIT_SECTION_WIDTH>;			// 根据节的长度选择节掩码类型
//	constexpr static std::size_t EVENT_MODEL_VALUE_COUNT = EventModelValueCount_v<BWIDTH, VALUE_TYPE>;  //
//public:
//	EventModelManage() = default;
//	virtual std::size_t const EventTypeCount() const = 0;												// 返回事务中所有可能出现的事件的总数量
//	virtual uint32_t const GetIndexFromKey(KEY_TYPE const& keyCode) const = 0;							// 通过数据编码获取数据的索引 
//};
//
//
//template <typename T>
//struct BitCountOfValueType;
//
//template <typename T>
//constexpr std::size_t BitCountOfValueType_v = BitCountOfValueType<T>::value;
//
//template <>
//struct BitCountOfValueType<__int8>
//{
//	constexpr static std::size_t value = 8;
//};
//
//template <>
//struct BitCountOfValueType<__int16>
//{
//	constexpr static std::size_t value = 16;
//};
//
//template <>
//struct BitCountOfValueType<__int32>
//{
//	constexpr static std::size_t value = 32;
//};
//
//template <>
//struct BitCountOfValueType<__int64>
//{
//	constexpr static std::size_t value = 64;
//};
//
//
//
//
//// TVALUE	EventModel 中值的类型，一般该类型与 BitWidth 相关
//// TINDEX	EventModel 中存储的索引值
//template <typename MANAGE,
//	typename = std::enable_if_t<std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE,typename MANAGE::INDEX_TYPE,MANAGE::BIT_WIDTH,MANAGE::BIT_SECTION_WIDTH>,MANAGE>>>
//struct EventModel
//{
//private:
//	std::size_t count_value;
//	std::size_t count_section;
//public:
//	typename MANAGE::INDEX_TYPE index;																	// 存储 索引
//	typename MANAGE::VALUE_TYPE value[MANAGE::EVENT_MODEL_VALUE_COUNT];									// 存储 数值
//
//	EventModel(typename MANAGE::INDEX_TYPE const & _idx)							// 构造函数 需要 索引和值
//		:count_value(0), count_section(0),is index(_idx)
//	{}
//
//	EventModel(typename MANAGE::INDEX_TYPE const & _idx, typename MANAGE::VALUE_TYPE const & _value)							// 构造函数 需要 索引和值
//		:count_value(0), count_section(0), index(_idx)
//	{
//		
//	}
//
//	EventModel(){}
//
//	EventModel const & operator=(EventModel const & rhs)						// 返回 const &  禁用连续赋值
//	{
//		this->index = rhs.index;
//		this->value = rhs.value;
//		return *this;
//	}
//
//	bool const Append(typename MANAGE::VALUE_TYPE const & _value)
//	{
//		if (MANAGE::EVENT_MODEL_VALUE_COUNT == count_value)												// 检查是否满，当 count_value 和 VALUE_COUNT 相等时，表明存储满
//			return true;																				// 防止多余的追加
//
//		typename MANAGE::VALUE_TYPE tp = _value & MANAGE::BIT_MASK;										// 掩码位操作 
//		tp << (BitCountOfValueType_v<typename MANAGE::VALUE_TYPE> - static_cast<int>(MANAGE::BIT_SECTION_WIDTH) - count_section);			// 位移操作
//
//		value[count_value] |= tp;																		// 追加数据
//
//		count_section += static_cast<int>(MANAGE::BIT_SECTION_WIDTH);									// 指向下一个追加数据段
//
//		if (BitCountOfValueType_v<typename MANAGE::VALUE_TYPE> == count_section)																// 当 count_section 和 mask_type 的位数相等时，表明当前 mask_type 数据段填满
//		{
//			++count_value;																				// 指向下一个  value type 段
//			count_section = 0;																			// 重置 count_section 段	
//		}
//
//		if (MANAGE::EVENT_MODEL_VALUE_COUNT == count_value)												// 检查是否满，当 count_value 和 VALUE_COUNT 相等时，表明存储满
//			return true;																				// 当存储满时，返回 true
//
//		return false;
//	}
//};
//
////
//template <std::size_t VALUE_COUNT, typename TVALUE, typename TINDEX>
//EventModel<VALUE_COUNT, TVALUE, TINDEX>::EventModel(TINDEX const & _idx, TVALUE const& _value)
//	:count_value(0), count_section(0), index(_idx), value(_value)
//{}
//
//template <std::size_t VALUE_COUNT, typename TVALUE, typename TINDEX>
//EventModel<VALUE_COUNT, TVALUE, TINDEX>::EventModel()
//{}
//
//
//template <std::size_t VALUE_COUNT, typename TVALUE, typename TINDEX>
//EventModel<VALUE_COUNT, TVALUE, TINDEX> const & EventModel<VALUE_COUNT, TVALUE, TINDEX>::operator=(EventModel<VALUE_COUNT, TVALUE, TINDEX> const & rhs)
//{
//	this->index = rhs.index;
//	this->value = rhs.value;
//	return *this;
//}


template <typename T>
void testT(int v, T c)
{
	std::cout << "test t" << std::endl;
}

template <>
void testT<double>(int v, double c)
{
	std::cout << "double spec" << std::endl;
}


class test
{
public:
	~test()
	{
		std::cout << "析构函数" << std::endl;
	}
};

void f()
{
	std::unique_ptr<test[],std::default_delete<test[]>> tp(new test[10]);

	tp.reset(new test[3]);
}

class V
{
private:
	int _v = 0;
public:

	void fc(int vs )
	{
		std::cout << "s";
	}
};

#include <functional>

//
//template <typename DERVIED_TYPE>
//class CRTP
//{
//protected:
//	DERVIED_TYPE * derived()
//	{
//		return static_cast<DERVIED_TYPE*>(this);
//	}
//
//	DERVIED_TYPE const * derived()const
//	{
//		return static_cast<DERVIED_TYPE const*>(this);
//	}
//};
//
//
//template <typename U,template<typename,typename> class DERVIED_TYPE>
//class HelloSpec : public CRTP<DERVIED_TYPE<U>>
//{
//public:
//	void RowSum()
//	{
//		this->derived()->hello();
//	}
//};
//
//template <typename U, 
//	template<typename, typename = std::enable_if_t<std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE, typename MANAGE::INDEX_TYPE, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>> class DERVIED_TEMPLATE>
//class DoubleKillSpec : public CRTP<DERVIED_TEMPLATE<U>>
//{
//public:
//	void RowSum(std::function<std::size_t(typename U::VALUE_TYPE)> sec_func)
//	{
//		this->derived()->double_kill();
//	}
//};

class MANAGE_TYPE
{
public:
	using VALUE_TYPE = double;
	using MASK_TYPE = VALUE_TYPE;
};
//
//template <typename MANAGE,typename CHECK = int>
//class spe : public std::conditional_t<sizeof(double) != 8,HelloSpec<MANAGE_TYPE,spe>,DoubleKillSpec<MANAGE_TYPE,spe>>
//{
//public:
//	using VALUE_TYPE = double;
//
//	template <typename U, template<typename> class DERVIED_TYPE>
//	friend class DoubleKillSpec;
//
//	template <typename U, template<typename> class DERVIED_TYPE>
//	friend class HelloSpec;
//
//protected:
//	void hello()
//	{
//		std::cout << "hello sp1" << std::endl;
//	}
//
//
//	void double_kill()
//	{
//		std::cout << "double killer" << std::endl;
//	}
//};

//template <typename MANAGE,
//	typename CHECK = std::enable_if_t<std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE, typename MANAGE::INDEX_TYPE, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
//class freq_tree
//{
//public:
//	freq_tree(std::size_t _level)
//	std::size_t freq_level()const;
//	void cut_greater
//
//	using TREE_TYPE = typename MANAGE::INDEX_TYPE;
//	using UNIQUE_TREE_PTR = std::unique_ptr<TREE_TYPE[], std::default_delete<TREE_TYPE[]>>;
//
//private:
//	UNIQUE_TREE_PTR m_freq_tree;
//};
//
//
//template <typename T>
//class C
//{
//public:
//	template <bool = sizeof(T) == 4>
//	void hello()
//	{
//		std::cout << "base hello" << std::endl;
//	}
//
//	template <typename>
//	void hello()
//	{
//		debug();
//	}
//};


class N
{
public:
	N(int _N)
	{
		mN = _N;
	}

	int mN;
};


std::size_t BitOneCount(int vt) 
{
	int count = 0;
	while (vt)
	{
		vt = vt & (vt - 1);
		++count;
	}
	return count;
}


// 阶乘
std::size_t LevelMulti(std::size_t value, std::size_t terminate = 0)
{
	if (value == terminate)
		return 1;
	return value * LevelMulti(value - 1);
}

// 组合数计算
std::size_t GroupMath(std::size_t total, std::size_t group_count)
{
	return LevelMulti(total) / (LevelMulti(group_count) * LevelMulti(total - group_count));
}




template <typename T>
struct TTreeNode
{
	std::unique_ptr<TTreeNode>  left; 
	std::unique_ptr<TTreeNode>  right;
	T value;

	TTreeNode()
		:left(nullptr),right(nullptr),value(std::numeric_limits<T>::max())
	{}

	TTreeNode(T _value)
		:left(nullptr), right(nullptr), value(_value)
	{}
};

int Static()
{
	static int nu = 0;
	return nu++;
}

class TMAM
{
public:
	using INDEX_TYPE = int;
};
class FrequentTree
{
public:
	FrequentTree(){}
};


template <typename MANAGE>
class StructTwoTree
{
public:
	using INDEX_TYPE = typename MANAGE::INDEX_TYPE;
	using NODE_TYPE = TTreeNode<INDEX_TYPE>;
	using UNIQUE_NODE_PTR = std::unique_ptr<NODE_TYPE>;
	using PAIR_COUNT_PATHS_TYPE = std::pair<std::size_t, std::unique_ptr<INDEX_TYPE[], std::default_delete<INDEX_TYPE[]>>>;
public:
	StructTwoTree()
		:work_level(1),root(new NODE_TYPE())
	{}

	void Push(typename MANAGE::INDEX_TYPE const * index_list,std::size_t count)
	{
		for (int i = 0; i < count; ++i)
		{
			_Push(root.get(), index_list[i]);
			//std::cout << "I : " << i << " V : " << index_list[i] << std::endl;
		}

		//std::cout << "push test : " << std::endl;
	}

	bool const Push(NODE_TYPE * root ,typename MANAGE::INDEX_TYPE const & idx,bool stat = true,std::size_t current_level = 1)
	{

		if (nullptr == root->left)
		{
			root->left.reset(new NODE_TYPE(idx));
			//std::cout << "left : work level : " << work_level << " idx : " << idx << std::endl;
			return true;
		}

		if (nullptr == root->right)
		{
			root->right.reset(new NODE_TYPE(idx));
			////std::cout << "right : work level : " << work_level << " idx : " << idx << std::endl;
			//if (stat)
			//	std::cout << "NEXT WORK LEVEL " << std::endl;
			if (stat)
				++work_level;
			return true;
		}

		if (current_level == work_level)
			return false;

		// 向左查看
		if (_Push(root->left.get(), idx, false,current_level + 1))
			return true;

		// 向右查看 
		return _Push(root->right.get(), idx, stat,current_level + 1);
	}

	//void Distance
	void cut(std::size_t distance, FrequentTree a);
	void PathResearch(FrequentTree );

private:
	PAIR_COUNT_PATHS_TYPE path;
	std::size_t work_level;
	UNIQUE_NODE_PTR root;
	// 路径节点
};
#include <vector>
std::vector<int*> tmp;



void splait(int * ay, int size, int group)
{
	std::size_t *group_point = new std::size_t[group * 2];					// 
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

		// 循环打印结果
		for (int i = 0; i < group * 2; i += 2)
		{
			std::cout << group_point[i] << ",";
			std::cout << ay[i/2] << ",";
			
		}
	
		std::cout << std::endl;



		// 推进
		++group_point[vernier];
	}
}


int main()
{
	int * a = new int[2]{ 1,2 };
	splait(a, 2, 2);

	//tmp.push_back(new int(32));
	//int * pa = new int[14];
	//for (int i = 0; i < 14; ++i)
	//{
	//	pa[i] = i;
	//}

	//StructTwoTree<TMAM> opo;
	//opo.Push(pa, 14);

	//std::cout << "int : "<<log(8) << std::endl;
	//std::cout << "int : "<< (int)floor(log2f(8.0)) << std::endl;
	//std::cout << floor(3.1415926f) << " " << (int)(3.1415926f) << std::endl;
	//std::cout << floor(3.99545) << " " << (int)(3.99545) << std::endl;
	//std::numeric_limits<int>::max();
	//std::unique_ptr<int> po(new int(22));

	//int * p =  po.get();

	//std::cout << *p << std::endl;
	//std::cout << *po << std::endl;

	//
	//int *a = new int[10];
	//for (int i = 0; i < 6; ++i)
	//	a[i] = i;

	//std::cout << GroupMath(7,2) << std::endl;

	//splait(a, 10, 5);

	////std::cout << GroupMath(4, 3) << std::endl;

	//std::unique_ptr<int> pa(new int(32));

	//std::pair<int, std::unique_ptr<int>> p;
	//p.first = 10;
	//p.second.reset(new int(32));

	//std::pair<int, std::unique_ptr<int>> pp;

	//pp = std::move(p);

	////std::cout << *p.second << std::endl;
	//std::cout << *pp.second << std::endl;
		



	/*int a = 745369;
	std::cout << BitOneCount(a) << std::endl;*/
	//N* pn = new N[10]{ N(2) };

	//C<int> ci;
	//std::function<int(spe<double>::VALUE_TYPE)> f;
	//spe<MANAGE_TYPE> sint;

	//sint.RowSum([](double) -> std::size_t{ return 0; });

	
	//sint.RowSum([](int)->std::size_t {return 0; });


	//spe<double> sdouble;
	//sdouble.RowSum()

	/*spe sp;
	sp.SP1Hello();

	sp.SP2Hello();*/

	//spe<int> s1;

	//spe<long double> sd;
	//std::cout << sizeof(long double) << std::endl;
	//s1.rowc(12);

	//sd.rowc(15, []() {std::cout << "ffff"; });

	//sd.rowc(12, []() {});
	//sd.rowc(12);

	
	//tp.release();
	//testT(12, 13.0);
	//int v = EventModelCount<BitWidth::ThreetwoBit , char>::value;

	//std::cout << v << std::endl;

	//EventModel<4, __int8, __int32,(__int8)3> ep;
	//eo.append

	//mytest<BitWidth::EightBit,SectionStyle::EightBit> e;
	//mytest<BitWidth::SixfourBit> e2;
	//mytest<BitWidth::FourzeroninesizBit> e3;
	//mytest<BitWidth::TwofivesixBit,SectionStyle::EightBit> e4;

	//auto as = e;

	//std::cout << typeid(e).name() << std::endl;
	//std::cout << typeid(as).name() << std::endl;
	//std::cout << typeid(e2).name() << std::endl;
	//std::cout << typeid(e3).name() << std::endl;
	//std::cout << typeid(e4 ).name() << std::endl;

	//int a[2];
	//std::cout << (int)myManage::BIT_MASK << std::endl;

	//std::cout << (1 << 4) << std::endl;
	//std::cout << (8 >> 2) << std::endl;
	//int c = 0;
	//c = MaskValue<MaskType_t<BitWidth::FourBit>, BitWidth::FourBit>::value;

	//std::cout << c << std::endl;

	//c = MaskValue<MaskType_t<BitWidth::OneBit>, BitWidth::OneBit>::value;

	//std::cout << c << std::endl;


	//c = MaskValue<MaskType_t<BitWidth::TwoBit>, BitWidth::TwoBit>::value;

	//std::cout << c << std::endl;


	//c = MaskValue<MaskType_t<BitWidth::EightBit>, BitWidth::EightBit>::value;

	//std::cout << c << std::endl;

	//c = MaskValue<MaskType_t<BitWidth::SixteenBit>, BitWidth::SixteenBit>::value;

	//std::cout << c << std::endl;

	//c = MaskValue<MaskType_t<BitWidth::ThirtytwoBit>, BitWidth::ThirtytwoBit>::value;

	//std::cout << c << std::endl;


	//std::cout << "hello world" << std::endl;
	//InstanceManage<myManage>();
	//EventModelSet<myManage, int> es(12);

	//es.Push(1, 2);
	//es.Push(2, 2);
	//es.Push(3, 3);
	//es.Push(5, 5);

	//auto v = es.Get(2);

	//std::cout << v.index << " " << v.value << std::endl;


	//InstanceManage<myManage>()->EventTypeCount();
	//InstanceManage<myManage>()->GetIndexFromKey(2);
	

	system("pause");
	return 0;
}

#endif // DISABLE_THIS_FILE