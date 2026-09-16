#pragma once


#include "box2d/id.h"
#include "raylib.h"

#include <cstdint>
#include <memory>
#include <vector>



namespace LBR
{

    enum ShapeProperties : uint8_t {
        X
       ,Y
       ,RADIUS
    };


    enum Shape : uint8_t {
        RECTANGLE,
        TRIANGLE,
        CIRCLE,
    };


    enum EntityType : uint8_t {
        NORMAL,
        KILLING,
        FINISH,
    };


    // enum : uint8_t {
    //     STATIC,
    //     MOVING,
    //     ROTATING,
    // };



    class Entity
    {
    public:
        b2BodyId bodyId;
        Color color;
        Shape shape;
        EntityType type;

        virtual void Draw() = 0;
        virtual void ChangeCoordinats(const float x, const float y) = 0;
    };


    class EntityRectangle : public Entity
    {
    public:
        float x;
        float y;
        float width;
        float height;

        EntityRectangle(b2WorldId world_id, EntityType type, float x, float y, float width, float height, Color color);
        void Draw() override;
        void ChangeCoordinats(float x, float y) override;
    };


    class EntityTriangle : Entity
    {
        Vector2 v1;
        Vector2 v2;
        Vector2 v3;
    public:

    };


    class EntityCircle : Entity
    {
        float x;
        float y;
        float radius;
    public:

    };


    typedef struct Button Button;


    class World {
        static World *instance; ///< Poiner to the one existing instance of the world.

    public:
        b2WorldId world_id;
        std::vector<std::unique_ptr<Entity>> entities;
        std::unique_ptr<Entity> copy_buffer;
        std::unique_ptr<Entity> selected_item;
        Vector2 menu_coordinats;
        bool show_menu = false;

        World();
        static World* GetIstance();
        static void CreateWindow();
        void CopyEntity(const float x, const float y);
        void PasteEntity(const float x, const float y);
        void SpawnRectangle(const float x, const float y);
        void DrawEntities();
        bool GuiMenu(const std::vector<Button> buttons, const float x, const float y, const float width);
    };


    struct Button
    {
        const char* label;
        void (World::*action)(const float, const float) { nullptr };
        bool is_enabled = true;
    };
}
