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
#include "skills.h"
#include "menu.h"
#include "events.h"
#include "pause.h"

namespace Event_Handler {

	/*I will make a component that will be pased to this funtion tree so the functions can do work on the position value of an entity "<velocity> <player_controllable>" */

	const Uint8* state = SDL_GetKeyboardState(NULL);

	void Movement_Input(entt::registry& zone, Component::Velocity& vel, Action_Component::Action& act, entt::entity entity) { //can return bools for x and y dir, and 2 enums for direction and state
		if (Events::event.key.repeat == 0) {
			if (Events::event.type == SDL_KEYDOWN) {
				if (act.state == Action_Component::idle || act.state == Action_Component::walk) {
					if (Events::event.type == SDL_KEYDOWN) {
						auto& position = zone.get<Component::Position>(entity);
						switch (Events::event.key.keysym.sym) {
						case SDLK_e:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y -= vel.speed; act.state = Action_Component::walk; break;
						case SDLK_d:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y += vel.speed; act.state = Action_Component::walk; break;
						case SDLK_s:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.x -= vel.speed; act.state = Action_Component::walk; break;
						case SDLK_f:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.x += vel.speed; act.state = Action_Component::walk; break;
						case SDLK_w:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y -= vel.speed; vel.magnitude.x -= vel.speed; act.state = Action_Component::walk; break;
						case SDLK_r:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y -= vel.speed; vel.magnitude.x += vel.speed; act.state = Action_Component::walk; break;
						case SDLK_v:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y += vel.speed; vel.magnitude.x += vel.speed; act.state = Action_Component::walk; break;
						case SDLK_x:  zone.emplace_or_replace<Component::Moving>(entity); vel.magnitude.y += vel.speed; vel.magnitude.x -= vel.speed; act.state = Action_Component::walk; break;
						}
					}
				}
			}
			else if (Events::event.type == SDL_KEYUP) {
				switch (Events::event.key.keysym.sym)
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
			if (act.state == Action_Component::attack) {
				vel.magnitude.x = 0.0f;
				vel.magnitude.y = 0.0f;
			}
			else if  ((vel.magnitude.x == 0.0f) && (vel.magnitude.y == 0.0f) && zone.any_of<Component::Moving>(entity)) {
				zone.remove<Component::Moving>(entity);
				act.state = Action_Component::idle;
			}
		}
	};

	void Interface_Input(entt::registry& zone, Component::Camera &camera, Component::Velocity& vel, Action_Component::Action& act, entt::entity entity) { //can return bools for x and y dir, and 2 enums for direction and state
		if (Events::event.key.repeat == 0) {
			if (Events::event.type == SDL_KEYDOWN) {
				if (act.state != Action_Component::attack) {
					switch (Events::event.key.keysym.sym)
					{
					case SDLK_1: Entity_Control::Spell_Attack(zone, entity, Mouse::iXWorld_Mouse, Mouse::iYWorld_Mouse, "fireball"); break;
					case SDLK_2: Death_Spells::Summon_Skeleton(zone, Mouse::iXWorld_Mouse, Mouse::iYWorld_Mouse, "skeleton");  break;
					case SDLK_3: SDL_SetRelativeMouseMode(SDL_FALSE); break;
					case SDLK_4: SDL_SetRelativeMouseMode(SDL_TRUE); break;
					case SDLK_5: Debug_System::Toggle_Frame_Rate_Mode(); break;
					case SDLK_6: Interface::gridDepth++; break;
					case SDLK_7: Interface::gridDepth--; break;
					case SDLK_8: Skills::Feign_Death(zone, entity); break;
					case SDLK_9: AI::Turn_On();  break;
					case SDLK_0: User_Mouse_Input::Selection_Soldiers();  break;
                    case SDLK_ESCAPE: Menu::Toggle(camera); break;
                    case SDLK_p: Pause::Toggle(); break;
					case SDLK_PLUS:  break;
					case SDLK_MINUS: break;
                    case SDLK_l: UI_Spellbook::Toggle();  break;
					case SDLK_i: UI::Bag_UI::Toggle_Bag();  break;
					case SDLK_LALT: Items::showGroundItems = true;  break;
					case SDLK_RALT: Items::showGroundItems = true;  break;
					}
				}
			}
			else if (Events::event.type == SDL_KEYUP) {
				switch (Events::event.key.keysym.sym)
				{
                case SDLK_LALT: Items::showGroundItems = false;  break;
				case SDLK_RALT: Items::showGroundItems = false;  break;
				}
			}
		}
	};

	void Mouse_Input(entt::registry &zone, entt::entity &player_ID, Component::Position &playerPosition, Component::Melee_Range &meleeRange, Component::Camera &camera) {
		if (Events::event.key.type == SDL_MOUSEBUTTONDOWN) {
			if (Events::event.button.button == SDL_BUTTON_LEFT) {
				//check if cursor is in the bag UI
				if (UI::bToggleCharacterUI && Mouse::bRect_inside_Cursor(UI::Character_UI)) {
                    UI::Bag_UI::Interact_With_Bag(zone, Mouse::mouseItem, Mouse::screenMousePoint, Mouse::itemCurrentlyHeld, camera);
                    if (UI::Equipment_UI::Interact_With_Equipment(zone, Mouse::mouseItem, Mouse::screenMousePoint, Mouse::itemCurrentlyHeld, camera, player_ID) == true) {
                        //updates character stats
                        zone.emplace_or_replace<Item_Component::Item_Equip>(player_ID);
					}
				}
                else {
                    UI::Drop_Item_If_On_Mouse(zone, camera, Mouse::mouseItem, Mouse::itemCurrentlyHeld);
                }
				if (Mouse::bRect_inside_Cursor(UI::Character_UI) == false) {
					User_Mouse_Input::Selection_Box(zone); //if units are currently selected
				}
			}

			else if (Events::event.button.button == SDL_BUTTON_RIGHT) {
				if (UI::bToggleCharacterUI) {
					if (Mouse::bRect_inside_Cursor(UI::Character_UI)) {
						if (UI::Swap_Item_In_Bag_For_Equipped(zone, Mouse::screenMousePoint, camera, player_ID)) {
							zone.emplace_or_replace<Item_Component::Item_Equip>(player_ID);
							return;
						}
						return;
					}
				}
                if (Mouse::itemCurrentlyHeld == false) {
                    User_Mouse_Input::Update_Move_Command_Box();
                    if (Input_Control::Check_For_Mouse_Target(zone, Items::showGroundItems, player_ID, playerPosition, meleeRange)) {
                        Mouse::bRight_Mouse_Pressed = false; //otherwise mouse move will override attack move
                    }
                    else {
                    }
                }
                else {
                    UI::Drop_Item_If_On_Mouse(zone, camera, Mouse::mouseItem, Mouse::itemCurrentlyHeld);
                }
			}
		}

		if (Events::event.key.type == SDL_MOUSEBUTTONUP) {
			if (Events::event.button.button == SDL_BUTTON_LEFT) {
				User_Mouse_Input::Select_Units(World::zone);
			}
			if (Events::event.button.button == SDL_BUTTON_RIGHT) {
                User_Mouse_Input::Command_Unit_Move(zone);
				Mouse::bRight_Mouse_Pressed = false;
			}
		}
	}


	void Update_User_Input(entt::registry &zone) {
//        keep function running to maintain input and perform actions during pause
        if (!Menu::toggleMenu) {
            while (SDL_PollEvent(&Events::event) != 0) {
                auto view = zone.view<Component::Velocity, Action_Component::Action, Component::Position, Component::Melee_Range, Component::Input, Component::Camera>();
                for (auto player_ID: view) {
                    auto &playerPosition = view.get<Component::Position>(player_ID);
                    auto &meleeRange = view.get<Component::Melee_Range>(player_ID);
                    auto &playerVelocity = view.get<Component::Velocity>(player_ID);
                    auto &playerAction = view.get<Action_Component::Action>(player_ID);
                    auto &camera = view.get<Component::Camera>(player_ID);

                    if (Events::event.key.type == SDL_MOUSEWHEEL) {
                        Interface::Update_Zoom(Events::event);
                    }
                    if (Events::event.key.type == SDL_MOUSEBUTTONDOWN || Events::event.key.type == SDL_MOUSEBUTTONUP) {
                        Mouse_Input(zone, player_ID, playerPosition, meleeRange, camera);
                    } else if (Events::event.key.type == SDL_KEYDOWN || Events::event.key.type == SDL_KEYUP) {
                        Movement_Input(zone, playerVelocity, playerAction, player_ID);
                        Interface_Input(zone, camera, playerVelocity, playerAction, player_ID);
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
        }
	};

	void Input_Handler(entt::registry& zone) {
		Update_User_Input(zone);
	}

}
