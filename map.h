#pragma once
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <algorithm>
#include <iostream>
#include "entt.hpp"
#include "components.h"
#include "entity_loader.h"
#include "collision.h"
#include "sprite_sheets.h"
#include "sprite_sheets.h"
//cell 100x100 pixels (change pixels to a meters??)
//map 100 cells x 100 cells

//hardcode the default size of the cell and the size of the map (eventually add the map as a cell for the "world" array)
//each cell needs an ID

//create the cell bounding boxes (the top left will be 0,0 and the map will grow right and down)
//parse all the trees and place them into their cell vector

namespace Maps {
    tmx::Map map;
    SDL_Texture* pTexture = NULL;
    int template_ID = 0;
    std::map<std::string, int> savedTemplate_IDs;

    // read name of the template object (it is already unique)
    // assign it an auto incrementented int
    // check if the object has already been assigned an int ID
    int Check_For_Template_ID (std::string &name) {
        if (savedTemplate_IDs.count(name) == 0){
            savedTemplate_IDs[name] = template_ID;
            int unit_ID = template_ID;
            template_ID++;
          //  std::cout << "created and assigned template key: " << unit_ID << std::endl;
            return unit_ID;
        }
        else {
           // std::cout << "assigned existing template key: " << savedTemplate_IDs[name] << std::endl;
            return savedTemplate_IDs[name];
        }
        std::cout << "failed to assign template key for: " << name << std::endl;
    }

    int Get_Existing_Template_ID (std::string &name, std::string &entity_class) {
        if (savedTemplate_IDs.count(name) == 0) {
            //if it is not found save the class as the name (the class will always have the name of the base unit of the class and thus will default to the base unit if the random unit is not found)
            name = entity_class;
            if (savedTemplate_IDs.count(name) == 0) {
                savedTemplate_IDs[name] = template_ID;
                int unit_ID = template_ID;
                template_ID++;
                //  std::cout << "created and assigned template key: " << unit_ID << std::endl;
                return unit_ID;
            }
            return savedTemplate_IDs[name];
        }
        else {
            // std::cout << "assigned existing template key: " << savedTemplate_IDs[name] << std::endl;
            return savedTemplate_IDs[name];
        }
        std::cout << "failed to assign template key for: " << name << std::endl;
    }

