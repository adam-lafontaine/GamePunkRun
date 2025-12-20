#pragma once
/* timestamp: 1766193780892098583 */


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


	enum class FilterKey : u8
	{
		Transparent = 0,
		Secondary = 50,
		Blend = 128,
		Primary = 255
	};


	enum class FileType : u8
    {
        Unknown = 0,

        Image4C,             // 4 channel image
		Image4C_Table,

		Image4C_Spritesheet, // 4 channel spritesheet
		Image4C_Tile,        // 4 channel tile

        Image1C,      // 1 channel image
		Image1C_Mask,
		Image1C_Filter,
		Image1C_Table,

        Music,
        SFX
    };


	class FilterImage1C
	{
	public:
		static constexpr FileType type = FileType::Image1C_Filter;
		
		u32 width = 0;
		u32 height = 0;

		u8* data = 0;
	};


	class TableImage1C
	{
	public:
		static constexpr FileType type = FileType::Image1C_Table;

		u32 width = 0;
		u32 height = 0;

		u8* data = 0;
	};


	class MaskImage1C
	{
	public:
		static constexpr FileType type = FileType::Image1C_Mask;

		u32 width = 0;
		u32 height = 0;

		u8* data = 0;
	};


	class ColorTable4C
	{
	public:
		static constexpr FileType type = FileType::Image4C_Table;

		u32 length = 0;

		p32* data = 0;
	};
    


	template <u8 FT>
    class FileInfo_Image
	{
	public:
		static constexpr FileType type = (FileType)FT;

		u32 width = 0;
		u32 height = 0;
		cstr name = 0;
		u32 offset = 0;
		u32 size = 0;
	};


	template <u8 FT>
	inline constexpr FileInfo_Image<FT> to_file_info_image(u32 width, u32 height, cstr name, u32 offset, u32 size)
	{
		FileInfo_Image<FT> f;
		f.width = width;
		f.height = height;
		f.name = name;
		f.offset = offset;
		f.size = size;

		return f;
	}


	using ImageGrayInfo = FileInfo_Image<(u8)FileType::Image1C>;
	using ImageRGBAInfo = FileInfo_Image<(u8)FileType::Image4C>;
	using ColorTableInfo = FileInfo_Image<(u8)FileType::Image4C_Table>;
	using TableImageInfo = FileInfo_Image<(u8)FileType::Image1C_Table>;
	using FilterImageInfo = FileInfo_Image<(u8)FileType::Image1C_Filter>;
	using MaskImageInfo = FileInfo_Image<(u8)FileType::Image1C_Mask>;


	inline void destroy_image(auto& item)
	{
		switch (item.type)
		{
		case FileType::Image4C:
		case FileType::Image4C_Table:
		{
			Image image;
			image.data_ = (p32*)item.data;
			img::destroy_image(image);
		} break;

		case FileType::Image1C:
		case FileType::Image1C_Mask:
		case FileType::Image1C_Filter:
		case FileType::Image1C_Table:
		{
			ImageGray image;
			image.data_ = (u8*)item.data;
			img::destroy_image(image);
		} break;

		default:
			break;
		}
	}
}


/* read */

namespace bin_table
{
	enum class ReadResult : u8
	{
		OK = 0,
		Unsupported,
		ReadError,
		SizeError
	};


	inline ReadResult read_image(ByteView const& src, FileType type, ImageGray& dst)
	{
		switch (type)
		{
		case FileType::Image1C:
		case FileType::Image1C_Mask:
		case FileType::Image1C_Filter:
		case FileType::Image1C_Table:
			return img::read_image_from_memory(src, dst) ? ReadResult::OK : ReadResult::ReadError;

		default: return ReadResult::Unsupported;
		}
	}


