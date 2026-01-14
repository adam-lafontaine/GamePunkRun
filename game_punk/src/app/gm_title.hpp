namespace game_punk
{
namespace gm_title
{

namespace internal
{

    static void set_title_image(StateData& data)
    {
    #include "../../res/title/title_game.cpp"

        auto& src = title_game;
        auto sw = src.width;
        auto sh = src.height;

        auto screen = to_image_view(data.camera);

        auto gw = screen.width;
        auto gh = screen.height;

        auto& pixels = data.ui.pixels;

        bt::ColorTableImage table;
        table.rgba.height = 1;
        table.rgba.width = sizeof(src.table) / sizeof(src.table[0]);
        table.rgba.data_ = (p32*)src.table;

        bt::TableFilterImage filter;
        filter.gray.width = sw;
        filter.gray.height = sh;
        filter.gray.data_ = (u8*)src.keys;

        auto out = data.ui.fullscreen_view;
        auto converted = img::make_view(sw, sh, push_elements(pixels, sw * sh));        

        bool ok = true;

        ok &= bt::color_table_convert(filter, table, converted);
        app_assert(ok && "Title image failed");

        u32 scale = math::min(gw / sw, gh / sh, 2u);

        auto w = sw * scale;
        auto h = sh * scale;
        auto x = (gw - w) / 2;
        auto y = (gh - h) / 2;
        auto scaled = img::sub_view(out, img::make_rect(x, y, w, h));

        auto color = img::pixel_at(converted, 0, 0);
        img::fill(out, color);

        img::scale_up(converted, scaled, scale);

        reset_stack(data.ui.pixels);
    }
    
    
    static void draw_title(StateData const& data)
    {
        img::copy(data.ui.fullscreen_view, to_image_view(data.camera));
    }


    static void draw_loading(StateData const& data)
    {
        draw_title(data);
    }
}
}
}


namespace game_punk
{
namespace gm_title
{
    static void init(StateData& data)
    {
        if (data.asset_data.status != AssetStatus::Success)
        {
            assets::load_game_assets(data);
        }

        internal::set_title_image(data);
    }


    static void update(StateData& data, InputCommand const& cmd)
    {
        auto& dq = data.drawq;
        auto& camera = data.camera;        

        switch (data.asset_data.status)
        {
        case AssetStatus::None:
        case AssetStatus::FailLoad:
        case AssetStatus::FailRead:        
            set_game_mode(data, GameMode::Error);
            break;

        case AssetStatus::Loading:
            internal::draw_loading(data);
            break;

        case AssetStatus::Success:
            internal::draw_title(data);
            break;

        default:
            break;
        }
        
        auto gameplay_ready = data.asset_data.status == AssetStatus::Success;
        if (gameplay_ready && cmd.title_ok)
        {
            set_game_mode(data, GameMode::Gameplay);            
        }
    }
}
}