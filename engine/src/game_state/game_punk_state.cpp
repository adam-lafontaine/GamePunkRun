#include "../../../game_punk/src/app/app.cpp"


/* game state properties */

namespace game_state
{
    namespace game = game_punk;
    namespace img = image;


    // static properties
    game::AppState game_state;
    game::DebugContext game_dbg;
    bool game_running = false;


    
}


/* game wrapper */

namespace game_state
{
    bool init(Vec2Du32& screen_dimensions)
    {
        auto result = game::init(game_state);

        screen_dimensions = result.app_dimensions;

        app_log("%s", game::decode_error(result.error));

        return result.success;
    }


    bool set_screen_memory(img::ImageView screen)
    {
        return game_running = game::set_screen_memory_dbg(game_state, screen, game_dbg);
    }


    void update(input::Input const& input)
    {
        game::update_dbg(game_state, input, game_dbg);
    }


    void reset()
    {
        game::reset_dbg(game_state, game_dbg);
    }


    void close()
    {
        game::close(game_state);
    }
}


#include "../../../libs/imgui_1_92/imgui.h"


namespace game_state
{
namespace internal
{
    static void camera(game::StateData const& data)
    {
        ImGui::SeparatorText("Camera");

        auto dims = data.camera.viewport_pos_px.game;

        ImGui::Text("Position: (%u, %u)", dims.x, dims.y);
        ImGui::Text("Color: %u", data.ui.font_color_id);

        auto pos = data.icon_pos.game;
        ImGui::Text("Icon: (%d, %d)", pos.x, pos.y);
    }
    
    
    static void render_layers()
    {        
        static bool sky;
        static bool bg1;
        static bool bg2;
        static bool sprite;
        static bool hud;
        static bool ui;
        
        auto& layers = game_dbg.layers;

        sky = (bool)layers.sky;
        bg1 = (bool)layers.bg1;
        bg2 = (bool)layers.bg2;
        sprite = (bool)layers.sprite;
        hud = (bool)layers.hud;
        ui = (bool)layers.ui;

        bool updated = false;

        ImGui::SeparatorText("Render Layers");

        ImGui::Checkbox("Sky##LayerSky", &sky);
        ImGui::Checkbox("Background 1##LayerBG1", &bg1);
        ImGui::Checkbox("Background 2##LayerBG2", &bg2);
        ImGui::Checkbox("Sprites/Tiles##LayerSprite", &sprite);
        ImGui::Checkbox("HUD##LayerHUD", &hud);
        ImGui::Checkbox("UI##LayerUI", &ui);

        updated = 
            sky != (bool)layers.sky ||
            bg1 != (bool)layers.bg1 ||
            bg2 != (bool)layers.bg2 ||
            sprite != (bool)layers.sprite ||
            hud != (bool)layers.hud ||
            ui != (bool)layers.ui;

        if (updated)
        {
            layers.sky = sky;
            layers.bg1 = bg1;
            layers.bg2 = bg2;
            layers.sprite = sprite;
            layers.hud = hud;
            layers.ui = ui;
        }
        
    }
}
}

/* game state */

namespace game_state
{
    void show_game_state()
    {
        auto& data = game::get_data(game_state);

        ImGui::Begin("Game State");

        internal::camera(data);
        internal::render_layers();

        ImGui::End();
    }
}