	inline ReadResult read_image(ByteView const& src, FileType type, Image& dst)
	{
		switch (type)
		{
		case FileType::Image4C:
		case FileType::Image4C_Table:
			return img::read_image_from_memory(src, dst) ? ReadResult::OK : ReadResult::ReadError;

		default: return ReadResult::Unsupported;
		}
	}


	inline ReadResult read_gray(ByteView const& src, ImageGrayInfo info, ImageGray& dst)
	{
		static_assert(ImageGrayInfo::type == FileType::Image1C);

		return read_image(src, info.type, dst);
	}


	inline ReadResult read_rgba(ByteView const& src, ImageRGBAInfo info, Image& dst)
	{
		static_assert(ImageRGBAInfo::type == FileType::Image4C);

		return read_image(src, info.type, dst);
	}


	inline ReadResult read_color_table(ByteView const& src, ColorTableInfo info, ColorTable4C& out)
	{
		static_assert(ColorTableInfo::type == ColorTable4C::type);

		Image dst;
		auto res = read_image(src, info.type, dst);
		if (res != ReadResult::OK)
		{
			return res;
		}

		if (dst.width != info.width || dst.height != info.height || info.height != 1)
		{
			return ReadResult::SizeError;
		}

		out.length = dst.width;
		out.data = dst.data_;

		return ReadResult::OK;
	}


	inline ReadResult read_table_image(ByteView const& src, TableImageInfo info, TableImage1C& out)
	{
		static_assert(TableImageInfo::type == TableImage1C::type);

		ImageGray dst;
		auto res = read_image(src, info.type, dst);
		if (res != ReadResult::OK)
		{
			return res;
		}

		if (dst.width != info.width || dst.height != info.height)
		{
			return ReadResult::SizeError;
		}

		out.width = dst.width;
		out.height = dst.height;
		out.data = dst.data_;

		return ReadResult::OK;
	}


	inline ReadResult read_filter_image(ByteView const& src, FilterImageInfo info, FilterImage1C& out)
	{
		static_assert(FilterImageInfo::type == FilterImage1C::type);

		ImageGray dst;
		auto res = read_image(src, info.type, dst);
		if (res != ReadResult::OK)
		{
			return res;
		}

		if (dst.width != info.width || dst.height != info.height)
		{
			return ReadResult::SizeError;
		}

		out.width = dst.width;
		out.height = dst.height;
		out.data = dst.data_;
		
		return ReadResult::OK;
	}


	inline ReadResult read_mask_image(ByteView const& src, MaskImageInfo info, MaskImage1C& out)
	{
		static_assert(MaskImageInfo::type == MaskImage1C::type);

		ImageGray dst;
		auto res = read_image(src, info.type, dst);
		if (res != ReadResult::OK)
		{
			return res;
		}

		if (dst.width != info.width || dst.height != info.height)
		{
			return ReadResult::SizeError;
		}

		out.width = dst.width;
		out.height = dst.height;
		out.data = dst.data_;
		
		return ReadResult::OK;
	}
}


/* process */

namespace bin_table
{
	inline bool mask_convert(MaskImage1C const& src, ImageView const& dst)
	{
		if (src.width != dst.width || src.height != dst.height)
		{
			return false;
		}
		
		constexpr auto on = img::to_pixel(255);
		constexpr auto off = img::to_pixel(0, 0, 0, 0);

		auto length = src.width * src.height;
		auto s = src.data;
		auto d = dst.matrix_data_;

		// mask/filter preserved in alpha channel

		for (u32 i = 0; i < length; i++)
		{
			d[i] = s[i] == (u8)FilterKey::Primary ? on : off;
		}

		return true;
	}


	inline void mask_update(ImageView const& dst, p32 color)
	{
		constexpr auto off = img::to_pixel(0, 0, 0, 0);

		auto length = dst.width * dst.height;
		auto d = dst.matrix_data_;

		p32 p;

		for (u32 i = 0; i < length; i++)
		{
			p = d[i];
			d[i] = p.alpha == (u8)FilterKey::Primary ? color : off;
		}
	}


