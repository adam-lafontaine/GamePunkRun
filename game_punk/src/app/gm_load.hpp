namespace game_punk
{
namespace gm_load
{
namespace internal
{

}
}
}


namespace game_punk
{
namespace gm_load
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
        switch (data.asset_data.status)
        {
        case AssetStatus::None:
        case AssetStatus::FailLoad:
        case AssetStatus::FailRead:
            set_game_mode(data, GameMode::Error);
            break;

        case AssetStatus::Success:            
            set_game_mode(data, GameMode::Title);
            break;

        case AssetStatus::Loading: return;
        }
    }
}
}