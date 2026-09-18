#pragma once


#include "box2d/id.h"
#include "raylib.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "config.hpp"



namespace LBR
{

    enum EntityShape : uint8_t {
        RECTANGLE,
        TRIANGLE,
        CIRCLE,
    };


    enum EntityBehaviour : uint8_t {
        NORMAL,
        KILLING,
        FINISH,
    };


    enum EntityType : uint8_t {
        STATIC,
        DYNAMIC,
        MOVING,
        ROTATING,
    };



    class Entity
    {
    public:
        b2BodyId bodyId;
        Color color;
        EntityShape shape;
        EntityBehaviour behaviour;
        EntityType type;

        virtual void Draw() = 0;
        virtual void ChangeCoordinats(const float x, const float y) = 0;
    };


    class EntityRectangle : public Entity
    {
    public:
        float width;
        float height;

        EntityRectangle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, float x, float y, float width, float height, Color color = COLOR_ENTITY);
        void Draw() override;
        void ChangeCoordinats(float x, float y) override;
    };


    class EntityTriangle : public Entity
    {
    public:
        Vector2 v2;
        Vector2 v3;

        EntityTriangle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, Vector2 v1, Vector2 v2, Vector2 v3, Color color = COLOR_ENTITY);
        void Draw() override;
        void ChangeCoordinats(float x, float y) override;
    };


    class EntityCircle : public Entity
    {
    public:
        float radius;

        EntityCircle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, float x, float y, float radius, Color color = COLOR_ENTITY);
        void Draw() override;
        void ChangeCoordinats(float x, float y) override;
    };


    typedef struct Button Button;


    class World {
        static World *instance; ///< Poiner to the one existing instance of the world.

    public:
        b2WorldId world_id;
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<Entity>*> copy_buffer;
        std::vector<std::unique_ptr<Entity>*> selected_entity;
        std::unique_ptr<Entity> *hovered_entity;
        Rectangle menu_collision;
        Vector2 menu_coordinats;
        bool show_menu = false;
        bool pause = false;

        World();
        ~World();
        World(const World& obj) = delete;
        static World* GetIstance();
        static void CreateWindow();
        void CopyEntity(const float x, const float y);
        void PasteEntity(const float x, const float y);
        void SpawnRectangle(const float x, const float y);
        void SpawnTriangle(const float x, const float y);
        void SpawnCircle(const float x, const float y);
        void DetermineHoveredEntity()
        {

        }
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