	inline bool filter_convert(FilterImage1C const& src, ImageView const& dst)
	{
		if (src.width != dst.width || src.height != dst.height)
		{
			return false;
		}

		constexpr auto white = img::to_pixel(255);

		auto length = src.width * src.height;
		auto s = src.data;
		auto d = dst.matrix_data_;

		// mask/filter preserved in alpha channel

		for (u32 i = 0; i < length; i++)
		{
			d[i] = white;
			d[i].alpha = s[i];
		}

		return true;
	}


	inline void filter_update(ImageView const& dst, p32 primary, p32 secondary)
	{
		constexpr auto off = img::to_pixel(0, 0, 0, 0);

		primary.alpha = 255; // no transparency allowed
        secondary.alpha = 255;

        auto blend_r = (primary.red + secondary.red) / 2;
        auto blend_g = (primary.green + secondary.green) / 2;
        auto blend_b = (primary.blue + secondary.blue) / 2;
        auto blend = img::to_pixel((u8)blend_r, (u8)blend_g, (u8)blend_b);

		auto length = dst.width * dst.height;
		auto d = dst.matrix_data_;

		p32 p;

		for (u32 i = 0; i < length; i++)
		{
			p = d[i];
			switch ((FilterKey)p.alpha)
			{
			case FilterKey::Transparent:
                d[i] = off;
                break;

            case FilterKey::Secondary:
                d[i] = secondary;
                break;

            case FilterKey::Blend:
                d[i] = blend;
                break;

            case FilterKey::Primary:
                d[i] = primary;
                break;

            default:
                break;
			}

			d[i].alpha = p.alpha;
		}
	}


	inline bool color_table_convert(TableImage1C const& src, ColorTable4C const& table, ImageView const& dst)
	{
		if (src.width != dst.width || src.height != dst.height)
		{
			return false;
		}

		auto length = src.width * src.height;
		auto s = src.data;
		auto d = dst.matrix_data_;

		for (u32 i = 0; i < length; i++)
		{
			d[i] = table.data[s[i]];
		}

		return true;
	}
}


// auto-generated
namespace bin_table
{

	// define_info_list_image()
	class InfoList_Image_Sky_Base
	{
	public:
		u32 offset = 0;
		u32 size = 466;

		static constexpr FileType file_type = FileType::Image4C;
		static constexpr auto uFT = (u8)file_type;
		using ImageInfo = FileInfo_Image<uFT>;

		static constexpr u32 count = 2;
		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(324, 1, "base_day_png", 0, 264),
				to_file_info_image<uFT>(324, 1, "base_night_png", 264, 202),
			};

