#pragma once
#include <stdint.h>
#include "tinyxml2.h"
#include "spritesheet_structs.h"
#include <string>

namespace Texture_Packer {
        ///on init we need to do it parse the SQLite db for all sprite sheets names "texture_packer" and use the result to preallocate all the nodes of the std::unordered_map

    std::unordered_map<std::string, Rendering_Components::Sheet_Data> Packer_Textures;

    struct Type_Data{
        std::string type;
        std::string path;
    };

    Type_Data Get_Sprite_Sheet(std::string sheetname) {// needs to search for  a specific row that I can input in the arguments
        Type_Data typeData;
            ///check if the name exists??
        std::string sheet_name = db::Append_Quotes(sheetname);

        const unsigned char* sheet;
        const unsigned char* type;
        sqlite3_stmt* stmt;
        char buf[400];
        const char* jj = "SELECT xml_path, equip_type FROM unit_data WHERE name = ";
        strcpy(buf, jj);
        strcat(buf, sheet_name.c_str());
        sqlite3_prepare_v2(db::db, buf, -1, &stmt, 0);
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            sheet = sqlite3_column_text(stmt, 0);
            const char * s = (const char *)sheet;
            typeData.path = std::string(reinterpret_cast< const char *> (s));
            type = sqlite3_column_text(stmt, 1);
            s = (const char *)type;
            typeData.type = std::string(reinterpret_cast< const char *> (s));
        }
        return typeData;
    }

    Rendering_Components::Sprite_Offset Get_Sprite_Offets_From_db(std::string &sheet) {// needs to search for  a specific row that I can input in the arguments
        std::string sheet_name = db::Append_Quotes(sheet);
        Rendering_Components::Sprite_Offset offset = {};
        sqlite3_stmt *stmt;
        const unsigned char *sheetType;
        char buf[300];
        const char *ii = "SELECT x_offset, y_offset FROM sprite_layout WHERE sprite_sheet = ";
        strcpy(buf, ii);
        strcat(buf, sheet_name.c_str());
        sqlite3_prepare_v2(db::db, buf, -1, &stmt, 0);
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            offset.x = (float) sqlite3_column_double(stmt, 0);
            offset.y = (float) sqlite3_column_double(stmt, 1);
        }
        return offset;
    }





    std::unordered_map<std::string, Rendering_Components::Sheet_Data>* TexturePacker_Import(std::string &name, std::string &xml_path, SDL_Texture* texture) {
            ///check if the sheet data already exists


        if ( Packer_Textures[name].frameList.size() > 1) {
            return &Packer_Textures;
        }

            ///get path from db
        Type_Data typeData = Get_Sprite_Sheet(name);
		tinyxml2::XMLDocument spriteSheetData;
		const char* path = typeData.path.c_str();
        if (path == NULL){
            Utilities::Log("TexturePacker_Import() failed, empty xml_path");
            return NULL;
        }
        spriteSheetData.LoadFile(path);
        tinyxml2::XMLElement *pSpriteElement;
        pSpriteElement = spriteSheetData.RootElement()->FirstChildElement("sprite");

        Rendering_Components::Sprite_Sheet_Data frame = {};
        Rendering_Components::Sheet_Data spritesheet;
        spritesheet.frameList.reserve(200);
        spritesheet.texture = texture;

        int frameIndex = 0;

        if (pSpriteElement != NULL) {
                ///get frame data for each state
            std::string n = pSpriteElement->Attribute("n");
            Spritesheet_Structs::Get_Frame_Action_Data(typeData.type, name, n, spritesheet.actionFrameData, frameIndex);
                ///get sprite data
            frame.clip.x = pSpriteElement->IntAttribute("x");
            frame.clip.y = pSpriteElement->IntAttribute("y");
            frame.clip.w = pSpriteElement->IntAttribute("w");
            frame.clip.h = pSpriteElement->IntAttribute("h");
            frame.x_offset = pSpriteElement->IntAttribute("oX");
            frame.y_offset = pSpriteElement->IntAttribute("oY");
            spritesheet.frameList.emplace_back(frame);
                ///so we only grab this once
            frameIndex++;
                ///this grabs the next line
            pSpriteElement = pSpriteElement->NextSiblingElement("sprite");
        }
            ///get the rest of the xml
        while (pSpriteElement != NULL) {
                ///get frame data for each state
            std::string n = pSpriteElement->Attribute("n");
            Spritesheet_Structs::Get_Frame_Action_Data(typeData.type, name, n, spritesheet.actionFrameData, frameIndex);
                ///get sprite data
			frame.clip.x = pSpriteElement->IntAttribute("x");
			frame.clip.y = pSpriteElement->IntAttribute("y");
			frame.clip.w = pSpriteElement->IntAttribute("w");
			frame.clip.h = pSpriteElement->IntAttribute("h");
			frame.x_offset = pSpriteElement->IntAttribute("oX");
			frame.y_offset = pSpriteElement->IntAttribute("oY");
            spritesheet.frameList.emplace_back(frame);
            frameIndex++;
                ///this grabs the next line
			pSpriteElement = pSpriteElement->NextSiblingElement("sprite");
		}

        spritesheet.frameList.shrink_to_fit();
        Packer_Textures[name] = spritesheet;
//        spritesheet.color = Graphics::Set_Random_Color();
        return &Packer_Textures;
	}
}