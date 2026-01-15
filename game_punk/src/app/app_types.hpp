#pragma once

#include "../../res/xbin/bin_table.hpp"
#include "../../../libs/util/stack_buffer.hpp"


/* asset constants */

namespace game_punk
{
    namespace bt = bin_table;
    namespace sb = stack_buffer;


namespace cxpr
{

    constexpr Vec2Du32 background_dimensions()
    {
        constexpr bt::Background_Bg1 list;

        Vec2Du32 dims;
        
        dims.x = list.items[0].width;
        dims.y = list.items[0].height;

        return dims;
    }

    // rotated
    constexpr u32 GAME_BACKGROUND_WIDTH_PX = background_dimensions().y;
    constexpr u32 GAME_BACKGROUND_HEIGHT_PX = background_dimensions().x;


    constexpr u32 find_4k_scale()
    {
        auto w = WIDTH_4K;
        u32 n = 2;
        while (w >= GAME_BACKGROUND_WIDTH_PX)
        {
            w = WIDTH_4K / n++;
        }        

        return n + 1;
    }

    constexpr u32 SCALE_4K = find_4k_scale();

    constexpr u32 GAME_CAMERA_WIDTH_PX = WIDTH_4K / SCALE_4K;
    constexpr u32 GAME_CAMERA_HEIGHT_PX = HEIGHT_4K / SCALE_4K;


    constexpr Vec2Du32 sky_overlay_dimensions()
    {
        constexpr bt::SkyOverlay_overlay list;

        Vec2Du32 dims;
        
        dims.x = list.items[0].width;
        dims.y = list.items[0].height;

        return dims;
    }


    constexpr u32 SKY_OVERLAY_WIDTH_PX = sky_overlay_dimensions().x;
    constexpr u32 SKY_OVERLAY_HEIGHT_PX = sky_overlay_dimensions().y;


    template <class T>
    constexpr u32 color_table_size()
    {
        return T::color_table.width;
    }


    constexpr u32 BACKGROUND_1_COUNT = bt::Background_Bg1::count;
    constexpr u32 BACKGROUND_2_COUNT = bt::Background_Bg2::count;
    constexpr u32 BACKGROUND_COUNT_MAX = math::cxpr::max(BACKGROUND_1_COUNT, BACKGROUND_2_COUNT);

    constexpr u32 TILE_WIDTH = bt::Tileset_ex_zone().items[0].height;
    constexpr u32 TILE_HEIGHT = bt::Tileset_ex_zone().items[0].width;




} // cxpr    

}


/* helpers */

namespace game_punk
{
    template <typename T>
    static bool rect_intersect(Rect2D<T> const& a, Rect2D<T> const& b)
    {
        return 
            a.x_begin < b.x_end &&
            a.x_end > b.x_begin &&
            a.y_begin < b.y_end &&
            a.y_end > b.y_begin;
    }
    

    static Rect2Du32 clamp_rect(Rect2Di32 rect, Rect2Di32 out)
    {
        Rect2Du32 dr{};
        dr.x_begin = (u32)math::max(rect.x_begin, out.x_begin);
        dr.y_begin = (u32)math::max(rect.y_begin, out.y_begin);
        dr.x_end = (u32)math::min(rect.x_end, out.x_end);
        dr.y_end = (u32)math::min(rect.y_end, out.y_end);

        return dr;
    }


    static void add_pma(Span32 const& src, Span32 const& dst)
    {
        p32 ps;

        for (u32 i = 0; i < src.length; i++)
        {
            ps = src.data[i];
            auto& pd = dst.data[i];

            pd.red += ps.red;
            pd.green += ps.green;
            pd.blue += ps.blue;
        }
    }


    template <typename T>
    bool has_data(T const& item)
    {
        return !(!item.data);
    }


    bool has_data(ImageView const& view)
    {
        return !(!view.matrix_data_);
    }

}


/* span cxpr */

namespace game_punk
{
    template <u32 W, u32 H>
    class Span32_cxpr
    {
    public:
        static constexpr u32 length = W * H;
        p32* data = 0;

        constexpr Span32_cxpr(p32* pixels) { data = pixels; }
    };


    template <u32 W, u32 H>
    static inline Span32 to_span(Span32_cxpr<W, H> const& sp)
    {
        return span::make_view(sp.data, sp.length);
    }


    template <u32 W, u32 H>
    static inline void copy(Span32_cxpr<W, H> const& src, Span32_cxpr<W, H> const& dst)
    {
        span::copy(to_span(src), to_span(dst));
    }


