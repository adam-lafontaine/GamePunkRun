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

        auto dims = data.camera.scene_position.pos_game();

        ImGui::Text("Position: (%d, %d)", dims.x.get(), dims.y.get());
    }


    static void background_animation(game::BackgroundAnimation const& an, cstr label)
    {        
        ImGui::SeparatorText(label);

        auto& w = an.work_asset_ids.data;
        auto& s = an.select_asset_ids.data;

        auto wi = an.work_asset_ids.cursor.value_;
        auto wc = an.work_asset_ids.count;
        auto sc = an.select_asset_ids.size;
        auto id = an.current_background.value_;

        auto uw = [&](int i) { return (u32)w[i].value_; };
        auto us = [&](int i) { return (u32)s[i].value_; };

        ImGui::Text(" Select   (%2u): %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u, %2u", sc, us(0), us(1), us(2), us(3), us(4), us(5), us(6), us(7), us(8), us(9), us(10), us(11));
        ImGui::Text("Working (%u/%u): %2u, %2u, %2u, %2u", wi, wc, uw(0), uw(1), uw(2), uw(3));
        ImGui::Text("Last: %u", id);        
    }


    static void player(game::StateData& data)
    {
        ImGui::SeparatorText("Player");

        auto id = data.player_state.sprite;

        auto pos = data.sprites.get_tile_pos(id);
        auto vel = data.sprites.get_tile_velocity(id);

        ImGui::Text("Position: (%4.1f, %4.1f)", pos.x.get(), pos.y.get());
        ImGui::Text("Velocity: (%3.2f, %3.2f), ", vel.x.get(), vel.y.get());

        /*static int v = 0;

        v = vel.x;

        ImGui::InputInt("Vel x", &v);

        vel.x = v;*/
    }
}


/* tiles */

namespace internal
{
    static void plot_active_tiles(game::TileTable const& table)
    {
        constexpr int data_count = 256;
        constexpr auto plot_min = 0.0f;
        constexpr auto plot_size = ImVec2(0, 80.0f);
        constexpr auto data_stride = sizeof(f32);

        auto N = table.capacity;

        auto plot_max = (f32)N;

        static f32 plot_data[data_count] = { 0 };
        static u8 data_offset = 0;

        int active_count = 0;
        for (u32 i = 0; i < N; i++)
        {
            game::TileID id = { i };
            active_count += game::is_spawned(table, id);
        }

        plot_data[data_offset++] = (f32)active_count;

        char overlay[32] = { 0 };
        stb::qsnprintf(overlay, 32, "%d/%d", active_count, (int)N);

        ImGui::PlotLines("##PlotTiles", 
            plot_data, 
            data_count, 
            (int)data_offset, 
            overlay,
            plot_min, plot_max, 
            plot_size, 
            data_stride);
    }


    static void tile_table(game::TileTable const& table)
    {
        static bool show_inactive = true;

        ImGui::Checkbox("Show Inactive", &show_inactive);

        constexpr int col_id = 0;
        constexpr int col_pos = col_id + 1;
        constexpr int col_bmp = col_pos + 1;
        constexpr int col_active = col_bmp + 1;
        constexpr int n_columns = col_active + 1;

        int table_flags = ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV;
        auto table_dims = ImVec2(0.0f, 0.0f);

        if (!ImGui::BeginTable("Tiles##TileTable", n_columns, table_flags, table_dims)) 
        { 
            return; 
        }

        ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableSetupColumn("Pos", ImGuiTableColumnFlags_WidthStretch, 20.0f);
        ImGui::TableSetupColumn("Bmp", ImGuiTableColumnFlags_WidthStretch, 20.0f);
        ImGui::TableSetupColumn("On", ImGuiTableColumnFlags_WidthStretch, 20.0f);

        auto N = table.capacity;
        auto pos = table.position;
        auto bmp = table.bitmap_id;

        ImGui::TableHeadersRow();

        for (u32 i = 0; i < N; i++)
        {
            game::TileID id = { i };
            if (!show_inactive && !game::is_spawned(table, id))
            {
                continue;
            }

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(col_id);
            ImGui::Text("%u", i);

            ImGui::TableSetColumnIndex(col_pos);
            ImGui::Text("(%4.0f, %4.0f)", pos[i].x.get(), pos[i].y.get());

            ImGui::TableSetColumnIndex(col_bmp);
            ImGui::Text("%u", bmp[i].value_);

            ImGui::TableSetColumnIndex(col_active);
            ImGui::Text("%d", game::is_spawned(table, id));
        }

        ImGui::EndTable();
    }


