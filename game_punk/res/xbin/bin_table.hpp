#pragma once
/* timestamp: 1766156321447610749 */


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
		u32 size = 725104;

		static constexpr FileType file_type = FileType::Image1C;

		static constexpr u32 count = 1;
		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 1200, 1800, "ov_13_png", 466, 725104),
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
		u32 offset = 725570;
		u32 size = 850;

		static constexpr FileType file_type = FileType::Image4C;

		static constexpr u32 count = 1;
		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 256, 1, "ct_13_png", 725570, 850),
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
		u32 offset = 726420;
		u32 size = 27113;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 8;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 324, 576, "A", 726420, 3189),
				to_file_info_image(file_type, 324, 576, "B", 729609, 3457),
				to_file_info_image(file_type, 324, 576, "C", 733066, 4126),
				to_file_info_image(file_type, 324, 576, "D", 737192, 2991),
				to_file_info_image(file_type, 324, 576, "E", 740183, 3561),
				to_file_info_image(file_type, 324, 576, "F", 743744, 3289),
				to_file_info_image(file_type, 324, 576, "G", 747033, 3391),
				to_file_info_image(file_type, 324, 576, "H", 750424, 3109),
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

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 9, 1, "table", 753533, 98);

		constexpr Background_Bg1(){}
	};

}


// auto-generated
namespace bin_table
{

	class Background_Bg2
	{
	public:
		u32 offset = 753631;
		u32 size = 64972;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 16;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 324, 576, "A", 753631, 4922),
				to_file_info_image(file_type, 324, 576, "B", 758553, 3407),
				to_file_info_image(file_type, 324, 576, "C", 761960, 3977),
				to_file_info_image(file_type, 324, 576, "D", 765937, 4429),
				to_file_info_image(file_type, 324, 576, "E", 770366, 3323),
				to_file_info_image(file_type, 324, 576, "F", 773689, 4035),
				to_file_info_image(file_type, 324, 576, "G", 777724, 3399),
				to_file_info_image(file_type, 324, 576, "H", 781123, 3313),
				to_file_info_image(file_type, 324, 576, "I", 784436, 3597),
				to_file_info_image(file_type, 324, 576, "J", 788033, 3161),
				to_file_info_image(file_type, 324, 576, "K", 791194, 3912),
				to_file_info_image(file_type, 324, 576, "L", 795106, 4395),
				to_file_info_image(file_type, 324, 576, "M", 799501, 3962),
				to_file_info_image(file_type, 324, 576, "N", 803463, 6221),
				to_file_info_image(file_type, 324, 576, "O", 809684, 3651),
				to_file_info_image(file_type, 324, 576, "P", 813335, 5268),
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

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 9, 1, "table", 818603, 98);

		constexpr Background_Bg2(){}
	};

}


// auto-generated
namespace bin_table
{

	class Spriteset_Punk
	{
	public:
		u32 offset = 818701;
		u32 size = 2064;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 2;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 48, 192, "Punk_idle", 818701, 617),
				to_file_info_image(file_type, 48, 288, "Punk_run", 819318, 1447),
			};

			struct
			{
				FileInfo_Image Punk_idle;
				FileInfo_Image Punk_run;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 14, 1, "table", 820765, 119);

		constexpr Spriteset_Punk(){}
	};

}


// auto-generated
namespace bin_table
{

	class Tileset_ex_zone
	{
	public:
		u32 offset = 820884;
		u32 size = 832;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 2;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 32, 32, "floor_02", 820884, 377),
				to_file_info_image(file_type, 32, 32, "floor_03", 821261, 369),
			};

			struct
			{
				FileInfo_Image floor_02;
				FileInfo_Image floor_03;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 5, 1, "table", 821630, 86);

		constexpr Tileset_ex_zone(){}
	};

}


// auto-generated
namespace bin_table
{

	class UIset_Font
	{
	public:
		u32 offset = 821716;
		u32 size = 3000;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 1;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 16, 806, "font", 821716, 2783),
			};

			struct
			{
				FileInfo_Image font;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 37, 1, "table", 824499, 217);

		constexpr UIset_Font(){}
	};

}


// auto-generated
namespace bin_table
{

	class UIset_Title
	{
	public:
		u32 offset = 824716;
		u32 size = 1403;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 1;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 74, 96, "title_main", 824716, 1321),
			};

			struct
			{
				FileInfo_Image title_main;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 4, 1, "table", 826037, 82);

		constexpr UIset_Title(){}
	};

}


// auto-generated
namespace bin_table
{

	class UIset_Icons
	{
	public:
		u32 offset = 826119;
		u32 size = 4948;

		static constexpr FileType file_type = FileType::Image1C;
		static constexpr FileType table_type = FileType::Image4C;

		static constexpr u32 count = 1;

		union
		{
			FileInfo_Image items[count] = {
				to_file_info_image(file_type, 32, 1312, "icons", 826119, 4731),
			};

			struct
			{
				FileInfo_Image icons;
			} file_info;
		};

		static constexpr FileInfo_Image color_table = to_file_info_image(table_type, 37, 1, "table", 830850, 217);

		constexpr UIset_Icons(){}
	};

}


