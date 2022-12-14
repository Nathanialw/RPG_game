#pragma once
#include "components.h"


namespace Mouse {
	namespace {
		float cursorRadius = 5.0f;
		float Mouse_Selection_Box_x;
		float Mouse_Selection_Box_y;
	}

	bool bRight_Mouse_Pressed = false;
	bool bLeft_Mouse_Pressed = false;
	float iXMouse;
	float iYMouse;
	SDL_FPoint mousePoint;
	SDL_FPoint screenMousePoint;

	float iXWorld_Mouse;
	float iYWorld_Mouse;
	float Mouse_Selection_Box_x_Display;
	float Mouse_Selection_Box_y_Display;

	entt::entity mouseItem;
	bool itemCurrentlyHeld = false;

	bool FRect_inside_Cursor(SDL_FRect &rect) {
		SDL_FRect cursor = {iXWorld_Mouse, iYWorld_Mouse, cursorRadius, cursorRadius };
		return Utilities::bFRect_Intersect(cursor, rect);
	}

    bool FRect_inside_Screen_Cursor(SDL_FRect &rect) {
        SDL_FRect cursor = {iXMouse, iYMouse, cursorRadius, cursorRadius };
        return Utilities::bFRect_Intersect(cursor, rect);
    }

    bool bRect_inside_Cursor(SDL_FRect& rect) {
		SDL_FRect cursor = {iXMouse, iYMouse, cursorRadius, cursorRadius };
		return Utilities::bFRect_Intersect(cursor, rect);
	}

	bool Point_Inside_Cursor(float targetX, float targetY) { 
		if (iXWorld_Mouse + cursorRadius > targetX &&
            iXWorld_Mouse - cursorRadius < targetX &&
            iYWorld_Mouse - cursorRadius < targetY &&
            iYWorld_Mouse + cursorRadius > targetY) {
			return true;
		}
		else return false;
	}

	bool Mouse_Selection_Box(SDL_FRect target) {
		SDL_FRect entity = { float(Mouse::Mouse_Selection_Box_x), float(Mouse::Mouse_Selection_Box_y), float(Mouse::iXWorld_Mouse - Mouse::Mouse_Selection_Box_x), float(Mouse::iYWorld_Mouse - Mouse::Mouse_Selection_Box_y) };
		if ((entity.y <= target.y + target.h) &&
			(entity.x <= target.x + target.w) &&
			(entity.y + entity.h >= target.y) &&
			(entity.x + entity.w >= target.x)) {
			return true;
		}
		return false;
	};
}