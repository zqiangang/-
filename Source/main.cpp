#define EVENT_MODEL_DEBUG
#define PRINT_PATH
#include <iostream>
#include "baseTools.hpp"
#include "EventModel.hpp"
#include "SuperMarkEventModel.h"
#include "EventModelManage.hpp"
#include "EventModelSet.hpp"
#include "StructuralBTree.hpp"
#include "SuperMarkEventModel.h"
#include "EventPage.hpp"

// testc
#include <cstdlib>
#include <chrono>


template <typename MANAGE>
void TEST_DATE(int row, std::vector<EventModelSet<MANAGE>*> & econ)
{
	srand(time(NULL));
	for (int i = 0; i < row; ++i)
	{
		int col = rand() % 14 + 3;

		econ.push_back(new EventModelSet<SuperMarkEventModel>(col));			// 3 - 14
		//std::cout << "ROW : " << i << "RAND : " << col<< std::endl;

		for (int j = 0; j < col; ++j)
		{
			auto & ref = econ.back();
			int v = rand() % 20 + 1;
			//std::cout << std::setw(3) << v;
			ref->Append(v, j);
		}

		//std::cout << std::endl;
	}
}


template <typename MANAGE>
void TEST_DATE_TP(int row, EventModelSet<MANAGE>**  econ)
{
	srand(time(NULL));
	for (int i = 0; i < row; ++i)
	{
		int col = rand() % 14 + 3;

		econ[i] = new EventModelSet<SuperMarkEventModel>(col);			// 3 - 14
		//std::cout << "ROW : " << i << "RAND : " << col<< std::endl;

		for (int j = 0; j < col; ++j)
		{
			auto & ref = econ[i];
			for (int c = 0; c < MANAGE::EVENT_MODEL_MAX_ELEMENTS; ++c)
			{
				int v = rand() % 20 + 1;
				std::cout << std::setw(3) << v;
				ref->Append(v, j);
			}
			std::cout << ",";
		}
		std::cout << std::endl;
	}
}

template <typename MANAGE>
void PRINT_DATA(std::vector<EventModelSet<SuperMarkEventModel>*> const & econ)
{
	for (int i = 0; i < econ.size(); ++i)
	{
		//std::cout << "DEBUG ECON.SIZE() : " << econ.size() << std::endl;
		for (int j = 0; j < econ[i]->Count(); ++j)
		{
			//std::cout << "DEBUG ECON.COUNT() : " << econ[i]->Count() << std::endl;
			std::cout << std::setw(3) << (int)econ[i]->GetEventModel(j).value[0];
		}

		std::cout << std::endl;
	}
}

template <typename MANAGE>
void PRINT_DATA_TP(int row, EventModelSet<MANAGE>**  econ)
{
	for (int i = 0; i < row; ++i)
	{
		//std::cout << "DEBUG ECON.SIZE() : " << econ.size() << std::endl;
		for (int j = 0; j < econ[i]->Count(); ++j)
		{
			std::cout << "{";
			//std::cout << "DEBUG ECON.COUNT() : " << econ[i]->Count() << std::endl;
			for (int c = 0; c < MANAGE::EVENT_MODEL_TVALUE_COUNT; ++c)					// TVALUE ����
			{
				std::size_t element_count = (BitCountOfValueType_v<typename MANAGE::MASK_TYPE> / static_cast<int>(MANAGE::BIT_SECTION_WIDTH));
				std::size_t max_element = element_count > MANAGE::EVENT_MODEL_MAX_ELEMENTS
				? MANAGE::EVENT_MODEL_MAX_ELEMENTS : element_count;
				for (int e = 1; e <= max_element; ++e)
				{
					auto mask_value = MANAGE::BIT_MASK;
					int move_step = (BitCountOfValueType_v<typename MANAGE::MASK_TYPE>
						-e * static_cast<typename MANAGE::MASK_TYPE>(MANAGE::BIT_SECTION_WIDTH));
					mask_value <<= move_step;
					auto v = (typename MANAGE::MASK_TYPE)econ[i]->GetEventModel(j).value[c];
					mask_value &= v;
					mask_value >>= move_step;
					mask_value &= MANAGE::BIT_MASK;
					std::cout << std::setw(3) << (int)mask_value;
					if (e != ((BitCountOfValueType_v<typename MANAGE::MASK_TYPE>) / (static_cast<typename MANAGE::MASK_TYPE>(MANAGE::BIT_SECTION_WIDTH))))
					{
						std::cout << ",";
					}
					
				}
			}
			std::cout << "},";
		}

		std::cout << std::endl;
	}
}

