#include "box2d/box2d.h"
#include "raylib.h"

#include <memory>
#include <vector>

#include "include/classes.hpp"
#include "include/config.hpp"



LBR::World* LBR::World::instance = nullptr;

static LBR::World* world = LBR::World::GetIstance(); ///< World class instance



int main(void)
{
    LBR::World world = LBR::World();

    world.CreateWindow();

    std::vector<std::unique_ptr<LBR::Entity>> rain_entities;

    while (!WindowShouldClose())
    {
        // if (IsKeyPressed(KEY_P))
        // {
        //     pause = !pause;
        // }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            world.menu_coordinats = { (float)GetMouseX(), (float)GetMouseY() };
            world.show_menu = true;
        }
        if (world.pause == false)
        {
            b2World_Step(world.world_id, GetFrameTime(), 4);
        }

        static double timestamp = GetTime();
        if (GetTime() - timestamp > 0.1f) {
            float y = 0;
            float x = GetRandomValue(0, WINDOW_WIDTH);

            switch (GetRandomValue(LBR::RECTANGLE, LBR::CIRCLE))
            {
            case LBR::RECTANGLE:
                rain_entities.push_back(
                        std::make_unique<LBR::EntityRectangle>(
                              world.world_id
                            , LBR::NORMAL
                            , LBR::DYNAMIC
                            , x - RAIN_RECTANGLE_SIZE / 2
                            , y - RAIN_RECTANGLE_SIZE / 2 + RAIN_RECTANGLE_SIZE
                            , RAIN_RECTANGLE_SIZE
                            , RAIN_RECTANGLE_SIZE
                ));
                break;
            case LBR::TRIANGLE:
                rain_entities.push_back(
                        std::make_unique<LBR::EntityTriangle>(
                              world.world_id
                            , LBR::NORMAL
                            , LBR::DYNAMIC
                            , Vector2 {x - RAIN_RECTANGLE_SIZE / 2, y + RAIN_RECTANGLE_SIZE }
                            , Vector2 {x, y}
                            , Vector2 {x + RAIN_RECTANGLE_SIZE / 2, y + RAIN_RECTANGLE_SIZE }
                ));
                break;
            case LBR::CIRCLE:
                rain_entities.push_back(
                        std::make_unique<LBR::EntityCircle>(
                              world.world_id
                            , LBR::NORMAL
                            , LBR::DYNAMIC
                            , x - RAIN_CIRCLE_SIZE / 2
                            , y - RAIN_CIRCLE_SIZE / 2 + RAIN_CIRCLE_SIZE
                            , RAIN_CIRCLE_SIZE
                ));
                break;
            default:
                exit(1);
            }
            timestamp = GetTime();
        }

        BeginDrawing();
        {
            ClearBackground(COLOR_BG);

            world.DrawEntities();

            for (auto& entity : rain_entities)
            {
                switch (entity->shape)
                {
                case LBR::RECTANGLE:
                    dynamic_cast<LBR::EntityRectangle*>(entity.get())->Draw();
                    break;
                case LBR::TRIANGLE:
                    dynamic_cast<LBR::EntityTriangle*>(entity.get())->Draw();
                    break;
                case LBR::CIRCLE:
                    dynamic_cast<LBR::EntityCircle*>(entity.get())->Draw();
                default:
                    break;
                }
            }

            if (world.show_menu) {
                std::vector<LBR::Button> buttons {
                    { "Copy"     , &LBR::World::CopyEntity, 0 }
                   ,{ "Paste"    , &LBR::World::PasteEntity, 0 }
                   ,{ "Rectangle", &LBR::World::SpawnRectangle }
                   ,{ "Triangle" , &LBR::World::SpawnTriangle }
                   ,{ "Circle"   , &LBR::World::SpawnCircle }
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
