#include "raylib.h"

#include "include/classes.hpp"
#include "include/config.hpp"



LBR::World* LBR::World::instance = nullptr;

static LBR::World* world = LBR::World::GetIstance(); ///< World class instance



int main(void)
{
    LBR::World world = LBR::World();

    world.CreateWindow();

    while (!WindowShouldClose())
    {
        // if (IsKeyPressed(KEY_P))
        // {
        //     pause = !pause;
        // }
        // if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        // {
        //     b2Vec2 mouse_pos = { (float)GetMouseX(), (float)GetMouseY() };

        //     for (int i = 0; i < BOX_DENCITY; ++i)
        //     {
        //         float y = mouse_pos.y - 0.5 * boxExtent.y + GetRandomValue(-RANDOM_SPREAD, RANDOM_SPREAD);
        //         float x = mouse_pos.x - 0.5 * boxExtent.x + GetRandomValue(-RANDOM_SPREAD, RANDOM_SPREAD);

        //         Entity entity = Entity();
        //         b2BodyDef bodyDef = b2DefaultBodyDef();
        //         bodyDef.type = b2_dynamicBody;
        //         bodyDef.position = (b2Vec2){ x, y };
        //         entity.bodyId = b2CreateBody(world_id, &bodyDef);
        //         entity.extent = boxExtent;
        //         b2ShapeDef shapeDef = b2DefaultShapeDef();
        //         b2CreatePolygonShape(entity.bodyId, &shapeDef, &boxPolygon);

        //         userEntities.push_back(entity);
        //     }
        // }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            world.menu_coordinats = { (float)GetMouseX(), (float)GetMouseY() };
            world.show_menu = true;
        }

        // if (pause == false)
        // {
        //     float deltaTime = GetFrameTime();
        //     b2World_Step(world_id, deltaTime, 4);
        // }

        BeginDrawing();
        {
            ClearBackground(COLOR_BG);

            world.DrawEntities();

            if (world.show_menu) {
                std::vector<LBR::Button> buttons {
                    { "Copy"     , &LBR::World::CopyEntity, 0 }
                   ,{ "Paste"    , &LBR::World::PasteEntity, 0 }
                   ,{ "Rectangle", &LBR::World::SpawnRectangle }
                   ,{ "Triangle" , &LBR::World::SpawnRectangle }
                   ,{ "Circle"   , &LBR::World::SpawnRectangle }
                };

                if (world.GuiMenu(buttons, world.menu_coordinats.x, world.menu_coordinats.y, GUI_TEXT_SIZE * 8)
                || IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    world.show_menu = false;
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
