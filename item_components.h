#pragma once
#include <SDL2/SDL.h>
#include "sstream"


namespace Item_Component {


	enum class Item_Type {
		helm,
		neck,
		shoulders,
		chest,
		weapon,
		gloves,
		belt,
		legs,
		boots,
		shield
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
		mace,
		axe
	};


	struct Ground_Item {
		SDL_FRect box;
        SDL_FRect ground_name;
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