    template <u32 W, u32 H>
    static inline Span32 sub_view(Span32_cxpr<W, H> const& src, u32 offset, u32 length)
    {
        return span::sub_view(to_span(src), offset, length);
    }
}


/* random */

namespace game_punk
{
    class Randomf32
    {
    public:
        static constexpr u32 capacity = 256;

        f32* values;

        u8 b_cursor = 0;
        u8 r_cursor = 0;

    };


    static void count_random(Randomf32& rng, MemoryCounts& counts)
    {
        add_count<f32>(counts, rng.capacity);
    }


    static bool create_random(Randomf32& rng, Memory& memory)
    {
        auto res = push_mem<f32>(memory, rng.capacity);

        if (res.ok)
        {
            rng.values = res.data;
        }

        return res.ok;
    }


    static void reset_random(Randomf32& rng)
    {
        rng.b_cursor = 0;
        rng.r_cursor = 0;

        math::rand_init();

        for (u32 i = 0; i < rng.capacity; i++)
        {
            rng.values[i] = math::rand(0.0f, 1.0f);
        }
    }


    static void refresh_random(Randomf32& rng)
    {
        for (u8 i = rng.b_cursor; i < rng.r_cursor; i++)
        {
            rng.values[i] = math::rand(0.0f, 1.0f);
        }

        rng.b_cursor = rng.r_cursor;
    }


    u32 next_random_u32(Randomf32& rng, u32 min, u32 max)
    {
        auto val = rng.values[rng.r_cursor++];
        app_assert(rng.r_cursor != rng.b_cursor && "*** Frame RNG exceded ***");

        auto delta = val * (max - min) + 0.5f;

        return min + (u32)delta;
    }
}


/* integral types */

namespace game_punk
{
    class GameTick64
    {
    private:

        constexpr GameTick64(u64 v) { static_assert(sizeof(GameTick64) == 8); value_ = v; }

    public:

        u64 value_ = 0;

        GameTick64() = delete;

        static constexpr GameTick64 make(u64 v) { return GameTick64(v); }

        static constexpr GameTick64 zero() { return make(0u); }

        static constexpr GameTick64 none() { return make((u64)0 - (u64)1); }

        static constexpr GameTick64 forever() { return make((u64)0 - (u64)2); }
        

        GameTick64& operator ++ () { ++value_; return *this; }

        bool operator == (GameTick64 other) const { return value_ == other.value_; }

        bool operator >= (GameTick64 other) const { return value_ >= other.value_; }
        bool operator <= (GameTick64 other) const { return value_ <= other.value_; }

        bool operator < (GameTick64 other) const { return value_ < other.value_; }
        bool operator > (GameTick64 other) const { return value_ > other.value_; }
        
    };


    class TickQty32
    {
    public:
        
        u32 value_ = 0;
    

        TickQty32() { value_ = 0; }

        constexpr TickQty32(u32 v) { value_ = v; }


        static constexpr TickQty32 make(u32 v) { return TickQty32(v); }

        static constexpr TickQty32 zero() { return TickQty32(0u); }


        TickQty32& operator = (GameTick64 other) { value_ = other.value_; return *this; }

        //TickQty32& operator += (TickQty32 other) { value_ += other.value_; return *this; }

        bool operator == (TickQty32 other) const { return value_ == other.value_; }

        bool operator < (TickQty32 other) const { return value_ < other.value_; }

        bool operator <= (TickQty32 other) const { return value_ <= other.value_; }

        bool operator >= (TickQty32 other) const { return value_ >= other.value_; }


        static TickQty32 random(Randomf32& rng, u32 min, u32 max) { return TickQty32(next_random_u32(rng, min, max)); }
    };


    GameTick64 operator + (GameTick64 lhs, TickQty32 rhs) { return GameTick64::make(lhs.value_ + rhs.value_); }

    TickQty32 operator + (TickQty32 lhs, TickQty32 rhs) { return lhs.value_ + rhs.value_; }

    TickQty32 operator - (GameTick64 lhs, GameTick64 rhs) { return lhs.value_ - rhs.value_; }

    TickQty32 operator - (GameTick64 lhs, TickQty32 rhs) { return lhs.value_ - rhs.value_; }

    TickQty32 operator % (GameTick64 lhs, TickQty32 rhs) { return lhs.value_ % rhs.value_; }


    bool operator <= (TickQty32 lhs, GameTick64 rhs) { return lhs.value_ <= rhs.value_; }

