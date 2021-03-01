#ifndef __STRUCTURAL_BINARY_TREE_HPP__
#define __STRUCTURAL_BINARY_TREE_HPP__
#include "CPPheader.hpp"
#include "FreqTree.hpp"

template <typename T,typename FLAG = std::size_t>
struct SBTreeNode
{
	std::unique_ptr<SBTreeNode>  left;
	std::unique_ptr<SBTreeNode>  right;
	T value;
	FLAG flag_id;

	SBTreeNode()
		:left(nullptr), right(nullptr), value(std::numeric_limits<T>::max()),flag_id(std::numeric_limits<FLAG>::max())
	{}

	SBTreeNode(T _value)
		:left(nullptr), right(nullptr), value(_value), flag_id(std::numeric_limits<FLAG>::max())
	{}
};



template <typename MANAGE>
class StructuralBTree
{
public:
	using INDEX_TYPE = typename MANAGE::INDEX_TYPE;
	using NODE_TYPE = SBTreeNode<INDEX_TYPE>;
	using UNIQUE_NODE_PTR = std::unique_ptr<NODE_TYPE>;
	using PAIR_INDEX_PATHS_TYPE = std::pair<std::size_t, std::unique_ptr<INDEX_TYPE[], std::default_delete<INDEX_TYPE[]>>>;		// 路径索引和路径节点的类型
	using PAIR_UNIQUE_INDEX_PATH_TYPE = std::pair<std::size_t,std::unique_ptr<PAIR_INDEX_PATHS_TYPE[], std::default_delete<PAIR_INDEX_PATHS_TYPE[]>>>;		//count ,索引路径数组类型
	using FREQ_TREE_TYPE = FrequentTree<MANAGE>;			// 频繁树类型
	using UNIQUE_FREQ_TREE_PTR = std::unique_ptr<FREQ_TREE_TYPE[], std::default_delete<FREQ_TREE_TYPE[]>>;
	using PAIR_COUNT_FREQ_LIST_TYPE = std::pair<std::size_t, UNIQUE_FREQ_TREE_PTR>;
public:
	StructuralBTree()
		:work_level(1), root(new NODE_TYPE())
	{}

	// 传入所有事件索引
	StructuralBTree(INDEX_TYPE const * index_list, std::size_t count)
		:work_level(1), root(new NODE_TYPE())
	{
		Push(index_list,count);
	}

	void Push(INDEX_TYPE const * index_list, std::size_t count)
	{
		for (int i = 0; i < count; ++i)
		{
			Push(root.get(), index_list[i]);
			//std::cout << "I : " << i << " V : " << index_list[i] << std::endl;
		}

		//std::cout << "push test : " << std::endl;
	}

	bool const Push(NODE_TYPE * root, INDEX_TYPE const & idx, bool stat = true, std::size_t current_level = 1)
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
		if (Push(root->left.get(), idx, false, current_level + 1))
			return true;

