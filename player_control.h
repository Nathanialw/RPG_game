#pragma once
#include "timer.h"
#include "entity_control.h"
#include "components.h"
#include "player_components.h"

namespace Player_Control {
	void Attack_Order(entt::registry& zone, entt::entity& entity, entt::entity &target_ID,  Component::Position &targetPosition, Component::Radius &targetRadius) {
		zone.emplace_or_replace<Component::Moving>(entity);
		zone.emplace_or_replace<Player_Component::Attack_Move>(entity, target_ID, targetPosition.x, targetPosition.y, targetRadius.fRadius);
	}

    int64_t Player_Move_Poll = 0;
	void Mouse_Move_To_Attack(entt::registry &zone) { //calculates unit direction after you give them a "Mouse_Move" component with destination coordinates
		Player_Move_Poll += Timer::timeStep;
		if (Player_Move_Poll >= 200) {
			Player_Move_Poll = 0;
			auto view = zone.view<Component::Position, Component::Velocity, Component::Actions, Component::Moving, Player_Component::Attack_Move>();
			for (auto entity : view) {
				const auto& position = view.get<Component::Position>(entity);
				auto& act = view.get<Component::Actions>(entity);
				auto& v = view.get<Component::Velocity>(entity);
				auto& moveTo = view.get<Player_Component::Attack_Move>(entity);
				act.action = Component::walk;
				v.magnitude.x = v.speed * (moveTo.x - position.x);
				v.magnitude.y = v.speed * (moveTo.y - position.y);
			}
		}
	}

	void Mouse_Move_Arrived_Attack_Target(entt::registry& zone) {
		auto view = zone.view<Component::Position, Component::Velocity, Component::Actions, Player_Component::Attack_Move, Component::Moving, Component::Melee_Range>();
		for (auto entity : view) {
			auto& act = view.get<Component::Actions>(entity);
			auto& v = view.get<Component::Velocity>(entity);
			auto& position = view.get<Component::Position>(entity);
			auto& target = view.get<Player_Component::Attack_Move>(entity);
            //check if the target is in attack range
            auto& meleeRange = view.get<Component::Melee_Range>(entity);
            Position targetPosition = {target.x, target.y};
            Radius targetRadius;
            targetRadius.fRadius = target.hitRadius;
			if (Entity_Control::Target_In_Melee_Range(zone, position, meleeRange, targetPosition, targetRadius)) {
                //attack target
                Position targetPosition = { target.x, target.y };
                Entity_Control::Melee_Attack(zone, entity, target.ID, targetPosition);
                zone.remove<Player_Component::Attack_Move>(entity);
                zone.remove<Component::Moving>(entity);
			}
            else {
                //Update target position to move to
                auto &targetPosition = zone.get<Position>(target.ID);
                target.x = targetPosition.x;
                target.y = targetPosition.y;
            }
		}
	}

	void Move_To_Atack_Routine(entt::registry & zone) {
		Mouse_Move_To_Attack(zone);
		Mouse_Move_Arrived_Attack_Target(zone);
	}
}