    static void tiles(game::TileTable const& table)
    {
        ImGui::SeparatorText("Tiles");

        plot_active_tiles(table);

        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
        if (ImGui::TreeNode("Table##TilesTreeNode"))
        {
            tile_table(table);
            ImGui::TreePop();
        }
        ImGui::PopStyleVar();
        
    }


    static void sprite_table(game::SpriteTable const& table)
    {
        static bool show_inactive = true;

        ImGui::Checkbox("Show Inactive", &show_inactive);

        constexpr int col_id = 0;
        constexpr int col_pos = col_id + 1;
        constexpr int col_vel = col_pos + 1;
        constexpr int col_active = col_vel + 1;
        constexpr int n_columns = col_active + 1;

        int table_flags = ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV;
        auto table_dims = ImVec2(0.0f, 0.0f);

        if (!ImGui::BeginTable("Tiles##TileTable", n_columns, table_flags, table_dims)) 
        { 
            return; 
        }

        ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableSetupColumn("Pos", ImGuiTableColumnFlags_WidthStretch, 20.0f);
        ImGui::TableSetupColumn("Vel", ImGuiTableColumnFlags_WidthStretch, 20.0f);
        ImGui::TableSetupColumn("On", ImGuiTableColumnFlags_WidthStretch, 20.0f);

        auto N = table.capacity;

        ImGui::TableHeadersRow();

        for (u32 i = 0; i < N; i++)
        {
            game::SpriteID id = { i };

            if (!show_inactive && !game::is_spawned(table, id))
            {
                continue;
            }
            auto pos = table.get_tile_pos(id);
            auto vel = table.get_tile_velocity(id);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(col_id);
            ImGui::Text("%u", i);

            ImGui::TableSetColumnIndex(col_pos);
            ImGui::Text("(%4.0f, %4.0f)", pos.x.get(), pos.y.get());

            ImGui::TableSetColumnIndex(col_vel);
            ImGui::Text("(%3.2f, %3.2f)", vel.x.get(), vel.y.get());

            ImGui::TableSetColumnIndex(col_active);
            ImGui::Text("%d", game::is_spawned(table, id));
        }

        ImGui::EndTable();
    }
    
}


/* sprites */

namespace internal
{
    static void plot_active_sprites(game::SpriteTable const& table)
    {
        constexpr int data_count = 256;
        constexpr auto plot_min = 0.0f;
        constexpr auto plot_size = ImVec2(0, 80.0f);
        constexpr auto data_stride = sizeof(f32);

        auto N = table.capacity;

        auto plot_max = (f32)N;

        static f32 plot_data[data_count] = { 0 };
        static u8 data_offset = 0;

        int active_count = 0;
        for (u32 i = 0; i < N; i++)
        {
            game::SpriteID id = { i };
            active_count += game::is_spawned(table, id);
        }

        plot_data[data_offset++] = (f32)active_count;

        char overlay[32] = { 0 };
        stb::qsnprintf(overlay, 32, "%d/%d", active_count, (int)N);

        ImGui::PlotLines("##PlotSprites", 
            plot_data, 
            data_count, 
            (int)data_offset, 
            overlay,
            plot_min, plot_max, 
            plot_size, 
            data_stride);
    }


    static void sprites(game::SpriteTable const& table)
    {
        ImGui::SeparatorText("Sprites");

        plot_active_sprites(table);

        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
        if (ImGui::TreeNode("Table##SpritesTreeNode"))
        {
            sprite_table(table);
            ImGui::TreePop();
        }
        ImGui::PopStyleVar();
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

        if (ImGui::CollapsingHeader("Camera"))
        {
            internal::camera(data);
        }

        if (ImGui::CollapsingHeader("Background"))
        {
            internal::background_animation(data.background.bg_1, "Background 1");
            internal::background_animation(data.background.bg_2, "Background 2");            
        }

        if (ImGui::CollapsingHeader("Player"))
        {
            internal::player(data);
        }

        if (ImGui::CollapsingHeader("Tiles"))
        {
            internal::tiles(data.tiles);
        }

        if (ImGui::CollapsingHeader("Sprites"))
        {
            internal::sprites(data.sprites);
        }        

        ImGui::End();
    }
}