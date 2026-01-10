namespace game_punk
{
namespace gm_title
{

/* static data */
namespace internal
{
    static ImageView title_image;
}



namespace internal
{

    static void set_title_image(StateData const& data)
    {
    #include "../../res/title/title_game.cpp"

        auto& src = title_game;

        bt::ColorTableImage table;
        table.rgba.height = 1;
        table.rgba.width = sizeof(src.table) / sizeof(src.table[0]);
        table.rgba.data_ = (p32*)src.table;

        bt::TableFilterImage filter;
        filter.gray.width = src.width;
        filter.gray.height = src.height;
        filter.gray.data_ = (u8*)src.keys;

        title_image = img::make_view(src.width, src.height, data.ui.pixels.data_);

        bool ok = true;

        ok &= bt::color_table_convert(filter, table, title_image);
        app_assert(ok && "Title image failed");
    }
    
    
    static void draw_centered(SceneCamera const& camera)
    {
        auto& src = title_image;
    
        auto dst = to_image_view(camera);

        u32 scale = 2;

        auto w = src.width * scale;
        auto h = src.height * scale;
        auto x = (dst.width - w) / 2;
        auto y = (dst.height - h) / 2;

        auto color = img::pixel_at(src, 0, 0);
        img::fill(dst, color);

        img::scale_up(src, img::sub_view(dst, img::make_rect(x, y, w, h)), scale);
    }


    static void draw_loading(SceneCamera const& camera)
    {
        draw_centered(camera);
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
            internal::draw_loading(camera);
            break;

        case AssetStatus::Success:
            internal::draw_centered(camera);
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