    void Create_Entity(entt::registry& zone, float x, float y, std::string &name, std::string entity_class, bool is_random, std::string &filepath) {
        auto entity = zone.create();
        Entity_Loader::Data data;
        int unit_ID = 0;

        //get the entity_class from the template in the tiled map
        //get the is_random from the template in the tiled map

        //if it is random, grab a random entry of the same class from the DB table, including the key name
        //
        //scaling only applies to trees
        if (is_random == 1) {
            name = Entity_Loader::Get_All_Of_Class(entity_class);

            //check if the random name has a tamplate ID, if it doesn't revert to default name
            unit_ID = Get_Existing_Template_ID(name, entity_class);
            Graphics::Create_Game_Object(unit_ID, filepath.c_str());
            data = Entity_Loader::parse_data(name);
//
            ////randomEntity must be converted into a std::string
//
            //auto& scale = zone.get<Component::Scale>(entity);
            //auto& radius = zone.get<Component::Radius>(entity);
//
            ////random number between 0.25 and 2
            //int rand_scale = 0.5;
            //scale.scale = rand_scale;
        }
        else {
            unit_ID = Check_For_Template_ID(name);
            Graphics::Create_Game_Object(unit_ID, filepath.c_str());
            data = Entity_Loader::parse_data(name);
        }
        if (name == "'player'") {

            Utilities::Log("yeah");
        }
        std::string sheetname = Entity_Loader::Get_Sprite_Sheet(name);
        //std::cout << "name: " << name << ", sheet name:" << sheetname << std::endl;
        Sprite_Sheet::sheetData sheetData = Sprite_Sheet::Assign (sheetname);


        // translates isometric position to world position
        float tileWidth = 128;
        float tileHeight = 64;
        float numTilesX =  x / tileWidth;
        float numTilesY =  y / tileHeight;
        x = 64.0f + x - (numTilesY * tileWidth / 2.0f);
        y = -37.0f + (numTilesX * tileHeight) + (y / 2.0f);

        //Add shared components
        auto& position = zone.emplace<Component::Position>(entity, x, y);
        auto& scale = zone.emplace<Component::Scale>(entity, data.scale);

        auto &radius = zone.emplace<Component::Radius>(entity, data.radius * data.scale);
        zone.emplace<Component::Direction>(entity, Component::Direction::N);
        zone.emplace<Component::handle>(entity, name);
        zone.emplace<Component::Mass>(entity, data.mass);
        zone.emplace<Component::Alive>(entity, true);
        zone.emplace<Component::Sprite_Offset>(entity, sheetData.x_offset * data.scale, sheetData.y_offset * data.scale);
        zone.emplace<Component::animation>(entity, Graphics::unitTextures[unit_ID]); /// need to load the texture only once and pass the pointer into that function

        //dynamic entities
        if (data.body_type == 1) {
            bool yes = true;
            Collision::Create_Dynamic_Body(zone, entity, position.x, position.y, radius.fRadius, data.mass, yes);
            std::vector<Component::Frame_Data> frameVector = {
                {sheetData.isStatic.numFrames ,0},
                {sheetData.idle.numFrames ,0},
                {sheetData.walk.numFrames ,0},
                {sheetData.run.numFrames ,0},
                {sheetData.attack.numFrames ,0},
                {sheetData.attack2.numFrames ,0},
                {sheetData.cast.numFrames ,0},
                {sheetData.struck.numFrames ,0},
                {sheetData.block.numFrames ,0},
                {sheetData.evade.numFrames ,0},
                {sheetData.stunned.numFrames ,0},
                {sheetData.dead.numFrames ,0},
                {sheetData.low_hp.numFrames ,0},
                {sheetData.resting.numFrames ,0},
                {sheetData.ranged.numFrames ,0},
                {sheetData.cheer.numFrames ,0},
                {sheetData.behavior.numFrames ,0},
                {sheetData.summoned.numFrames ,0}
            };;

            zone.emplace<Component::Actions>(entity, Component::idle, frameVector);

            //std::cout << frame.size() << std::endl;

            zone.get<Component::animation>(entity).sheet = { //populate the vector
                { {sheetData.isStatic.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.isStatic.firstFrame * sheetData.w, sheetData.isStatic.numFrames * sheetData.w,  sheetData.isStatic.reverses, 0, 75.0f, 0.0f},//idle array[numframes] = { 2ms, 4ms, 2ms} },
                { {sheetData.idle.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.idle.firstFrame * sheetData.w, sheetData.idle.numFrames * sheetData.w,  sheetData.idle.reverses, 0, 75.0f, 0.0f},//idle array[numframes] = { 2ms, 4ms, 2ms}
                { {sheetData.walk.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.walk.firstFrame * sheetData.w, sheetData.walk.numFrames * sheetData.w,  sheetData.walk.reverses, 0, 75.0f, 0.0f},//walk
                { {sheetData.run.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.run.firstFrame * sheetData.w, sheetData.run.numFrames * sheetData.w,  sheetData.run.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.attack.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.attack.firstFrame * sheetData.w, sheetData.attack.numFrames * sheetData.w,  sheetData.attack.reverses, 0, 75.0f, 0.0f},//atack
                { {sheetData.attack2.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.attack2.firstFrame * sheetData.w, sheetData.attack2.numFrames * sheetData.w,  sheetData.attack2.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.cast.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.cast.firstFrame * sheetData.w, sheetData.cast.numFrames * sheetData.w,  sheetData.cast.reverses, 0, 75.0f, 0.0f},//cast
                { {sheetData.struck.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.struck.firstFrame * sheetData.w, sheetData.struck.numFrames * sheetData.w,  sheetData.struck.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.block.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.block.firstFrame * sheetData.w, sheetData.block.numFrames * sheetData.w, sheetData.block.reverses, 0, 75.0f, 0.0f},//block
                { {sheetData.evade.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.evade.firstFrame * sheetData.w, sheetData.evade.numFrames * sheetData.w,  sheetData.evade.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.stunned.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.stunned.firstFrame * sheetData.w, sheetData.stunned.numFrames * sheetData.w,  sheetData.stunned.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.dead.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.dead.firstFrame * sheetData.w, sheetData.dead.numFrames * sheetData.w,  sheetData.dead.reverses, 0, 75.0f, 0.0f},//death/reverse to summon
                { {sheetData.low_hp.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.low_hp.firstFrame * sheetData.w, sheetData.low_hp.numFrames * sheetData.w,  sheetData.low_hp.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.resting.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.resting.firstFrame * sheetData.w, sheetData.resting.numFrames * sheetData.w,  sheetData.resting.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.ranged.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.ranged.firstFrame * sheetData.w, sheetData.ranged.numFrames * sheetData.w,  sheetData.ranged.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.cheer.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.cheer.firstFrame * sheetData.w, sheetData.cheer.numFrames * sheetData.w,  sheetData.cheer.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.behavior.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.behavior.firstFrame * sheetData.w, sheetData.behavior.numFrames * sheetData.w,  sheetData.behavior.reverses, 0, 75.0f, 0.0f},//ranged
                { {sheetData.summoned.firstFrame * sheetData.w, 0, sheetData.w, sheetData.h}, sheetData.summoned.firstFrame * sheetData.w, sheetData.summoned.numFrames * sheetData.w,  sheetData.summoned.reverses, 0, 75.0f, 0.0f},//ranged
            };
            zone.emplace<Component::Melee_Damage>(entity, data.damage_min, data.damage_max);
            zone.emplace<Component::Attack_Speed>(entity, data.attack_speed, 0);
            zone.emplace<Component::Velocity>(entity, 0.f, 0.0f, 0.f, 0.0f, data.speed);
            zone.emplace<Component::Health>(entity, data.health);
            zone.emplace<Component::Melee_Range>(entity, (data.radius + data.melee_range));
            zone.emplace<Component::Soldier>(entity);
            zone.emplace<Component::Commandable>(entity);
            zone.emplace<Component::Spellbook>(entity);

            if (name == "'player'") {
                zone.emplace<Component::Entity_Type>(entity, Component::Entity_Type::player);
                zone.emplace<Input>(entity);
                zone.emplace<Camera>(entity, 0.0f, 0.0f, Graphics::resolution.w, Graphics::resolution.h, 2.0f, 2.0f);

            }
            else {
                zone.emplace<Component::Entity_Type>(entity, Component::Entity_Type::unit);
                zone.emplace<Component::Sight_Range>(entity, data.sight_radius, position.x - (data.sight_radius / 2.0f), position.y - (data.sight_radius / 2.0f), data.sight_radius, data.sight_radius);
            }
        }

        //static entities
        else if (data.body_type == 0) {
            Collision::Create_Static_Body(zone, entity, position.x, position.y, data.radius);
            zone.get<Component::animation>(entity).sheet = {  //populate the vector
                {{ 0, 0, sheetData.h, sheetData.h}, 0, sheetData.w, 0, 0, 16.0f, 0.0f }
            };
            zone.emplace<Actions>(entity, isStatic);
            zone.get<Actions>(entity).frameCount = { { 0, 0} };

            zone.emplace<Component::Entity_Type>(entity, Entity_Type::foliage);
        }
    }

    // read the map data and import it into the data structures
    void Create_Map() {
        if (map.load("maps/untitled3.tmx")){
            for (auto &spriteSheet : map.getTilesets()) {

                std::string name = spriteSheet.getName();
                std::string filePathString = spriteSheet.getImagePath();
                filePathString.erase(0, 5);
                const char* filePathChar = filePathString.c_str();
                Graphics::pTexture[name] = Graphics::createTexture(filePathChar);

            }
            std::cout << "Loaded Map version: " << map.getVersion().upper << ", " << map.getVersion().lower << std::endl;
            if (map.isInfinite())
            {
                std::cout << "Map is infinite.\n";
            }

            const auto& layers = map.getLayers();
            std::cout << "Map has " << layers.size() << " layers" << std::endl;

            for (const auto& layer : layers) {
                if (layer->getType() == tmx::Layer::Type::Tile) {
                    const auto& tiles = layer->getLayerAs<tmx::TileLayer>().getTiles();
                    if (tiles.empty()) {
                        std::cout << "Layer has missing tile data\n";
                    }
                    else {
                        std::cout << "Layer has " << tiles.size() << " tiles.\n";
                    }
                }
                if (layer->getType() == tmx::Layer::Type::Object) {
                    //layer->getLayerAs<tmx::ObjectLayer>();
                    //for (auto object : ) {

                    //layer->getLayerAs<tmx::ObjectGroup>().
                    for (auto &object : layer->getLayerAs<tmx::ObjectGroup>().getObjects()) {
                        auto &position = object.getPosition();

                        std::string name = object.getName();

                        std::string entity_class = object.getClass();

                        bool is_random = false;
                        for (auto i : object.getProperties()) {
                            is_random = i.getBoolValue();
                        }
                        //if it is random it needs to grab a name from a unit that was already loaded into graphics or default to a default unit name
                        //get an array of all the potential names, check each on against teh std::map of graphics, keep all the ones already there and pick a random one
                        //if (is_random == false ) {}
                        std::string texture = object.getTilesetName();

                        auto &ff = map.getTemplateTilesets();
                        texture = ff.at(texture).getImagePath();
                        texture.erase(0, 5);
                        if (texture == "") {
                            Utilities::Log("asds");
                        }
                        Create_Entity(World::zone, position.x, position.y, name, entity_class, is_random, texture);
                    };
                    //}
                }
            }

        }
        else
        {
            std::cout << "Failed loading map" << std::endl;
        }
    }
}
