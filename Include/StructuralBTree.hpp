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
	using PAIR_INDEX_PATHS_TYPE = std::pair<std::size_t, std::unique_ptr<INDEX_TYPE[], std::default_delete<INDEX_TYPE[]>>>;		// ·��������·���ڵ������
	using PAIR_UNIQUE_INDEX_PATH_TYPE = std::pair<std::size_t,std::unique_ptr<PAIR_INDEX_PATHS_TYPE[], std::default_delete<PAIR_INDEX_PATHS_TYPE[]>>>;		//count ,����·����������
	using FREQ_TREE_TYPE = FrequentTree<MANAGE>;			// Ƶ��������
	using UNIQUE_FREQ_TREE_PTR = std::unique_ptr<FREQ_TREE_TYPE[], std::default_delete<FREQ_TREE_TYPE[]>>;
	using PAIR_COUNT_FREQ_LIST_TYPE = std::pair<std::size_t, UNIQUE_FREQ_TREE_PTR>;
public:
	StructuralBTree()
		:work_level(1), root(new NODE_TYPE())
	{}

	// ���������¼�����
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

		// ����鿴
		if (Push(root->left.get(), idx, false, current_level + 1))
			return true;

		// ���Ҳ鿴 
		return Push(root->right.get(), idx, stat, current_level + 1);
	}

	// �������е�Ƶ������
	//void Distance
	PAIR_COUNT_FREQ_LIST_TYPE cut(std::size_t distance, PAIR_COUNT_FREQ_LIST_TYPE const & pair_freq_list)
	{
		// �������
		pair_freq_list.second[0].ClearUniquePool();
		// Ȼ������е�Ƶ���������� unique_pool ��ȡ���п��ܵ�����
		for (int i = 0; i < pair_freq_list.first; ++i)
		{
			pair_freq_list.second[i].UniquePool();
		}
		// ��������·����
		// �洢����Ƶ�������г��ֵĵ�����������
		paths.first = pair_freq_list.second[0].idx_pool_count;				// ��ʼ������
		paths.second.reset(new PAIR_INDEX_PATHS_TYPE[paths.first]);
		// �����������
		int bt_depth = static_cast<int>(log2f(InstanceManage<MANAGE>()->GetEventTypeCount()));
		bt_depth += 1;														// ���һ�� index λ��
		// �����С 
		for (int i = 0; i < paths.first; ++i)
		{
			paths.second[i].first = 1;
			paths.second[i].second.reset(new INDEX_TYPE[bt_depth]);
			paths.second[i].second[0] = pair_freq_list.second[0].unique_idx_pool[i];			// �׸�λ�ô����������ȥλ�ô��·��
		}

		//// ����·���ռ�,·���ռ���ո�Ԫ�ش洢��Ӧ������
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



		// ���ü���֦Ҷ
		std::unique_ptr<INDEX_TYPE[],std::default_delete<INDEX_TYPE[]>> cuted(new INDEX_TYPE[pair_freq_list.second[0].idx_pool_count]);
		std::size_t cuted_count = 0;

		//�ü�Ƶ������
		std::vector<FREQ_TREE_TYPE*> tmpFreq;
		for (int i = 0; i < pair_freq_list.first; ++i)
		{
			if (pair_freq_list.second[i].GetLevel() == 1)					// ���������޷��ü�
				continue;
			// ������� unique pool
			pair_freq_list.second[i].ClearUniquePool();
			// �ѵ�ǰƵ�������������Ӽ��������õ� unique pool
			pair_freq_list.second[i].UniquePool();

			// �����е� 2 2 ���
			auto tmp_k2 = GroupSplit<MANAGE, PAIR_COUNT_FREQ_LIST_TYPE, INDEX_TYPE>(pair_freq_list.second[i].unique_idx_pool.get(), pair_freq_list.second[i].idx_pool_count, 2);
			
			// ������е� k2
			for (int j = 0; j < tmp_k2.first; ++j)
			{
				auto index1 = tmp_k2.second[j].Get(0);
				auto index2 = tmp_k2.second[j].Get(1);
				// ���Ҷ�Ӧ��·������
				bool stat = false;
				for (int k = 0; k < paths.first; ++k)
				{
					if (paths.second[k].second[0] == index1)
					{
						index1 = k;
						if (stat)					// �Ѿ�Ϊ true ֱ�ӷ���
							break;
						else
							stat = true;
						continue;
					}
					if (paths.second[k].second[0] == index2)
					{
						index2 = k;				// �Ѿ�Ϊ true ֱ�ӷ���
						if (stat)
							break;
						else
							stat = true;
					}
				}
				// 
				stat = false;       // �������ڷ�ִֹ��������
				// �ȶ�·��
				std::size_t path_distance = 0;
				std::size_t max_path_distance = paths.second[index1].first > paths.second[index2].first ? paths.second[index1].first : paths.second[index2].first;
				std::size_t min_path_distance = paths.second[index1].first > paths.second[index2].first ? paths.second[index2].first : paths.second[index1].first;

					// ��·����ȫ����С·�� 
				if (paths.second[index1].second[min_path_distance - 1] == paths.second[index2].second[min_path_distance - 1] && !stat)		// �϶�·�������һ��·���ͳ�·����·���ڵ㸴��
				{	// ·���غϳ�·���Ͷ�·���Ĳ�ֵ - 1 Ϊ·������
					path_distance = max_path_distance - min_path_distance - 1;
					stat = true;
				}

				// ����·����ȫ�޹�
				if (paths.second[index1].second[1] == paths.second[index2].second[1] && paths.second[index1].second[2] != paths.second[index2].second[2] && !stat)
				{
					path_distance = paths.second[index1].first + paths.second[index2].first - 6;      //�������ʼ�ڵ����
					stat = true;
				}

				// ��·�����ְ���С·��
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

				// �Ѵ���ָ���������������ӵ� cuted ����
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

					if (!has_index1)					// ��� cuted �в����� index1 ��� index1 ��������
					{
						cuted[cuted_count] = index1;
						++cuted_count;
					}

					if (!has_index2)					// ��� cuted �в����� index2 ��� index2 ��������
					{
						cuted[cuted_count] = index2;
						++cuted_count;
					}
				}
			}
			// ���� cuted ���� �� unique_idx_pool �ȶ�,���в��� cutted ���ӽڵ����¹���һ��Ƶ��������
			// ��� ����һ��Ԫ�� �򲻽���Ƶ���������Ĺ���
			if (pair_freq_list.second[0].idx_pool_count - cuted_count <= 1)
			{
				cuted_count = 0;
				continue;
			}

			else
			{
				// ����һ��Ƶ����,���� level Ϊ idx_pool_count ��  cuted_count �Ĳ�ֵ
				tmpFreq.push_back(new FrequentTree<MANAGE>(pair_freq_list.second[0].idx_pool_count - cuted_count));
				// ����׷��
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
					if (!is_exist)				// ��������ڣ���Ѹ�����λ������������
					{
						tmpFreq.back()->Push(pair_freq_list.second[0].unique_idx_pool[ipc]);
					}
				}
			}

			cuted_count = 0;			// ���òü����鳤��
		}

		PAIR_COUNT_FREQ_LIST_TYPE ret;
		ret.first = tmpFreq.size();
		ret.second.reset(new FrequentTree<MANAGE>[ret.first]);

		// �������ݵ�����ֵ
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
	// ·���ڵ�
};

// ����ṹ��������
// ���뵱ǰƵ�����������֦���������زü����Ĺ���������




#endif // !__STRUCTURAL_BINARY_TREE_HPP__
