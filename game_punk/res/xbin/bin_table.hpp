#pragma once
/* timestamp: 1767887394904463395 */


// bin_table_types.hpp

#ifndef app_assert
#include <cassert>
#define app_assert(condition) assert(condition)
#endif

#ifndef app_log
#include <cstdio>
#define app_log(...) printf(__VA_ARGS__)
#endif

#ifndef app_crash
#define app_crash(message) assert(false && message)
#endif

/* types */

namespace bin_table
{
    namespace img = image;

    using p32 = img::Pixel;
    using Image = img::Image;
    using ImageView = img::ImageView;
    using ImageGray = img::ImageGray;
    using GrayView = img::GrayView;
	using Buffer8 = img::Buffer8;
	using imgMode = img::ModeRW;


	enum class AlphaFilter : u8
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

        Image1C,      // 1 channel image
		Image1C_AlphaFilter,
		Image1C_TableFilter,

        Music,
        SFX
    };


	class AlphaFilterImage
	{
	public:
		static constexpr FileType type = FileType::Image1C_AlphaFilter;

		ImageGray gray;

		void destroy() { img::destroy_image(gray); }
	};


	class TableFilterImage
	{
	public:
		static constexpr FileType type = FileType::Image1C_TableFilter;

		ImageGray gray;

		void destroy() { img::destroy_image(gray); }
	};


	class ColorTableImage
	{
	public:
		static constexpr FileType type = FileType::Image4C_Table;

		Image rgba;

		p32 at(u32 id) { return rgba.data_[id]; }

		void destroy() { img::destroy_image(rgba); }
	};
	
}


/* helpers */


namespace bin_table
{
	inline constexpr u32 data_size(FileType type)
	{
		switch (type)
		{
		case FileType::Image4C:
		case FileType::Image4C_Table:
			return sizeof(p32);

		case FileType::Image1C:
		case FileType::Image1C_AlphaFilter:
		case FileType::Image1C_TableFilter:
			return sizeof(u8);

		default:
			return 0;
		}
	}


	auto item_at(auto const& list, auto key) { return list.items[(u32) key]; }
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
	

	class AssetInfo_Image
	{
	public:
		FileType type = FileType::Unknown;

		u32 width = 0;
		u32 height = 0;
		cstr name = 0;
		u32 offset = 0;
		u32 size = 0;
	};


	inline constexpr AssetInfo_Image to_image_info(FileType type, u32 width, u32 height, cstr name, u32 offset, u32 size)
	{
		AssetInfo_Image f;
		f.type = type;
		f.width = width;
		f.height = height;
		f.name = name;
		f.offset = offset;
		f.size = size;

		return f;
	}


	static ByteView make_byte_view(Buffer8 const& buffer, AssetInfo_Image const& info)
    {
        ByteView view{};

        view.data = buffer.data_ + info.offset;
        view.length = info.size;

        return view;
    }


	inline ReadResult read_image(Buffer8 const& buffer, AssetInfo_Image const& info, ImageGray& dst, imgMode mode)
	{
		auto src = make_byte_view(buffer, info);

		bool ok = false;

		switch (info.type)
		{
		case FileType::Image1C:
		case FileType::Image1C_AlphaFilter:
		case FileType::Image1C_TableFilter:
			ok = img::read_image_from_memory(src, dst);
			break;

		default: return ReadResult::Unsupported;
		}

		if (!ok)
		{
			return ReadResult::ReadError;
		}

		ok &= dst.width == info.width;
		ok &= dst.height == info.height;
		if (!ok)
		{
			return ReadResult::SizeError;
		}

		return ReadResult::OK;
	}


	inline ReadResult read_image(Buffer8 const& buffer, AssetInfo_Image const& info, Image& dst, imgMode mode)
	{
		auto src = make_byte_view(buffer, info);
		
		bool ok = false;

		switch (info.type)
		{
		case FileType::Image4C:
		case FileType::Image4C_Table:
			ok = img::read_image_from_memory(src, dst);
			break;

		default: return ReadResult::Unsupported;
		}

		if (!ok)
		{
			return ReadResult::ReadError;
		}

		ok &= dst.width == info.width;
		ok &= dst.height == info.height;
		if (!ok)
		{
			return ReadResult::SizeError;
		}

		return ReadResult::OK;
	}


