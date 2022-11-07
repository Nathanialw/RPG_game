#pragma once
#include <SDL2/SDL.h>
#include "ui.h"
#include "timer.h"
#include "utilities.h"
#include "unit_control.h"
#include "movement.h"
#include "death_spells.h"
#include "spells.h"
#include "weapons.h"
#include "ai_control.h"
#include "input_control.h"


namespace Event_Handler {

	/*I will make a component that will be pased to this funtion tree so the functions can do work on the position value of an entity "<velocity> <player_controllable>" */

	const Uint8* state = SDL_GetKeyboardState(NULL);
	SDL_Event event;

	void Movement_Input(entt::registry& zone, Component::Velocity& vel, Component::Action& act, entt::entity entity) { //can return bools for x and y dir, and 2 enums for direction and state
		if (event.key.repeat == 0) {
			if (event.type == SDL_KEYDOWN) {
				if (act.state == Component::idle || act.state == Component::walk) {
					if (event.type == SDL_KEYDOWN) {
						auto& position = zone.get<Component::Position>(entity);
						switch (event.key.keysym.sym) {
						case SDLK_e:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y -= vel.speed; act.state = Component::walk; break;
						case SDLK_d:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y += vel.speed; act.state = Component::walk; break;
						case SDLK_s:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.x -= vel.speed; act.state = Component::walk; break;
						case SDLK_f:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.x += vel.speed; act.state = Component::walk; break;
						case SDLK_w:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y -= vel.speed; vel.magnitude.x -= vel.speed; act.state = Component::walk; break;
						case SDLK_r:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y -= vel.speed; vel.magnitude.x += vel.speed; act.state = Component::walk; break;
						case SDLK_v:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y += vel.speed; vel.magnitude.x += vel.speed; act.state = Component::walk; break;
						case SDLK_x:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y += vel.speed; vel.magnitude.x -= vel.speed; act.state = Component::walk; break;
						}
					}
				}
			}
			else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym)
				{
				case SDLK_e: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.y) > 0) vel.magnitude.y += vel.speed; break;
				case SDLK_d: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.y) > 0) vel.magnitude.y -= vel.speed; break;
				case SDLK_s: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.x) > 0) vel.magnitude.x += vel.speed; break;
				case SDLK_f: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.x) > 0) vel.magnitude.x -= vel.speed; break;
				case SDLK_w: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.y) > 0) vel.magnitude.y += vel.speed; if (fabs(vel.magnitude.x) > 0) vel.magnitude.x += vel.speed;  break;
				case SDLK_r: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.y) > 0) vel.magnitude.y += vel.speed; if (fabs(vel.magnitude.x) > 0) vel.magnitude.x -= vel.speed;  break;
				case SDLK_v: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.y) > 0) vel.magnitude.y -= vel.speed; if (fabs(vel.magnitude.x) > 0) vel.magnitude.x -= vel.speed;  break;
				case SDLK_x: zone.emplace_or_replace<Component::Moving>(entity); if (fabs(vel.magnitude.y) > 0) vel.magnitude.y -= vel.speed; if (fabs(vel.magnitude.x) > 0) vel.magnitude.x += vel.speed;  break;
				}
			}
			if (act.state == Component::attack) {
				vel.magnitude.x = 0.0f;
				vel.magnitude.y = 0.0f;
			}
			else if  ((vel.magnitude.x == 0.0f) && (vel.magnitude.y == 0.0f) && zone.any_of<Component::Moving>(entity)) {
				zone.remove<Component::Moving>(entity);
				act.state = Component::idle;
			}
		}
	};

	void Interface_Input(entt::registry& zone, Component::Velocity& vel, Component::Action& act, entt::entity entity) { //can return bools for x and y dir, and 2 enums for direction and state
		if (event.key.repeat == 0) {
			if (event.type == SDL_KEYDOWN) {
				if (act.state != Component::attack) {
					switch (event.key.keysym.sym)
					{
					case SDLK_1: Entity_Control::Spell_Attack(zone, entity, Mouse::iXWorld_Mouse, Mouse::iYWorld_Mouse, "'fireball'"); break;
					case SDLK_2: Death_Spells::Summon_Skeleton(zone, Mouse::iXWorld_Mouse, Mouse::iYWorld_Mouse, "'skeleton'");  break;
					case SDLK_3: SDL_SetRelativeMouseMode(SDL_FALSE); break;
					case SDLK_4: SDL_SetRelativeMouseMode(SDL_TRUE); break;
					case SDLK_5: Debug_System::Toggle_Frame_Rate_Mode(); break;
					case SDLK_6: Interface::gridDepth++; break;
					case SDLK_7: Interface::gridDepth--; break;
					case SDLK_8: ; break;
					case SDLK_9: AI::Turn_On();  break;
					case SDLK_0: User_Mouse_Input::Selection_Soldiers();  break;
                    case SDLK_ESCAPE: Menu::Toggle(); break;
					case SDLK_PLUS:  break;
					case SDLK_MINUS: break;
                    case SDLK_p: UI_Spellbook::Toggle();  break;
					case SDLK_i: UI::Bag_UI::Toggle_Bag();  break;
					case SDLK_LALT: Items::showGroundItems = true;  break;
					case SDLK_RALT: Items::showGroundItems = true;  break;
					}
				}
			}
			else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym)
				{
                case SDLK_LALT: Items::showGroundItems = false;  break;
				case SDLK_RALT: Items::showGroundItems = false;  break;
				}
			}
		}
	};

	void Mouse_Input(entt::registry &zone, entt::entity &player_ID, Component::Position &playerPosition, Component::Melee_Range &meleeRange, Component::Camera &camera) {
		if (event.key.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				//check if cursor is in the bag UI
				if (UI::bToggleCharacterUI && Mouse::bRect_inside_Cursor(UI::Character_UI)) {
                    UI::Bag_UI::Interact_With_Bag(zone, Mouse::mouseItem, Mouse::screenMousePoint, Mouse::itemCurrentlyHeld, camera);
                    if (UI::Equipment_UI::Interact_With_Equipment(zone, Mouse::mouseItem, Mouse::screenMousePoint, Mouse::itemCurrentlyHeld, camera) == true) {
                        //updates character stats
                        zone.emplace_or_replace<Item_Component::Item_Equip>(player_ID);
					}
				}
                else {
                    UI::Drop_Item_If_On_Mouse(zone, camera, Mouse::mouseItem, Mouse::itemCurrentlyHeld);
                }
				//if (Mouse::bRect_inside_Cursor(UI::Character_UI) == false) {
				//	User_Mouse_Input::Selection_Box(zone); //if units are currently selected
				//}
			}

			else if (event.button.button == SDL_BUTTON_RIGHT) {
				if (UI::bToggleCharacterUI) {
					if (Mouse::bRect_inside_Cursor(UI::Character_UI)) {
						if (UI::Swap_Item_In_Bag_For_Equipped(zone, Mouse::screenMousePoint, camera)) {
							zone.emplace_or_replace<Item_Component::Item_Equip>(player_ID);
							return;
						}
						return;
					}
				}
                if (Mouse::itemCurrentlyHeld == false) {
                    if (Input_Control::Check_For_Mouse_Target(zone, Items::showGroundItems, player_ID, playerPosition,
                                                              meleeRange)) {
                        //Mouse::bRight_Mouse_Pressed_Attacking = true; //held down for constant attacking
                    } else {
                        Mouse::bRight_Mouse_Pressed = true; //held down for constant moving
                    }
                }
                else {
                    UI::Drop_Item_If_On_Mouse(zone, camera, Mouse::mouseItem, Mouse::itemCurrentlyHeld);
                }
			}
		}

		if (event.key.type == SDL_MOUSEBUTTONUP) {
			if (event.button.button == SDL_BUTTON_LEFT) {
			//	User_Mouse_Input::Select_Units(World::zone);
			}
			if (event.button.button == SDL_BUTTON_RIGHT) {
				Mouse::bRight_Mouse_Pressed = false;
				User_Mouse_Input::Command_Unit(World::zone);
			}
		}
	}


	void Update_User_Input(entt::registry &zone) {
        while (SDL_PollEvent(&event) != 0) {
            auto view = zone.view<Component::Velocity, Component::Action, Component::Position, Component::Melee_Range, Component::Input, Component::Camera>();
            for (auto player_ID: view) {
                auto &playerPosition = view.get<Component::Position>(player_ID);
                auto &meleeRange = view.get<Component::Melee_Range>(player_ID);
                auto &playerVelocity = view.get<Component::Velocity>(player_ID);
                auto &playerAction = view.get<Component::Action>(player_ID);
                auto &camera = view.get<Component::Camera>(player_ID);

				if (event.key.type == SDL_MOUSEWHEEL) {
					Interface::Update_Zoom(event);
				}
                if (event.key.type == SDL_MOUSEBUTTONDOWN || event.key.type == SDL_MOUSEBUTTONUP) {
                    Mouse_Input(zone, player_ID, playerPosition, meleeRange, camera);
                } else if (event.key.type == SDL_KEYDOWN || event.key.type == SDL_KEYUP) {
                    Movement_Input(zone, playerVelocity, playerAction, player_ID);
                    Interface_Input(zone, playerVelocity, playerAction, player_ID);
                }
                //if (event.key.type == SDL_JOYAXISMOTION) { // it works!
                //	if (event.jaxis. == 0) {
                //Joystick::JoystickInput(event);
                //std::cout << Joystick::JoystickInput(event) << std::endl;
                //	if (event.jaxis.axis == 0) {
                //	std::cout << event.jaxis.value << std::endl;
                //	}
            }
        }
	};

	void Input_Handler(entt::registry& zone) {
		Update_User_Input(zone);
	}

}
