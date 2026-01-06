namespace game_punk
{
namespace gm_title
{
namespace internal
{
    static void draw_centered(DrawQueue& dq, ImageView const& src, SceneCamera const& camera)
    {
        auto dst = to_image_view(camera);

        Point2Di32 pos;
        pos.x = (dst.width - src.width) / 2;
        pos.y = (dst.height - src.height) / 2;

        push_draw_view(dq, src, dst, pos);

        auto color = img::pixel_at(src, 0, 0);
        img::fill(dst, color);
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
        
    }


    static void update(StateData& data, InputCommand const& cmd)
    {
        auto src = data.ui.data.title;
        auto& dq = data.drawq;
        auto& camera = data.camera;

        internal::draw_centered(dq, src, camera);
        
        auto gameplay_ready = data.asset_data.status == AssetStatus::Success;
        if (gameplay_ready && cmd.title_ok)
        {
            set_game_mode(data, GameMode::Gameplay);
        }
    }
}
}