		// 向右查看 
		return Push(root->right.get(), idx, stat, current_level + 1);
	}

	// 传入所有的频繁子树
	//void Distance
	PAIR_COUNT_FREQ_LIST_TYPE cut(std::size_t distance, PAIR_COUNT_FREQ_LIST_TYPE const & pair_freq_list)
	{
		// 首先清空
		pair_freq_list.second[0].ClearUniquePool();
		// 然后对所有的频繁子树调用 unique_pool 获取所有可能的索引
		for (int i = 0; i < pair_freq_list.first; ++i)
		{
			pair_freq_list.second[i].UniquePool();
		}
		// 创建索引路径表
		// 存储所有频繁子树中出现的单独索引个数
		paths.first = pair_freq_list.second[0].idx_pool_count;				// 初始化数量
		paths.second.reset(new PAIR_INDEX_PATHS_TYPE[paths.first]);
		// 满二叉树深度
		int bt_depth = static_cast<int>(log2f(InstanceManage<MANAGE>()->GetEventTypeCount()));
		bt_depth += 1;														// 多出一个 index 位置
		// 分配大小 
		for (int i = 0; i < paths.first; ++i)
		{
			paths.second[i].first = 1;
			paths.second[i].second.reset(new INDEX_TYPE[bt_depth]);
			paths.second[i].second[0] = pair_freq_list.second[0].unique_idx_pool[i];			// 首个位置存放索引，后去位置存放路径
		}

		//// 申请路径空间,路径空间的收个元素存储对应的索引
		for (int i = 0; i < paths.first; ++i) 
		{
			PathResearch(root.get(), paths.second[i].second[0], i);
		}

#ifdef PRINT_PATH
		for (int i = 0; i < paths.first; ++i)
		{
			for (int j = 0; j < paths.second[i].first; ++j)
			{
				std::cout << paths.second[i].second[j] << " path ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
#endif // DEBUG



		// 被裁减的枝叶
		std::unique_ptr<INDEX_TYPE[],std::default_delete<INDEX_TYPE[]>> cuted(new INDEX_TYPE[pair_freq_list.second[0].idx_pool_count]);
		std::size_t cuted_count = 0;

		//裁剪频繁子树
		std::vector<FREQ_TREE_TYPE*> tmpFreq;
		for (int i = 0; i < pair_freq_list.first; ++i)
		{
			if (pair_freq_list.second[i].GetLevel() == 1)					// 单棵子树无法裁剪
				continue;
			// 首先清空 unique pool
			pair_freq_list.second[i].ClearUniquePool();
			// 把当前频繁子树的所有子级子树放置到 unique pool
			pair_freq_list.second[i].UniquePool();

			// 求所有的 2 2 组合
			auto tmp_k2 = GroupSplit<MANAGE, PAIR_COUNT_FREQ_LIST_TYPE, INDEX_TYPE>(pair_freq_list.second[i].unique_idx_pool.get(), pair_freq_list.second[i].idx_pool_count, 2);
			
			// 检查所有的 k2
			for (int j = 0; j < tmp_k2.first; ++j)
			{
				auto index1 = tmp_k2.second[j].Get(0);
				auto index2 = tmp_k2.second[j].Get(1);
				// 查找对应的路径索引
				bool stat = false;
				for (int k = 0; k < paths.first; ++k)
				{
					if (paths.second[k].second[0] == index1)
					{
						index1 = k;
						if (stat)					// 已经为 true 直接返回
							break;
						else
							stat = true;
						continue;
					}
					if (paths.second[k].second[0] == index2)
					{
						index2 = k;				// 已经为 true 直接返回
						if (stat)
							break;
						else
							stat = true;
					}
				}
				// 
				stat = false;       // 复用用于防止执行流穿行
				// 比对路径
				std::size_t path_distance = 0;
				std::size_t max_path_distance = paths.second[index1].first > paths.second[index2].first ? paths.second[index1].first : paths.second[index2].first;
				std::size_t min_path_distance = paths.second[index1].first > paths.second[index2].first ? paths.second[index2].first : paths.second[index1].first;

					// 大路径完全包含小路径 
				if (paths.second[index1].second[min_path_distance - 1] == paths.second[index2].second[min_path_distance - 1] && !stat)		// 较短路径的最后一个路径和长路径的路径节点复合
				{	// 路径重合长路径和段路径的差值 - 1 为路径距离
					path_distance = max_path_distance - min_path_distance - 1;
					stat = true;
				}

				// 两个路径完全无关
				if (paths.second[index1].second[1] == paths.second[index2].second[1] && paths.second[index1].second[2] != paths.second[index2].second[2] && !stat)
				{
					path_distance = paths.second[index1].first + paths.second[index2].first - 6;      //不计算初始节点距离
					stat = true;
				}

				// 大路径部分包含小路径
				if (!stat)
				{
					for (int z = 1; z < min_path_distance; ++z)
					{
						if (paths.second[index1].second[z] != paths.second[index2].second[z])
						{
							path_distance = paths.second[index1].first + paths.second[index2].first - 2 * (z + 1) + 1;
						}
					}
				}

				// 把大于指定最大距离的子树添加到 cuted 数组
				if (path_distance > distance)
				{
					bool has_index1 = false, has_index2 = false;
					for (int c = 0; c < cuted_count; ++c)
					{
						if (cuted[c] == index1)
						{
							has_index1 = true;
							continue;
						}

						if (cuted[c] == index2)
						{
							has_index2 = true;
							
						}

						if (has_index1 && has_index2)
							break;
					}

					if (!has_index1)					// 如果 cuted 中不存在 index1 则把 index1 放入其中
					{
						cuted[cuted_count] = index1;
						++cuted_count;
					}

					if (!has_index2)					// 如果 cuted 中不存在 index2 则把 index2 放入其中
					{
						cuted[cuted_count] = index2;
						++cuted_count;
					}
				}
			}
			// 根据 cuted 数组 和 unique_idx_pool 比对,所有不在 cutted 的子节点重新构造一颗频繁子树集
			// 如果 仅有一个元素 则不进行频繁子树集的构造
			if (pair_freq_list.second[0].idx_pool_count - cuted_count <= 1)
			{
				cuted_count = 0;
				continue;
			}
			else
			{
				// 创建一棵频繁树,树的 level 为 idx_pool_count 和  cuted_count 的差值
				tmpFreq.push_back(new FrequentTree<MANAGE>(pair_freq_list.second[0].idx_pool_count - cuted_count));
				// 检查和追加
				bool is_exist;
				for (int ipc = 0; ipc < pair_freq_list.second[0].idx_pool_count; ++ipc)
				{
					is_exist = false;
					for (int ct = 0; ct < cuted_count; ++ct)
					{
						if (pair_freq_list.second[0].unique_idx_pool[ipc] == cuted[ct])
						{
							is_exist = true;
							break;
						}
					}
					if (!is_exist)				// 如果不存在，则把该索引位置推入子树中
					{
						tmpFreq.back()->Push(pair_freq_list.second[0].unique_idx_pool[ipc]);
					}
				}
			}

			cuted_count = 0;			// 重置裁剪数组长度
		}

		PAIR_COUNT_FREQ_LIST_TYPE ret;
		ret.first = tmpFreq.size();
		ret.second.reset(new FrequentTree<MANAGE>[ret.first]);

		// 复制数据到返回值
		for (int cy = 0; cy < ret.first; ++cy)
		{
 			ret.second[cy] = *tmpFreq[cy];
		}
		return std::move(ret);
	}

	bool PathResearch(NODE_TYPE * root ,INDEX_TYPE const & index,std::size_t path_index,std::size_t start = 1)
	{
		if (nullptr == root)
			return false;

		if (root->value == index)
		{
			++paths.second[path_index].first;
			paths.second[path_index].second[start] = root->value;
			return true;
		}

		
		if (PathResearch(root->left.get(), index,path_index, start + 1))
		{
			++paths.second[path_index].first;
			paths.second[path_index].second[start] = root->value;
			return true;
		}

		if (PathResearch(root->right.get(), index, path_index, start + 1))
		{
			++paths.second[path_index].first;
			paths.second[path_index].second[start] = root->value;
			return true;
		}

		return false;
	}

private:
	PAIR_UNIQUE_INDEX_PATH_TYPE paths;
	std::size_t work_level;
	UNIQUE_NODE_PTR root;
	// 路径节点
};

// 构造结构化二叉树
// 传入当前频繁子树进入剪枝操作，返回裁剪过的关联二叉树

#endif // !__STRUCTURAL_BINARY_TREE_HPP__