int main(int argc, char * argv[])
{
	// �¼�ģ�͵��ڴ沼��
	std::cout << "\neventModel ���ڴ�洢����\n" << std::endl;
	EventModel<SuperMarkEventModel> spm;
	spm.MemoryStyle();

	std::size_t eventSize = 8;

	// �հ׵� Page
	std::cout << "\n�հ�Page\n" << std::endl;
	EventPage<SuperMarkEventModel> epage(eventSize);
	epage.ShowDatas();
	
	auto manage = InstanceManage<SuperMarkEventModel>();

	// ������ݵ���ʱ�洢
	std::vector<EventModelSet<SuperMarkEventModel>*> econ;
	EventModelSet<SuperMarkEventModel>** econ2 = new EventModelSet<SuperMarkEventModel>*[manage->GetEventTypeCount()]{nullptr};
	
	// ��������
	std::cout << "\n���ɲ�������\n" << std::endl;
	TEST_DATE_TP(eventSize,econ2);

	// ��������
	std::cout << "\n������������\n" << std::endl;
	PRINT_DATA_TP(eventSize, econ2);

	epage.Push(econ2,eventSize);				// ��������
	std::cout << "\nչʾPage������\n" << std::endl;
 	epage.ShowDatas();					// չʾ���ݵĴ洢����

	auto v = epage.FreqTrees(0.3f);		// Ƶ������

	auto rsv = InstanceManage<SuperMarkEventModel>();		// ʵ����  manage  �����п���
	StructuralBTree<SuperMarkEventModel> s(rsv->GetIndexList().get(),rsv->GetEventTypeCount());	// ��ʼ���ṹ������

	// ��ӡ��ʾƵ������
	std::cout << "\nƵ������\n" << std::endl;
	for (int i = 0; i < v.first; ++i)
	{
		//v.second[i].UniquePool();
		v.second[i].Print(true);
		if (i % 5 == 0 && i != 0)
			std::cout << std::endl;
	}

	std::cout << std::endl;
	std::cout << "\n�ü���,���й�����ϵ��Ƶ������\n" << std::endl;
	try
	{
		auto test = s.cut(2, v);

		epage.RowSum(test);

		for (int i = 0; i < test.first; ++i)
		{
			test.second[i].Print(true);
			if (i % 5 == 0 && i != 0)
				std::cout << std::endl;
		}
	}
	catch (const std::exception&e)
	{
		std::cout << e.what() << std::endl;
		std::cout << "----------------------------------------------------------catch expection" << std::endl;
	}

	std::cout << std::endl;


	//std::cout << FrequentTree<SuperMarkEventModel>::idx_pool_count << std::endl;

	//epage.ShowDatas();
	/*sp.ShowDatas();
	std::cout << "MANAGE BIT MASK :" << (int)SuperMarkEventModel::BIT_MASK << std::endl;
	
	std::cout << "Capacity : " <<  ep.Capacity() << std::endl;
	std::cout << "Count : " << ep.Count() << std::endl;


	for (int j = 0; j < 128; ++j) {
		ep.Append(j, j);
	}

	 
	std::cout << "Capacity : " << ep.Capacity() << std::endl;
	std::cout << "Count : " << ep.Count() << std::endl;
	
	for (int i = 0; i < ep.Count(); ++i)
	{
		auto tp = ep.GetEventModel  (i);
		std::cout << tp.index << " " << (int)tp.value[0] << std::endl;
		std::cout << typeid(tp.index).name() << " " <<  typeid(tp.value).name() << std::endl;
	} 

	__int64 a;
	__int8 d;
	__int16 c;
	__int32 v;
	
	std::cout << "__int16" << sizeof(__int16) << "__int32 " << sizeof(__int32) << " int_8" << sizeof(__int8) << "  __int64 " << sizeof(__int64) << std::endl;
		
*/
	system("pause");
	return 0;
}