			struct
			{
				ImageInfo base_day_png;
				ImageInfo base_night_png;
			} file_info;
		};

		constexpr InfoList_Image_Sky_Base(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_info_list_image()
	class InfoList_Image_Sky_Overlay
	{
	public:
		u32 offset = 466;
		u32 size = 725104;

		static constexpr FileType file_type = FileType::Image1C_Table;
		static constexpr auto uFT = (u8)file_type;
		using ImageInfo = FileInfo_Image<uFT>;

		static constexpr u32 count = 1;
		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(1200, 1800, "ov_13_png", 466, 725104),
			};

			struct
			{
				ImageInfo ov_13_png;
			} file_info;
		};

		constexpr InfoList_Image_Sky_Overlay(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_info_list_image()
	class InfoList_Image_Sky_ColorTable
	{
	public:
		u32 offset = 725570;
		u32 size = 850;

		static constexpr FileType file_type = FileType::Image4C_Table;
		static constexpr auto uFT = (u8)file_type;
		using ImageInfo = FileInfo_Image<uFT>;

		static constexpr u32 count = 1;
		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(256, 1, "ct_13_png", 725570, 850),
			};

			struct
			{
				ImageInfo ct_13_png;
			} file_info;
		};

		constexpr InfoList_Image_Sky_ColorTable(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_image_set()
	class Background_Bg1
	{
	public:
		u32 offset = 726420;
		u32 size = 27113;

		static constexpr FileType file_type = FileType::Image1C_Mask;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr auto uFT = (u8)file_type;
		static constexpr auto uTT = (u8)table_type;

		using ImageInfo = FileInfo_Image<uFT>;
		using TableInfo = FileInfo_Image<uTT>;

		static constexpr u32 count = 8;

		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(324, 576, "A", 726420, 3189),
				to_file_info_image<uFT>(324, 576, "B", 729609, 3457),
				to_file_info_image<uFT>(324, 576, "C", 733066, 4126),
				to_file_info_image<uFT>(324, 576, "D", 737192, 2991),
				to_file_info_image<uFT>(324, 576, "E", 740183, 3561),
				to_file_info_image<uFT>(324, 576, "F", 743744, 3289),
				to_file_info_image<uFT>(324, 576, "G", 747033, 3391),
				to_file_info_image<uFT>(324, 576, "H", 750424, 3109),
			};

			struct
			{
				ImageInfo A;
				ImageInfo B;
				ImageInfo C;
				ImageInfo D;
				ImageInfo E;
				ImageInfo F;
				ImageInfo G;
				ImageInfo H;
			} file_info;
		};

		static constexpr TableInfo color_table = to_file_info_image<uTT>(256, 1, "table", 753533, 114);

		constexpr Background_Bg1(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_image_set()
	class Background_Bg2
	{
	public:
		u32 offset = 753647;
		u32 size = 64972;

		static constexpr FileType file_type = FileType::Image1C_Mask;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr auto uFT = (u8)file_type;
		static constexpr auto uTT = (u8)table_type;

		using ImageInfo = FileInfo_Image<uFT>;
		using TableInfo = FileInfo_Image<uTT>;

		static constexpr u32 count = 16;

		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(324, 576, "A", 753647, 4922),
				to_file_info_image<uFT>(324, 576, "B", 758569, 3407),
				to_file_info_image<uFT>(324, 576, "C", 761976, 3977),
				to_file_info_image<uFT>(324, 576, "D", 765953, 4429),
				to_file_info_image<uFT>(324, 576, "E", 770382, 3323),
				to_file_info_image<uFT>(324, 576, "F", 773705, 4035),
				to_file_info_image<uFT>(324, 576, "G", 777740, 3399),
				to_file_info_image<uFT>(324, 576, "H", 781139, 3313),
				to_file_info_image<uFT>(324, 576, "I", 784452, 3597),
				to_file_info_image<uFT>(324, 576, "J", 788049, 3161),
				to_file_info_image<uFT>(324, 576, "K", 791210, 3912),
				to_file_info_image<uFT>(324, 576, "L", 795122, 4395),
				to_file_info_image<uFT>(324, 576, "M", 799517, 3962),
				to_file_info_image<uFT>(324, 576, "N", 803479, 6221),
				to_file_info_image<uFT>(324, 576, "O", 809700, 3651),
				to_file_info_image<uFT>(324, 576, "P", 813351, 5268),
			};

			struct
			{
				ImageInfo A;
				ImageInfo B;
				ImageInfo C;
				ImageInfo D;
				ImageInfo E;
				ImageInfo F;
				ImageInfo G;
				ImageInfo H;
				ImageInfo I;
				ImageInfo J;
				ImageInfo K;
				ImageInfo L;
				ImageInfo M;
				ImageInfo N;
				ImageInfo O;
				ImageInfo P;
			} file_info;
		};

		static constexpr TableInfo color_table = to_file_info_image<uTT>(256, 1, "table", 818619, 114);

		constexpr Background_Bg2(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_image_set()
	class Spriteset_Punk
	{
	public:
		u32 offset = 818733;
		u32 size = 2597;

		static constexpr FileType file_type = FileType::Image1C_Table;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr auto uFT = (u8)file_type;
		static constexpr auto uTT = (u8)table_type;

		using ImageInfo = FileInfo_Image<uFT>;
		using TableInfo = FileInfo_Image<uTT>;

		static constexpr u32 count = 2;

		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(48, 192, "Punk_idle", 818733, 855),
				to_file_info_image<uFT>(48, 288, "Punk_run", 819588, 1742),
			};

			struct
			{
				ImageInfo Punk_idle;
				ImageInfo Punk_run;
			} file_info;
		};

		static constexpr TableInfo color_table = to_file_info_image<uTT>(256, 1, "table", 821330, 134);

		constexpr Spriteset_Punk(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_image_set()
	class Tileset_ex_zone
	{
	public:
		u32 offset = 821464;
		u32 size = 848;

		static constexpr FileType file_type = FileType::Image1C_Table;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr auto uFT = (u8)file_type;
		static constexpr auto uTT = (u8)table_type;

		using ImageInfo = FileInfo_Image<uFT>;
		using TableInfo = FileInfo_Image<uTT>;

		static constexpr u32 count = 2;

		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(32, 32, "floor_02", 821464, 377),
				to_file_info_image<uFT>(32, 32, "floor_03", 821841, 369),
			};

			struct
			{
				ImageInfo floor_02;
				ImageInfo floor_03;
			} file_info;
		};

		static constexpr TableInfo color_table = to_file_info_image<uTT>(256, 1, "table", 822210, 102);

		constexpr Tileset_ex_zone(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_image_set()
	class UIset_Font
	{
	public:
		u32 offset = 822312;
		u32 size = 3014;

		static constexpr FileType file_type = FileType::Image1C_Filter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr auto uFT = (u8)file_type;
		static constexpr auto uTT = (u8)table_type;

		using ImageInfo = FileInfo_Image<uFT>;
		using TableInfo = FileInfo_Image<uTT>;

		static constexpr u32 count = 1;

		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(16, 806, "font", 822312, 2783),
			};

			struct
			{
				ImageInfo font;
			} file_info;
		};

		static constexpr TableInfo color_table = to_file_info_image<uTT>(256, 1, "table", 825095, 231);

		constexpr UIset_Font(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_image_set()
	class UIset_Title
	{
	public:
		u32 offset = 825326;
		u32 size = 1413;

		static constexpr FileType file_type = FileType::Image1C_Filter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr auto uFT = (u8)file_type;
		static constexpr auto uTT = (u8)table_type;

		using ImageInfo = FileInfo_Image<uFT>;
		using TableInfo = FileInfo_Image<uTT>;

		static constexpr u32 count = 1;

		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(74, 96, "title_main", 825326, 1321),
			};

			struct
			{
				ImageInfo title_main;
			} file_info;
		};

		static constexpr TableInfo color_table = to_file_info_image<uTT>(256, 1, "table", 826647, 92);

		constexpr UIset_Title(){}
	};

}


// auto-generated
namespace bin_table
{

	// define_image_set()
	class UIset_Icons
	{
	public:
		u32 offset = 826739;
		u32 size = 4962;

		static constexpr FileType file_type = FileType::Image1C_Filter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr auto uFT = (u8)file_type;
		static constexpr auto uTT = (u8)table_type;

		using ImageInfo = FileInfo_Image<uFT>;
		using TableInfo = FileInfo_Image<uTT>;

		static constexpr u32 count = 1;

		union
		{
			ImageInfo items[count] = {
				to_file_info_image<uFT>(32, 1312, "icons", 826739, 4731),
			};

			struct
			{
				ImageInfo icons;
			} file_info;
		};

		static constexpr TableInfo color_table = to_file_info_image<uTT>(256, 1, "table", 831470, 231);

		constexpr UIset_Icons(){}
	};

}