    //bool operator >= (TickQty lhs, u32 rhs) { return lhs.value_ >= (u64)rhs; }

    

    TickQty32 operator - (TickQty32 lhs, TickQty32 rhs) { return lhs.value_ - rhs.value_; }


    template <typename uT, u64 N>
    class uN2
    {
    public:
        static constexpr uT COUNT = (uT)N;
        static constexpr uT MAX_VALUE = COUNT - 1;

        static constexpr auto is_valid = math::cxpr::is_unsigned<uT>() && math::cxpr::is_power_of_2(N);

    private:
        static uT n2_add(uT a, uT b) { static_assert(is_valid); return (a + b) & MAX_VALUE; }
        static uT n2_sub(uT a, uT b) { static_assert(is_valid); return (a - b) & MAX_VALUE; }

    public:
        uT value_ = 0;


        void reset() 
        { 
            static_assert(is_valid);

            value_ = 0; 
        }


        uN2& operator ++ () { value_ = n2_add(value_, 1); return *this; }
    };
    
}


/* circular buffer */

namespace game_punk
{
    template <typename T, u32 COUNT>
    class RingStackBuffer
    {
    public:
        static constexpr u32 count = COUNT;

        T data[COUNT];

        uN2<u32, COUNT> cursor;

        T& front() { return data[cursor.value_]; }

        void next() { ++cursor; }        
    };


    template <typename T, u32 COUNT>
    class RandomStackBuffer
    {
    public:
        static constexpr u32 capacity = COUNT;

        u32 size = 0;

        T data[COUNT];

        u32 id = 0;

        T& get(Randomf32& rng) 
        { 
            app_assert(size > 0 && "*** size not set ***");
            app_assert(size <= capacity && "*** size too large ***");
            id = next_random_u32(rng, 0, size - 1); 
            return data[id]; 
        }

        void set(T value) { data[id] = value; }
    };


    
}


/* object table */

namespace game_punk
{
    template <typename T, u32 TAG>
    class ObjectTable
    {
    public:
        static constexpr u32 tag = TAG;

        struct ID { u16 value_; };

        u32 capacity = 0;
        u32 size = 0;

        T empty_item;

        T* data;


        ID push_item(T const& obj)
        {
            ID id;
            id.value_ = capacity;
            
            if (size < capacity)
            {
                id.value_ = size;
                data[size++] = obj;
            }

            return id;
        }


        ID push()
        {
            ID id;
            id.value_ = capacity;
            
            if (size < capacity)
            {
                id.value_ = size;
                data[size++] = empty_item;
            }

            return id;
        }


        T& item_at(ID id)
        {
            if (id.value_ >= capacity)
            {
                return empty_item;
            }

            return data[id.value_];
        }
    };


    template <typename T, u32 TAG>
    static void reset_table(ObjectTable<T, TAG>& table)
    {
        table.size = 0;
        for (u32 i = 0; i < table.capacity; i++)
        {
            table.data[i] = table.empty_item;
        }
    }


    template <typename T, u32 TAG>
    static void count_table(ObjectTable<T, TAG>& table, MemoryCounts& counts, u32 capacity)
    {
        table.capacity = capacity;

        add_count<T>(counts, capacity);
    }


    template <typename T, u32 TAG>
    static bool create_table(ObjectTable<T, TAG>& table, Memory& memory)
    {
        if (!table.capacity)
        {
            app_crash("*** ObjectTable not initialized ***");
            return false;
        }

        auto res = push_mem<T>(memory, table.capacity);
        if (res.ok)
        {
            table.data = res.data;
        }

        return res.ok;
    }


    using BitmapTable = ObjectTable<ImageView, 0>;
    using BitmapID = BitmapTable::ID;
}


/* orientation context */

namespace game_punk
{
    enum class DimCtx
    {
        Proc,
        Game
    };
    
    
    class ContextDims
    {
    public:
        union
        {
            struct { u32 width; u32 height; } proc;

            struct { u32 height; u32 width; } game;

            u64 any = 0;
        };

        constexpr ContextDims(u32 w, u32 h, DimCtx ctx)
        {
            if (ctx == DimCtx::Proc)
            {
                proc.width = w;
                proc.height = h;
            }
            else
            {
                game.width = w;
                game.height = h;
            }
        }


        ContextDims() { any = 0; }
    };


    template <typename T>
    class ContextPosition
    {
    public:
        union { Point2D<T> proc; struct { T y; T x; } game; };

