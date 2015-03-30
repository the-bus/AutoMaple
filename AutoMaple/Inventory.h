#include "inc.h"
namespace maple
{
	struct item
	{
		item(std::size_t index, std::size_t item_id, std::size_t quantity)
			: index(index), item_id(item_id), quantity(quantity)
		{
		}

		std::size_t index;
		std::size_t item_id;
		std::size_t quantity;
	};
}

vector<vector<maple::item*>> get_inv();