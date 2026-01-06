namespace game_punk
{
namespace gm_title
{
namespace internal
{
    
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
        auto gameplay_ready = data.asset_data.status == AssetStatus::Success;

        if (gameplay_ready && cmd.title_ok)
        {
            set_game_mode(data, GameMode::Gameplay);
        }
    }
}
}