        ContextPosition(T x, T y, DimCtx ctx)
        {
            if (ctx == DimCtx::Proc)
            {
                proc.x = x;
                proc.y = y;
            }
            else
            {
                game.x = x;
                game.y = y;
            }
        }


        ContextPosition(Vec2D<T> pos, DimCtx ctx)
        {
            if (ctx == DimCtx::Proc)
            {
                proc.x = pos.x;
                proc.y = pos.y;
            }
            else
            {
                game.x = pos.x;
                game.y = pos.y;
            }
        }


        Vec2D<T> pos_game() { return { game.x, game.y }; }
    };


    using GamePosition = ContextPosition<i64>;
    using ScenePosition = ContextPosition<i32>;

    
    static Point2Di32 delta_pos_px(ScenePosition a, ScenePosition b)
    {
        Point2Di32 p;
        p.x = a.proc.x - b.proc.x;
        p.y = a.proc.y - b.proc.y;

        return p;
    }


    constexpr auto BACKGROUND_DIMS = ContextDims(cxpr::GAME_BACKGROUND_WIDTH_PX, cxpr::GAME_BACKGROUND_HEIGHT_PX, DimCtx::Game);

    constexpr auto CAMERA_DIMS = ContextDims(cxpr::GAME_CAMERA_WIDTH_PX, cxpr::GAME_CAMERA_HEIGHT_PX, DimCtx::Game);

    constexpr auto SCENE_DIMS = BACKGROUND_DIMS;


}


/* game scene */

namespace game_punk
{
    class GameScene
    {
    public:
        static constexpr auto dims = SCENE_DIMS;

        GamePosition game_position;
    };


    static void reset_game_scene(GameScene& scene)
    {
        scene.game_position = GamePosition(0, 0, DimCtx::Game);
    }


    static ScenePosition delta_pos_scene(GamePosition const& pos, GameScene const& scene)
    {
        constexpr u32 dmax = 10 * math::cxpr::max(cxpr::GAME_BACKGROUND_WIDTH_PX, cxpr::GAME_BACKGROUND_HEIGHT_PX);
        
        auto dx = pos.proc.x - scene.game_position.proc.x;
        auto dy = pos.proc.y - scene.game_position.proc.y;

        app_assert(math::abs(dx) < dmax && math::abs(dy) < dmax);

        return ScenePosition((i32)dx, (i32)dy, DimCtx::Proc);        
    }
}


/* scene camera */

namespace game_punk
{     
    class SceneCamera
    {
    public:
        static constexpr auto dims = CAMERA_DIMS;

        p32* pixels;

        ScenePosition scene_position;

        u8 speed_px;
    };


    static bool init_screen_camera(SceneCamera& camera, ImageView screen)
    {
        bool ok = true;

        auto dims = camera.dims.proc;

        ok &= has_data(screen);
        ok &= screen.width == dims.width;
        ok &= screen.height == dims.height;

        if (ok)
        {
            camera.pixels = screen.matrix_data_;
        }

        return ok;
    }


    static void reset_screen_camera(SceneCamera& camera)
    {
        camera.scene_position = ScenePosition(10, 16, DimCtx::Game);
        camera.speed_px = 2;
    }


    static void move_camera(SceneCamera& camera, Vec2Di8 delta_px)
    {        
        auto cam_dims = CAMERA_DIMS.game;
        auto& pos = camera.scene_position.game;

        auto pos_x = (i32)pos.x + delta_px.x;
        auto pos_y = (i32)pos.y + delta_px.y;

        auto max_dims = SCENE_DIMS.game;

        auto x_max = (i32)(max_dims.width - cam_dims.width);
        auto y_max = (i32)(max_dims.height - cam_dims.height);

        pos.x = (u32)math::cxpr::clamp(pos_x, 0, x_max);
        pos.y = (u32)math::cxpr::clamp(pos_y, 0, y_max);
    }


    static ImageView to_image_view(SceneCamera const& camera)
    {
        ImageView view;

        auto dims = CAMERA_DIMS.proc;

        view.width = dims.width;
        view.height = dims.height;
        view.matrix_data_ = camera.pixels;

        return view;
    }


    static Span32 to_span(SceneCamera const& camera)
    {
        Span32 view;
        auto dims = CAMERA_DIMS.proc;
        auto length = dims.width * dims.height;

        return span::make_view(camera.pixels, length);
    }
}


/* asset data */

namespace game_punk
{
    enum class AssetStatus : u8
    {
        None = 0,
        Loading,
        Success,