	inline bool test_read(Buffer8 const& buffer, AssetInfo_Image const& info)
	{
		bool ok = false;

		auto mode = imgMode::None;

		switch (data_size(info.type))
		{
		case data_size(FileType::Image4C):
		{
			Image rgba;
			ok = read_image(buffer, info, rgba, mode) == ReadResult::OK;
			img::destroy_image(rgba);
		} break;

		case data_size(FileType::Image1C):
		{
			ImageGray gray;
			ok = read_image(buffer, info, gray, mode) == ReadResult::OK;
			img::destroy_image(gray);
		}break;

		default:
			break;
		}

		if (!ok)
		{		
			app_log("Asset read error: %s\n", info.name);
			app_crash("Asset read error");
		}

		return ok;
	}


	inline bool test_items(Buffer8 const& buffer, AssetInfo_Image* items, u32 count)
	{
		bool ok = true;

		for (u32 i = 0; i < count; i++)
		{
			ok &= test_read(buffer, items[i]);
		}

		return ok;
	}


	inline Image read_rgba(Buffer8 const& buffer, AssetInfo_Image const& info, imgMode mode = imgMode::None)
	{
		Image rgba;
		read_image(buffer, info, rgba, mode);
		return rgba;
	}


	inline ImageGray read_gray(Buffer8 const& buffer, AssetInfo_Image const& info, imgMode mode = imgMode::None)
	{
		ImageGray gray;
		read_image(buffer, info, gray, mode);
		return gray;
	}


	AlphaFilterImage read_alpha_filter(Buffer8 const& buffer, AssetInfo_Image const& info, imgMode mode = imgMode::None)
	{
		AlphaFilterImage filter;
		read_image(buffer, info, filter.gray, mode);
		return filter;
	}


	TableFilterImage read_table_filter(Buffer8 const& buffer, AssetInfo_Image const& info, imgMode mode = imgMode::None)
	{
		TableFilterImage filter;
		read_image(buffer, info, filter.gray, mode);
		return filter;
	}


	ColorTableImage read_color_table(Buffer8 const& buffer, AssetInfo_Image const& info, imgMode mode = imgMode::None)
	{
		ColorTableImage table;
		read_image(buffer, info, table.rgba, mode);
		return table;
	}
}


/* process */

namespace bin_table
{
	inline bool alpha_filter_convert(AlphaFilterImage const& filter, ImageView const& dst)
	{
		auto& src = filter.gray;
		
		if (src.width != dst.width || src.height != dst.height)
		{
			return false;
		}

		constexpr auto white = img::to_pixel(255);

		auto length = src.width * src.height;
		auto s = src.data_;
		auto d = dst.matrix_data_;

		for (u32 i = 0; i < length; i++)
		{
			d[i] = white;

			// filter preserved in alpha channel
			d[i].alpha = s[i];
		}

		return true;
	}


	inline bool alpha_filter_convert(AlphaFilterImage const& filter, ImageView const& dst, p32 primary)
	{
		auto& src = filter.gray;
		
		if (src.width != dst.width || src.height != dst.height)
		{
			return false;
		}

		constexpr auto off = img::to_pixel(0, 0, 0, 0);
		primary.alpha = 255; // no transparency allowed

		auto length = src.width * src.height;
		auto s = src.data_;
		auto d = dst.matrix_data_;

		u8 alpha = 0;

		for (u32 i = 0; i < length; i++)
		{
			alpha = s[i];
			switch ((AlphaFilter)alpha)
			{
            case AlphaFilter::Primary:
                d[i] = primary;
                break;

            default:
				d[i] = off;
                break;
			}

			// filter preserved in alpha channel
			d[i].alpha = alpha;
		}

		return true;
	}


	inline void alpha_filter_update(ImageView const& dst, p32 primary, p32 secondary)
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
			switch ((AlphaFilter)p.alpha)
			{
			case AlphaFilter::Transparent:
                d[i] = off;
                break;

            case AlphaFilter::Secondary:
                d[i] = secondary;
                break;

            case AlphaFilter::Blend:
                d[i] = blend;
                break;

            case AlphaFilter::Primary:
                d[i] = primary;
                break;

            default:
                break;
			}

