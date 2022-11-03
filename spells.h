#pragma once
#include "movement.h"
#include "collision.h"
#include "combat_control.h"

namespace Spells {

	void Spell_Move_Target(entt::entity entity, float& x, float& y) { //sends spell to where the mouse is
		World::zone.emplace<Component::Moving>(entity);
		World::zone.emplace<Component::Mouse_Move>(entity, x, y);
		World::zone.remove<Component::Casted>(entity);
	}

	void Spell_Linear_Target(entt::entity& entity, float& x, float& y, float& sourceX, float& sourceY) { //sends spell to where the mouse is
		World::zone.emplace<Component::Moving>(entity);
		World::zone.emplace<Component::Linear_Move>(entity, x, y);
		World::zone.emplace<Component::Spell_Range>(entity, sourceX, sourceY, 0.0f);
		World::zone.remove<Component::Casted>(entity);
	}

	void Spell_Stack_Spells(float& x, float& y) { //sends spell to where the mouse is
		auto view = World::zone.view<Component::Spell>();
		for (auto entity : view) {
			World::zone.emplace_or_replace<Component::Moving>(entity);
			World::zone.emplace_or_replace<Component::Mouse_Move>(entity, x, y);
		}
	}

	DataTypes::f2d Spell_Direction(DataTypes::f2d& pos, Component::Direction& direction, float& scale) {
		switch (direction) {
		case Component::Direction::N: return { pos.x, pos.y - (20.0f * scale) };
		case Component::Direction::S: return { pos.x, pos.y + (20.0f * scale) };
		case Component::Direction::E: return { pos.x + (20.0f * scale) , pos.y };
		case Component::Direction::W: return { pos.x - (20.0f * scale) , pos.y };
		case Component::Direction::NW: return { pos.x - (20.0f * scale) , pos.y - (20.0f * scale)  };
		case Component::Direction::NE: return { pos.x + (20.0f * scale) , pos.y - (20.0f * scale)  };
		case Component::Direction::SW: return { pos.x - (20.0f * scale) , pos.y + (20.0f * scale)  };
		case Component::Direction::SE: return { pos.x + (20.0f * scale) , pos.y + (20.0f * scale)  };
		}
		Utilities::Log("Spell_Direction() function fallthrough error");
		return { 1.0f, 1.0f };
	}

	void create_spell(entt::entity caster_ID, entt::entity& entity, DataTypes::f2d& pos, Component::Direction& direction, const char* spellname, float& targetX, float& targetY) {
		float scale = 1.0f;
		Entity_Loader::Data data = Entity_Loader::parse_data(spellname);
		DataTypes::f2d spelldir = Spell_Direction(pos, direction, scale);
		//rendering data
		World::zone.emplace<Component::animation>(entity, Graphics::fireball_0); /// need to load the texture /only /once and pass the pointer into this function
		World::zone.get<Component::animation>(entity).sheet = { //populate the vector
			{ NULL },
			{ {0, 0, 64, 64 }, 0, 512, 0, 0, 16 }, //idle
			{ {0, 0, 64, 64 }, 0, 512, 0, 0, 16 } //walk
		};
		World::zone.emplace<Component::Sprite_Offset>(entity, 32.0f * scale, 32.0f * scale);
		World::zone.emplace<Component::Scale>(entity, scale);

		World::zone.emplace<Component::Actions>(entity, Component::walk);
		World::zone.get<Component::Actions>(entity).frameCount = { {0, 0}, {0, 0}, {8, 0} };
		//positon data
		World::zone.emplace<Component::Position>(entity, spelldir.x, spelldir.y);
		//spell data
		World::zone.emplace<Component::Radius>(entity, data.radius * scale);
		World::zone.emplace<Component::Velocity>(entity, 0.f, 0.0f, 0.0f, 0.0f, data.speed);
		World::zone.emplace<Component::Mass>(entity, data.mass * scale);
		World::zone.emplace<Component::Entity_Type>(entity, Component::Entity_Type::spell);
		World::zone.emplace<Component::Damage>(entity, 1, 10);
		//Scenes::scene.emplace<Spell_Range>(spell, 1000.0f);
		World::zone.emplace<Component::Caster_ID>(entity, caster_ID);
		//default data
		World::zone.emplace<Component::Spell>(entity);
		World::zone.emplace<Component::Casted>(entity);
		World::zone.emplace<Component::Renderable>(entity);
		World::zone.emplace<Component::Direction>(entity, direction); //match Direction of the caster
		World::zone.emplace<Component::Alive>(entity, true);
        World::zone.emplace<Component::Caster>(entity, caster_ID);
		bool yes = true;
		Collision::Create_Dynamic_Body(World::zone, entity, spelldir.x, spelldir.y, data.radius, data.mass, yes);
		Spell_Linear_Target(entity, targetX, targetY, spelldir.x, spelldir.y);
		//Spell_Move_Target(spell, targetX, targetY);
		//std::cout << "casted " << scene.get<handle>(spell).sName << std::endl;
	}

