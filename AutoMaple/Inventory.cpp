//https://wecodez.com/threads/source-inventory-browser.351/

#include "inc.h"

#define Padding(x) struct { unsigned char __padding##x[(x)]; };

template <typename T>
struct ZArray
{
	T *a;
};

template <typename T>
struct ZRef
{
	Padding(0x04);
	T* p;

public:
	ZRef()
	{
		p = new T;
	}

	~ZRef()
	{
		p = NULL;
	}
};

template <typename T>
struct TSecData
{
	T data;
	char bKey;
	char FakePtr1;
	char FakePtr2;
	unsigned short wChecksum;
};

template <typename T>
struct TSecType
{
	unsigned int FakePtr1;
	unsigned int FakePtr2;
	TSecData<T>* m_secdata;
};

#pragma pack(push, 1)
struct ZRefCountedVtbl
{
	Padding(0x08);
	unsigned int(__thiscall* get_item_count_1)(void* ecx); //+ 0x08
	Padding(0x3C);
	unsigned int(__thiscall* get_item_count_2)(void* ecx); //+ 0x48
};

struct ZRefCounted
{
	ZRefCountedVtbl* vfptr;

	union
	{
		int ref;
		ZRefCounted* next;
	};

	ZRefCounted* prev;
};

struct GW_ItemSlotBase
{
	ZRefCounted baseclass;                                //+ 0x00
	TSecType<long> nItemID;                               //+ 0x0C
	LARGE_INTEGER liCastItemSN;                           //+ 0x18
	FILETIME dateExpire;                                  //+ 0x20
	Padding(0x14);                                        //+ 0x28
};

struct CharacterData
{
	Padding(0x000006C7); //8B 84 81 ? ? ? ? 85 C0 75
	ZArray<ZRef<GW_ItemSlotBase>> aaItemSlot[5];
};
#pragma pack(pop)

namespace maple
{
	enum class inventory_type
	{
		equip = 1,
		use = 2,
		etc = 4,
		setup = 3,
		cash = 5
	};

	class inventory
	{
	public:
		inventory(unsigned int CWvsContext, unsigned int CWvsContext__GetCharacterData, unsigned int TSecType__GetData)
			: CWvsContext(reinterpret_cast<void* const>(CWvsContext))
		{
			this->CWvsContext__GetCharacterData = reinterpret_cast<CWvsContext__GetCharacterData_t>(CWvsContext__GetCharacterData);
			this->TSecType__GetData = reinterpret_cast<TSecType__GetData_t>(TSecType__GetData);
		}

		~inventory(){}

		bool update()
		{
			ZRef<CharacterData> ref_characterdata;
			if (!CWvsContext__GetCharacterData(*reinterpret_cast<void**>(this->CWvsContext), &ref_characterdata))
				return false;

			this->inventories = ref_characterdata.p->aaItemSlot;
			return true;
		}

		std::size_t get_size(inventory_type type)
		{
			ZRef<GW_ItemSlotBase>* inventory = this->inventories[static_cast<int>(type)].a;
			return reinterpret_cast<std::size_t>(inventory[-1].p) - 1;
		}

		std::vector<item*> get_items(inventory_type type)
		{
			std::vector<item*> items;
			std::size_t size = this->get_size(type);

			ZRef<GW_ItemSlotBase>* inventory = this->inventories[static_cast<int>(type)].a;

			for (std::size_t i = 1; i < (size + 1); i++)
			{
				ZRef<GW_ItemSlotBase> slot = inventory[i];
				items.push_back(new item(i, this->get_item_id(slot), this->get_quantity(slot)));
			}

			return items;
		}

		std::size_t get_item_id(ZRef<GW_ItemSlotBase> slot)
		{
			return (slot.p ? TSecType__GetData(&slot.p->nItemID) : 0);
		}

		std::size_t get_quantity(ZRef<GW_ItemSlotBase> slot)
		{
			if (!slot.p)
				return 0;

			std::size_t quantity = slot.p->baseclass.vfptr->get_item_count_1(slot.p);

			if (!quantity)
				quantity = slot.p->baseclass.vfptr->get_item_count_2(slot.p);

			return quantity;
		}

	private:
		void* const CWvsContext;

		typedef ZRef<CharacterData>* (__thiscall* CWvsContext__GetCharacterData_t)(void* ecx, ZRef<CharacterData>* result);
		CWvsContext__GetCharacterData_t CWvsContext__GetCharacterData;

		typedef unsigned int(__thiscall* TSecType__GetData_t)(void* lpvEcx);
		TSecType__GetData_t TSecType__GetData;

		ZArray<ZRef<GW_ItemSlotBase>>* inventories;
	};
}

void get_inv(vector<maple::item*> tabs[5])
{
	unsigned int CWvsContext = ServerBase; //8B 0D ? ? ? ? E8 ? ? ? ? 8B 86 ? ? ? ? 33 [in MOV]
	unsigned int GetCharacterData = 0x004A22F0; //51 8B 81 ? ? ? ? 56 8B 74 24 ? C7 44 24 ? 00 00 00 00 89 46 [1st Result]
	unsigned int TSecType_GetData = 0x004CC3B0; //E8 ? ? ? ? 8B CE 8B F8 E8 ? ? ? ? 8B C8 8B [Any Result, in CALL]


	maple::inventory inventory(CWvsContext, GetCharacterData, TSecType_GetData);

	if (!inventory.update())
	{
		return;
		//printf("Failed to update Inventory Data!");
		//::FreeLibraryAndExitThread(hInstance, EXIT_FAILURE);
	}

	for (int inv_id = 1; inv_id < 6; inv_id++)
	{
		std::vector<maple::item*> items = inventory.get_items(static_cast<maple::inventory_type>(inv_id));

		tabs[inv_id - 1] = items;

		/*if (items.empty())
			continue;

		for (auto x : items)
		{
			if (x != 0)
				if (x->quantity > 0)
					printf("Index: %d, ID: %d, Quantity: %d\n", x->index, x->item_id, x->quantity);
		}*/
	}
	return;
}