			d[i].alpha = p.alpha;
		}
	}


	inline bool color_table_convert(TableFilterImage const& filter, ColorTableImage const& table, ImageView const& dst)
	{
		auto& src = filter.gray;

		if (src.width != dst.width || src.height != dst.height)
		{
			return false;
		}

		auto length = src.width * src.height;
		auto s = src.data_;
		auto d = dst.matrix_data_;
		auto t = table.rgba.data_;

		for (u32 i = 0; i < length; i++)
		{
			d[i] = t[s[i]];
		}

		return true;
	}
}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX)
	class SkyBase_base
	{
	public:
		using ImageInfo = AssetInfo_Image;

		static constexpr u32 offset = 0;
		static constexpr u32 size = 466;

		static constexpr FileType file_type = FileType::Image4C;
		static constexpr u32 count = 2;

		ImageInfo items[count] = {
			to_image_info(file_type, 324, 1, "day", 0, 264),
			to_image_info(file_type, 324, 1, "night", 264, 202),
		};

		enum class Items : u32
		{
			day,
			night,
		};


		constexpr SkyBase_base(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count);
		}


		Image read_rgba_item(Buffer8 const& buffer, Items key) const
		{
			return read_rgba(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_TableX)
	class SkyOverlay_overlay
	{
	public:
		static constexpr u32 offset = 466;
		static constexpr u32 size = 725954;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_TableFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 1;

		ImageInfo items[count] = {
			to_image_info(file_type, 1200, 1800, "ov_13", 466, 725104),
		};

		enum class Items : u32
		{
			ov_13,
		};


		ImageInfo table_items[count] = {
			to_image_info(table_type, 256, 1, "ov_13", 725570, 850),
		};


		constexpr SkyOverlay_overlay(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_items(buffer, (ImageInfo*)table_items, count);
		}


		TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(TableFilterImage::type == file_type);
			return read_table_filter(buffer, items[(u32)key]);
		}


		ColorTableImage read_table_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, table_items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_Table1)
	class Background_Bg1
	{
	public:
		static constexpr u32 offset = 726420;
		static constexpr u32 size = 27113;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_AlphaFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 8;

		ImageInfo items[count] = {
			to_image_info(file_type, 324, 576, "A", 726420, 3189),
			to_image_info(file_type, 324, 576, "B", 729609, 3457),
			to_image_info(file_type, 324, 576, "C", 733066, 4126),
			to_image_info(file_type, 324, 576, "D", 737192, 2991),
			to_image_info(file_type, 324, 576, "E", 740183, 3561),
			to_image_info(file_type, 324, 576, "F", 743744, 3289),
			to_image_info(file_type, 324, 576, "G", 747033, 3391),
			to_image_info(file_type, 324, 576, "H", 750424, 3109),
		};

		enum class Items : u32
		{
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
		};


		static constexpr ImageInfo color_table = to_image_info(table_type, 256, 1, "table", 753533, 114);

		constexpr Background_Bg1(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);
		}


		ColorTableImage read_table(Buffer8 const& buffer) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, color_table);
		}


		AlphaFilterImage read_alpha_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(AlphaFilterImage::type == file_type);
			return read_alpha_filter(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_Table1)
	class Background_Bg2
	{
	public:
		static constexpr u32 offset = 753647;
		static constexpr u32 size = 64972;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_AlphaFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 16;

		ImageInfo items[count] = {
			to_image_info(file_type, 324, 576, "A", 753647, 4922),
			to_image_info(file_type, 324, 576, "B", 758569, 3407),
			to_image_info(file_type, 324, 576, "C", 761976, 3977),
			to_image_info(file_type, 324, 576, "D", 765953, 4429),
			to_image_info(file_type, 324, 576, "E", 770382, 3323),
			to_image_info(file_type, 324, 576, "F", 773705, 4035),
			to_image_info(file_type, 324, 576, "G", 777740, 3399),
			to_image_info(file_type, 324, 576, "H", 781139, 3313),
			to_image_info(file_type, 324, 576, "I", 784452, 3597),
			to_image_info(file_type, 324, 576, "J", 788049, 3161),
			to_image_info(file_type, 324, 576, "K", 791210, 3912),
			to_image_info(file_type, 324, 576, "L", 795122, 4395),
			to_image_info(file_type, 324, 576, "M", 799517, 3962),
			to_image_info(file_type, 324, 576, "N", 803479, 6221),
			to_image_info(file_type, 324, 576, "O", 809700, 3651),
			to_image_info(file_type, 324, 576, "P", 813351, 5268),
		};

		enum class Items : u32
		{
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
		};


		static constexpr ImageInfo color_table = to_image_info(table_type, 256, 1, "table", 818619, 114);

		constexpr Background_Bg2(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);
		}


		ColorTableImage read_table(Buffer8 const& buffer) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, color_table);
		}


		AlphaFilterImage read_alpha_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(AlphaFilterImage::type == file_type);
			return read_alpha_filter(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_Table1)
	class Spriteset_Punk
	{
	public:
		static constexpr u32 offset = 818733;
		static constexpr u32 size = 2597;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_TableFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 2;

		ImageInfo items[count] = {
			to_image_info(file_type, 48, 192, "Punk_idle", 818733, 855),
			to_image_info(file_type, 48, 288, "Punk_run", 819588, 1742),
		};

		enum class Items : u32
		{
			Punk_idle,
			Punk_run,
		};


		static constexpr ImageInfo color_table = to_image_info(table_type, 256, 1, "table", 821330, 134);

		constexpr Spriteset_Punk(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);
		}


		ColorTableImage read_table(Buffer8 const& buffer) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, color_table);
		}


		TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(TableFilterImage::type == file_type);
			return read_table_filter(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_Table1)
	class Tileset_ex_zone
	{
	public:
		static constexpr u32 offset = 821464;
		static constexpr u32 size = 848;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_TableFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 2;

		ImageInfo items[count] = {
			to_image_info(file_type, 32, 32, "floor_02", 821464, 377),
			to_image_info(file_type, 32, 32, "floor_03", 821841, 369),
		};

		enum class Items : u32
		{
			floor_02,
			floor_03,
		};


		static constexpr ImageInfo color_table = to_image_info(table_type, 256, 1, "table", 822210, 102);

		constexpr Tileset_ex_zone(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);
		}


		ColorTableImage read_table(Buffer8 const& buffer) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, color_table);
		}


		TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(TableFilterImage::type == file_type);
			return read_table_filter(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_Table1)
	class UIset_Font
	{
	public:
		static constexpr u32 offset = 822312;
		static constexpr u32 size = 3011;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_TableFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 1;

		ImageInfo items[count] = {
			to_image_info(file_type, 16, 806, "font", 822312, 2783),
		};

		enum class Items : u32
		{
			font,
		};


		static constexpr ImageInfo color_table = to_image_info(table_type, 256, 1, "table", 825095, 228);

		constexpr UIset_Font(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);
		}


		ColorTableImage read_table(Buffer8 const& buffer) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, color_table);
		}


		TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(TableFilterImage::type == file_type);
			return read_table_filter(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_Table1)
	class UIset_Title
	{
	public:
		static constexpr u32 offset = 825323;
		static constexpr u32 size = 1413;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_TableFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 1;

		ImageInfo items[count] = {
			to_image_info(file_type, 74, 96, "title_main", 825323, 1321),
		};

		enum class Items : u32
		{
			title_main,
		};


		static constexpr ImageInfo color_table = to_image_info(table_type, 256, 1, "table", 826644, 92);

		constexpr UIset_Title(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);
		}


		ColorTableImage read_table(Buffer8 const& buffer) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, color_table);
		}


		TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(TableFilterImage::type == file_type);
			return read_table_filter(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	// define_image_set(Info_ImageX_Table1)
	class UIset_Icons
	{
	public:
		static constexpr u32 offset = 826736;
		static constexpr u32 size = 4959;

		using ImageInfo = AssetInfo_Image;

		static constexpr FileType file_type = FileType::Image1C_TableFilter;
		static constexpr FileType table_type = FileType::Image4C_Table;

		static constexpr u32 count = 1;

		ImageInfo items[count] = {
			to_image_info(file_type, 32, 1312, "icons", 826736, 4731),
		};

		enum class Items : u32
		{
			icons,
		};


		static constexpr ImageInfo color_table = to_image_info(table_type, 256, 1, "table", 831467, 228);

		constexpr UIset_Icons(){}


		bool test(Buffer8 const& buffer) const
		{
			return test_items(buffer, (ImageInfo*)items, count) && test_read(buffer, color_table);
		}


		ColorTableImage read_table(Buffer8 const& buffer) const
		{
			static_assert(ColorTableImage::type == table_type);
			return read_color_table(buffer, color_table);
		}


		TableFilterImage read_table_filter_item(Buffer8 const& buffer, Items key) const
		{
			static_assert(TableFilterImage::type == file_type);
			return read_table_filter(buffer, items[(u32)key]);
		}


	};

}


// auto-generated
namespace bin_table
{

	constexpr u32 CLASS_COUNT = 9;

}


