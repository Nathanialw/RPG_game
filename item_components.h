#pragma once
#include <SDL2/SDL.h>
#include "sstream"


namespace Item_Component {

	enum class Item_Type {
        amulet,
        legs,
        belt,
        wrist,
        boots,
        kilt,
        hair,
        clothes,
        chest,
        gloves,
        ring,
        face,
        helm,
        crown,
        hood,
        shins,
        back,
		shoulders,
        jewelry,
        dirt,
        ranged,
        mainhand,
        offhand
	};

    struct Item_Pickup {

    };

    struct Equip_Items_Type{
    };

    entt::entity emptyEquipSlot;

    struct Equipment {
        std::string type = "none";
        std::map<Item_Type, entt::entity>equippedItems{
            { Item_Type::amulet, emptyEquipSlot },
            { Item_Type::back, emptyEquipSlot },
            { Item_Type::face, emptyEquipSlot },
            { Item_Type::belt, emptyEquipSlot },
            { Item_Type::wrist, emptyEquipSlot },
            { Item_Type::boots, emptyEquipSlot },
            { Item_Type::crown, emptyEquipSlot },
            { Item_Type::dirt, emptyEquipSlot },
            { Item_Type::hair, emptyEquipSlot },
            { Item_Type::helm, emptyEquipSlot },
            { Item_Type::hood, emptyEquipSlot },
            { Item_Type::legs, emptyEquipSlot },
            { Item_Type::kilt, emptyEquipSlot },
            { Item_Type::gloves, emptyEquipSlot },
            { Item_Type::clothes, emptyEquipSlot },
            { Item_Type::chest, emptyEquipSlot },
            { Item_Type::ring, emptyEquipSlot },
            { Item_Type::shins, emptyEquipSlot },
            { Item_Type::shoulders, emptyEquipSlot },
            { Item_Type::jewelry, emptyEquipSlot },
            { Item_Type::ranged, emptyEquipSlot },
            { Item_Type::offhand, emptyEquipSlot },
            { Item_Type::mainhand, emptyEquipSlot },
        };
    };


    enum class Armor_Type {
		cloth,
		padded,
		leather,
		mail,
		plate
	};

	enum class Weapon_Type {
		sword,
        axe,
		mace,
        spear
	};

    enum class Weapon_Material {
        copper,
        bronze,
        iron,
    };

	struct Ground_Item {
		SDL_FRect box = {};
        SDL_FRect ground_name = {};
	};

    struct Update_Ground_Item {

    };

	struct Name {
		std::string name;
	};

	enum class Rarity {
		common,		//white
		magic,		//blue
		rare,		//yellow
		unique		//gold
	};

	struct Item_Equip {

	};

	struct Weapon_Damage {
		int min;
		int max;
	};


	struct statRange {
		int min;
		int max;
	};

	//This is the order that it will show the stats on the item
	enum class Stat {
		armor,
		damage,
		spellDamage,
		health,
		piety,
		attackSpeed
	};

	struct statValue {
		Stat name;
		int value;
	};

	struct Item_Stats {;
		std::map<Stat, int>stats = {

		};
	};
}