	void create_fireball(entt::entity & caster_ID, float& x, float& y, Component::Direction& direction, const char* spellname, float& targetX, float& targetY) {
		auto fireball = World::zone.create();
		DataTypes::f2d pos = { x, y };
		create_spell(caster_ID, fireball, pos, direction, spellname, targetX, targetY);
	}

	void cast_fireball() {
		auto view = World::zone.view<Component::Direction, Component::Actions, Component::Position, Component::Cast, Component::Spell_Name, Component::Velocity>();
		for (auto entity : view) {
			auto& act = view.get<Component::Actions>(entity);
			act.action = Component::cast;
			act.frameCount[act.action].currentFrame = 0;
			auto& target = view.get<Component::Cast>(entity);
			auto& direction = view.get<Component::Direction>(entity);
			auto& x = view.get<Component::Position>(entity).x;
			auto& y = view.get<Component::Position>(entity).y;
			auto& angle = view.get<Component::Velocity>(entity).angle;
			auto& name = view.get<Component::Spell_Name>(entity).spell;
			//look at target
			direction = Movement::Look_At_Target(x, y, target.targetX, target.targetY, angle);
			//cast Fireball
			create_fireball(entity, x, y, direction, name, target.targetX, target.targetY);		
			//set into casting mode
			World::zone.emplace_or_replace<Component::Casting>(entity);
			World::zone.remove<Component::Cast>(entity);
		}
	}

	void add_spells_to_scene() {
		cast_fireball();
	}

	void Create_Explosion(float& x, float y) { //creates the explosion for fireballs
		auto explosion = World::zone.create();
		World::zone.emplace<Component::Position>(explosion, x, y);
		World::zone.emplace<Component::Sprite_Frames>(explosion, 63, 0, 0, 0);
		World::zone.emplace<Component::Texture>(explosion, Graphics::fireball_explosion_0, 0, 0, 128, 128);
		World::zone.emplace<Component::Frame_Delay>(explosion, 16.0f, 0.0f);
		World::zone.emplace<Component::Explosion>(explosion, 0, 0, 0.0f, 0.0f, 128.0f, 128.0f, 64.0f, 100.0f);
	}

	void Destroy_NonMoving_Spells() {
		auto view = World::zone.view<Component::Spell, Component::Body, Component::Position, Component::Radius, Component::In_Object_Tree>(entt::exclude<Component::Mouse_Move, Component::Linear_Move, Component::Explosion>);
		for (auto entity : view) {
			auto& position = view.get<Component::Position>(entity);
			auto& radius = view.get<Component::Radius>(entity).fRadius;
			auto& body = view.get<Component::Body>(entity);
			auto& inTree = view.get<Component::In_Object_Tree>(entity).inTree;
			//create explosion
			Create_Explosion(position.x, position.y);
			//destroy box2d body
			Collision::world->DestroyBody(body.body);
			World::zone.remove<Component::Body>(entity);
			//Collision::world->DestroyBody(body.rigidBody);
			// 
			//set to remove from quad tree on update
			SDL_FRect rect = Utilities::Get_FRect_From_Point_Radius(radius, position.x, position.y);
			World::zone.emplace<Component::Remove_From_Object_Tree>(entity, rect); //goto: Dynamic_Quad_Tree::Remove_From_Tree_And_Registry()
			World::zone.emplace<Component::Destroyed>(entity, rect); //goto: Dynamic_Quad_Tree::Remove_From_Tree_And_Registry()
		}
	}