        FailLoad,
        FailRead
    };


    class AssetData
    {
    public:
        AssetStatus status = AssetStatus::None;

        cstr bin_file_path = 0;

        MemoryBuffer<u8> bytes;
    };


    static void destroy_asset_data(AssetData& gd)
    {
        mb::destroy_buffer(gd.bytes);
    }


    class LoadContext
    {
    public:
        ImageView dst;
        p32 color = COLOR_BLACK;
        u32 item_id = 0;
    };
    
    
    using OnAssetLoad = void (*)(Buffer8 const&, LoadContext const&);

    
    class LoadAssetCommand
    {
    public:
        b8 is_active = 0;

        LoadContext ctx;

        OnAssetLoad on_load;
    };
    
    
    class LoadAssetQueue
    {
    public:
        u32 capacity = 0;
        u32 size = 0;

        LoadAssetCommand* commands;
    };


    void count_queue(LoadAssetQueue& q, MemoryCounts& counts, u32 capacity)
    {
        q.capacity = capacity;
        add_count<LoadAssetCommand>(counts, capacity);
    }


    bool create_queue(LoadAssetQueue& q, Memory& mem)
    {      
        if (!q.capacity)
        {
            app_assert("LoadAssetQueue not initialized" && false);
            return false;
        }

        auto res = push_mem<LoadAssetCommand>(mem, q.capacity);

        auto ok = res.ok;

        if (ok)
        {
            q.commands = res.data;
        }

        return ok;
    }


    static void push_load(LoadAssetQueue& q, LoadAssetCommand cmd)
    {
        auto i = q.size;

        if (i < q.capacity && cmd.is_active)
        {
            q.commands[i] = cmd;
            q.size++;
        }        
    }


    static void load_all(AssetData const& src, LoadAssetQueue& q)
    {
        for (u32 i = 0; i < q.size; i++)
        {
            auto& cmd = q.commands[i];            
            cmd.on_load(src.bytes, cmd.ctx);
        }

        q.size = 0;
    }


    template <class LIST>
    static void load_background_image(Buffer8 const& buffer, LoadContext const& ctx)
    {
        constexpr LIST list;

        auto item = static_cast<LIST::Items>(ctx.item_id);
        auto filter = list.read_alpha_filter_item(buffer, item);
        bt::alpha_filter_convert(filter, ctx.dst, ctx.color);
        filter.destroy();

        //img::fill_row(ctx.dst, 0, img::to_pixel(255, 0, 0));
    }


    template <class LIST>
    static void load_spritesheet_image(Buffer8 const& buffer, LoadContext const& ctx)
    {
        constexpr LIST list;

        auto table = list.read_table(buffer);
        auto item = static_cast<LIST::Items>(ctx.item_id);
        auto filter = list.read_table_filter_item(buffer, item);
        bt::color_table_convert(filter, table, ctx.dst);

        table.destroy();
        filter.destroy();
    }


    template <class LIST>
    static void load_tile_image(Buffer8 const& buffer, LoadContext const& ctx)
    {
        constexpr LIST list;

        auto table = list.read_table(buffer);
        auto item = static_cast<LIST::Items>(ctx.item_id);
        auto filter = list.read_table_filter_item(buffer, item);
        bt::color_table_convert(filter, table, ctx.dst);

        table.destroy();
        filter.destroy();
    }
}


/* input command */

namespace game_punk
{
    class InputCommand
    {
    public:
    
        b8 action = 0;
    
        // gameplay
        union
        {
            b8 move = 0;

            struct
            {
                b8 up : 1;
                b8 down : 1;
                b8 left : 1;
                b8 right : 1;
            };

        } camera;
    };


    static InputCommand map_input(Input const& input)
    {
        auto& kbd = input.keyboard;
        auto& gpd = input.gamepad;

        InputCommand cmd;

        cmd.action = 
            kbd.kbd_return.pressed || 
            kbd.kbd_space.pressed ||
            gpd.btn_south.pressed;

        cmd.camera.move = 0;
        cmd.camera.up = kbd.kbd_up.is_down || gpd.btn_dpad_up.is_down;
        cmd.camera.down = kbd.kbd_down.is_down || gpd.btn_dpad_down.is_down;
        cmd.camera.right =  kbd.kbd_right.is_down || gpd.btn_dpad_right.is_down;
        cmd.camera.left = kbd.kbd_left.is_down || gpd.btn_dpad_left.is_down;

        //cmd.camera.move = 0; // disable

        return cmd;
    }
}