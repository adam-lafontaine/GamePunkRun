namespace game_punk
{
namespace gm_title
{
namespace internal
{
    static void draw_loading(DrawQueue& dq, SceneCamera const& camera)
    {
        auto color = img::to_pixel(0);
        auto dst = to_image_view(camera);
        img::fill(dst, color);
    }
    
    
    static void draw_centered(DrawQueue& dq, ImageView const& src, SceneCamera const& camera)
    {
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
    }


    static void update(StateData& data, InputCommand const& cmd)
    {
        auto src = data.ui.data.title;
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
            internal::draw_loading(dq, camera);
            break;

        case AssetStatus::Success:
            internal::draw_centered(dq, src, camera);
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