	void Clear_Collided_Spells() {
		auto view = World::zone.view<Component::Spell, Component::Position, Component::Alive, Component::Body, Component::Radius, Component::In_Object_Tree>(entt::exclude<Component::Mouse_Move, Component::Linear_Move, Component::Explosion>);
		for (auto entity : view) {
			if (view.get<Component::Alive>(entity).bIsAlive == false) {
				auto& position = view.get<Component::Position>(entity);
				auto& radius = view.get<Component::Radius>(entity).fRadius;
				auto& body = view.get<Component::Body>(entity);
				auto& inTree = view.get<Component::In_Object_Tree>(entity).inTree;
				//create explosion
				Create_Explosion(position.x, position.y);
				//destroy box2d body
				Collision::world->DestroyBody(body.body);
				World::zone.remove<Component::Body>(entity);
				SDL_FRect rect = Utilities::Get_FRect_From_Point_Radius(radius, position.x, position.y);
				//set to remove from quad tree on update
				World::zone.emplace<Component::Remove_From_Object_Tree>(entity, rect); //goto: Dynamic_Quad_Tree::Remove_From_Tree_And_Registry()
				World::zone.emplace<Component::Destroyed>(entity, rect); //goto: Dynamic_Quad_Tree::Remove_From_Tree_And_Registry()
			}
		}
	}
	
	void Casting_Updater() {
		auto view = World::zone.view<Component::Casting, Component::Actions>();
		for (auto entity : view) {
			auto& act = view.get<Component::Actions>(entity);
			//std::cout << act.frameCount[act.action].currentFrame << "/" << act.frameCount[act.action].NumFrames <<std::endl;
			if (act.action != Component::dead) {
				if (act.frameCount[act.action].currentFrame >= act.frameCount[act.action].NumFrames - 1) {
					act.action = Component::idle;
					World::zone.remove<Component::Casting>(entity);
				}
			}
		}
	}

    void Spell_Hit (entt::entity spell_ID, entt::entity struck_ID) {
        Component::Damage damageRange = World::zone.get<Component::Damage>(spell_ID);
        //std::cout << damageRange.minDamage << ", " << damageRange.maxDamage << std::endl;
        int damage = Combat_Control::Calculate_Damage(damageRange);
        entt::entity player;
        auto view = World::zone.view<Component::Input>();
        for (auto input : view) {
            player = input;
        }
        if (World::zone.get<Component::Caster>(spell_ID).caster == player) {
            Damage_Text::Add_To_Scrolling_Damage(World::zone, spell_ID, struck_ID, damage);
        }
        //std::cout << damage << std::endl;
        auto& struck = World::zone.get_or_emplace<Component::Struck>(struck_ID);
        struck.struck += damage;
    }

    void Check_Spell_Collide () {
        auto view = World::zone.view<Component::Spell, Component::Radius, Component::Position, Component::Alive, Component::Caster>();
        for (auto entity: view) {
            auto &alive = view.get<Component::Alive>(entity).bIsAlive;
            if (alive) {
                auto &radius = view.get<Component::Radius>(entity).fRadius;
                auto &position = view.get<Component::Position>(entity);
                auto &caster_ID = World::zone.get<Component::Caster>(entity).caster;

                SDL_FRect spellRect = Utilities::Get_FRect_From_Point_Radius(radius, position.x, position.y);
                Dynamic_Quad_Tree::Entity_Data targetData = Dynamic_Quad_Tree::Entity_vs_QuadTree_Collision(World::zone,
                                                                                                            spellRect);

                //prevent spell from hitting itself or it's caster or a ground item
                if (targetData.b == true && caster_ID != targetData.entity_ID && targetData.entity_ID != entity && World::zone.any_of<Ground_Item>(targetData.entity_ID) == false) {
                    alive = false;
                    Spell_Hit(entity, targetData.entity_ID);
                    World::zone.remove<Component::Linear_Move>(entity);
                    World::zone.remove<Component::Mouse_Move>(entity);
                }
            }
        }
    }

	void Update_Spells() {
        Destroy_NonMoving_Spells();
		Clear_Collided_Spells();
		Casting_Updater();
        Check_Spell_Collide();
		add_spells_to_scene();
	}
}


