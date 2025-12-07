#pragma once
/* timestamp: 1765071867307273469 */


// bin_table_types.hpp

/* types */

namespace bin_table
{
    namespace img = image;

    using p32 = img::Pixel;
    using Image = img::Image;
    using ImageView = img::ImageView;
    using ImageGray = img::ImageGray;
    using GrayView = img::GrayView;


    enum class FileType : u8
    {
        Unknown = 0,
        Image4C,             // 4 channel image
		Image4C_Spritesheet, // 4 channel spritesheet
		Image4C_Tile,        // 4 channel tile

        Image1C,      // 1 channel image
        Music,
        SFX
    };


    class FileInfo_Image
	{
	public:
		FileType type = FileType::Unknown;
		u32 width = 0;
		u32 height = 0;
		cstr name = 0;
		u32 offset = 0;
		u32 size = 0;
	};


	inline constexpr FileInfo_Image to_file_info_image(FileType type, u32 width, u32 height, cstr name, u32 offset, u32 size)
	{
		FileInfo_Image f;
		f.type = type;
		f.width = width;
		f.height = height;
		f.name = name;
		f.offset = offset;
		f.size = size;

		return f;
	}
}


// auto-generated
namespace bin_table
{

	class InfoList_Image_Sky_Base
	{
	public:
		u32 offset = 0;
		u32 size = 466;

		static constexpr FileType file_type = FileType::Image4C;

		static constexpr u32 count = 2;
		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 324, 1, "base_day_png", 0, 264),
				to_file_info_image(file_type, 324, 1, "base_night_png", 264, 202),
			};

			struct
			{
				FileInfo_Image base_day_png;
				FileInfo_Image base_night_png;
			} file_info;
		};

		constexpr InfoList_Image_Sky_Base(){}
	};

}


// auto-generated
namespace bin_table
{

	class InfoList_Image_Sky_Overlay
	{
	public:
		u32 offset = 466;
		u32 size = 723893;

		static constexpr FileType file_type = FileType::Image1C;

		static constexpr u32 count = 1;
		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 1200, 1800, "ov_13_png", 466, 723893),
			};

			struct
			{
				FileInfo_Image ov_13_png;
			} file_info;
		};

		constexpr InfoList_Image_Sky_Overlay(){}
	};

}


// auto-generated
namespace bin_table
{

	class InfoList_Image_Sky_ColorTable
	{
	public:
		u32 offset = 724359;
		u32 size = 866;

		static constexpr FileType file_type = FileType::Image4C;

		static constexpr u32 count = 1;
		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 256, 1, "ct_13_png", 724359, 866),
			};

			struct
			{
				FileInfo_Image ct_13_png;
			} file_info;
		};

		constexpr InfoList_Image_Sky_ColorTable(){}
	};

}


// auto-generated
namespace bin_table
{

	class Background_Bg1
	{
	public:
		u32 offset = 725225;
		u32 size = 27113;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 8;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 324, 576, "A", 725225, 3189),
				to_file_info_image(file_type, 324, 576, "B", 728414, 3457),
				to_file_info_image(file_type, 324, 576, "C", 731871, 4126),
				to_file_info_image(file_type, 324, 576, "D", 735997, 2991),
				to_file_info_image(file_type, 324, 576, "E", 738988, 3561),
				to_file_info_image(file_type, 324, 576, "F", 742549, 3289),
				to_file_info_image(file_type, 324, 576, "G", 745838, 3391),
				to_file_info_image(file_type, 324, 576, "H", 749229, 3109),
			};

			struct
			{
				FileInfo_Image A;
				FileInfo_Image B;
				FileInfo_Image C;
				FileInfo_Image D;
				FileInfo_Image E;
				FileInfo_Image F;
				FileInfo_Image G;
				FileInfo_Image H;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 9, 1, "table", 752338, 98);

		constexpr Background_Bg1(){}
	};

}


// auto-generated
namespace bin_table
{

	class Background_Bg2
	{
	public:
		u32 offset = 752436;
		u32 size = 64972;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 16;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 324, 576, "A", 752436, 4922),
				to_file_info_image(file_type, 324, 576, "B", 757358, 3407),
				to_file_info_image(file_type, 324, 576, "C", 760765, 3977),
				to_file_info_image(file_type, 324, 576, "D", 764742, 4429),
				to_file_info_image(file_type, 324, 576, "E", 769171, 3323),
				to_file_info_image(file_type, 324, 576, "F", 772494, 4035),
				to_file_info_image(file_type, 324, 576, "G", 776529, 3399),
				to_file_info_image(file_type, 324, 576, "H", 779928, 3313),
				to_file_info_image(file_type, 324, 576, "I", 783241, 3597),
				to_file_info_image(file_type, 324, 576, "J", 786838, 3161),
				to_file_info_image(file_type, 324, 576, "K", 789999, 3912),
				to_file_info_image(file_type, 324, 576, "L", 793911, 4395),
				to_file_info_image(file_type, 324, 576, "M", 798306, 3962),
				to_file_info_image(file_type, 324, 576, "N", 802268, 6221),
				to_file_info_image(file_type, 324, 576, "O", 808489, 3651),
				to_file_info_image(file_type, 324, 576, "P", 812140, 5268),
			};

			struct
			{
				FileInfo_Image A;
				FileInfo_Image B;
				FileInfo_Image C;
				FileInfo_Image D;
				FileInfo_Image E;
				FileInfo_Image F;
				FileInfo_Image G;
				FileInfo_Image H;
				FileInfo_Image I;
				FileInfo_Image J;
				FileInfo_Image K;
				FileInfo_Image L;
				FileInfo_Image M;
				FileInfo_Image N;
				FileInfo_Image O;
				FileInfo_Image P;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 9, 1, "table", 817408, 98);

		constexpr Background_Bg2(){}
	};

}


// auto-generated
namespace bin_table
{

	class Spriteset_Punk
	{
	public:
		u32 offset = 817506;
		u32 size = 2586;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 2;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 48, 192, "Punk_idle", 817506, 858),
				to_file_info_image(file_type, 48, 288, "Punk_run", 818364, 1728),
			};

			struct
			{
				FileInfo_Image Punk_idle;
				FileInfo_Image Punk_run;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 14, 1, "table", 820092, 119);

		constexpr Spriteset_Punk(){}
	};

}


// auto-generated
namespace bin_table
{

	class Tileset_ex_zone
	{
	public:
		u32 offset = 820211;
		u32 size = 832;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 2;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 32, 32, "floor_02", 820211, 377),
				to_file_info_image(file_type, 32, 32, "floor_03", 820588, 369),
			};

			struct
			{
				FileInfo_Image floor_02;
				FileInfo_Image floor_03;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 5, 1, "table", 820957, 86);

		constexpr Tileset_ex_zone(){}
	};

}


// auto-generated
namespace bin_table
{

	class UIset_Font
	{
	public:
		u32 offset = 821043;
		u32 size = 3000;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 1;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 16, 806, "font", 821043, 2783),
			};

			struct
			{
				FileInfo_Image font;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 37, 1, "table", 823826, 217);

		constexpr UIset_Font(){}
	};

}


// auto-generated
namespace bin_table
{

	class UIset_Title
	{
	public:
		u32 offset = 824043;
		u32 size = 1403;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 1;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 74, 96, "title_main", 824043, 1321),
			};

			struct
			{
				FileInfo_Image title_main;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 4, 1, "table", 825364, 82);

		constexpr UIset_Title(){}
	};

}


