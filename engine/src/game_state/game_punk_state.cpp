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

        auto dims = data.camera.scene_position.game;

        ImGui::Text("Position: (%u, %u)", dims.x, dims.y);
    }


    static void background_animation(game::BackgroundAnimation const& an, cstr label)
    {        
        ImGui::SeparatorText(label);

        auto& w = an.work_asset_ids.data;
        auto& s = an.select_asset_ids.data;

        auto wc = an.work_asset_ids.count;
        auto sc = an.select_asset_ids.size;
        auto id = an.load_cmd.ctx.item_id;

        auto uw = [&](int i) { return (u32)w[i].value_; };
        auto us = [&](int i) { return (u32)s[i].value_; };

        ImGui::Text(" Select (%2u): %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u", sc, us(0), us(1), us(2), us(3), us(4), us(5), us(6), us(7), us(8), us(9), us(10), us(11));
        ImGui::Text("Working (%2u): %2u, %2u, %2u, %2u", wc, uw(0), uw(1), uw(2), uw(3));
        ImGui::Text("Last: %u", id);        
    }


    static void player(game::StateData& data)
    {
        ImGui::SeparatorText("Player");

        auto id = data.punk_sprite;

        auto pos = data.sprites.position_at(id);
        auto& vel = data.sprites.velocity_px_at(id);

        ImGui::Text("Position: (%u, %u)", (u32)pos.x, (u32)pos.y);
        ImGui::Text("Velocity: (%d, %d), ", vel.x, vel.y);

        static int v = 0;

        v = vel.x;

        ImGui::InputInt("Vel x", &v);

        vel.x = v;
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
        internal::background_animation(data.background.bg_1, "Background 1");
        internal::background_animation(data.background.bg_2, "Background 2");
        internal::player(data);

        ImGui::End();
    }
}