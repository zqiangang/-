#ifndef __EVENT_MODELS_FWD_HPP__
#define __EVENT_MODELS_FWD_HPP__

#include "enumTools.hpp"
#include "fwdtools.hpp"

// Ç°ÖÃÉùÃ÷
template <typename TKEY, typename TINDEX = uint32_t, BitWidth BWIDTH = BitWidth::OneBit, SectionWidth BSWIDTH = TryAlignBWMapSW_v<BWIDTH>,
	typename CHECK = std::enable_if_t<IsSecWidthLessBitWidth_v<BWIDTH,BSWIDTH>>>
class EventModelManage;


template <typename MANAGE, typename TVALUE = typename MANAGE::VALUE_TYPE, typename TKEY = typename MANAGE::KEY_TYPE, typename TINDEX = typename MANAGE::INDEX_TYPE,
	typename CHECK= std::enable_if_t<std::is_base_of_v<EventModelManage<TKEY, TINDEX, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
class EventModelSet;


template <typename MANAGE,
	typename = std::enable_if_t<std::is_base_of_v<EventModelManage<typename MANAGE::KEY_TYPE, typename MANAGE::INDEX_TYPE, MANAGE::BIT_WIDTH, MANAGE::BIT_SECTION_WIDTH>, MANAGE>>>
class EventModel;

#endif // !__EVENT_MODELS_FWD_HPP__
