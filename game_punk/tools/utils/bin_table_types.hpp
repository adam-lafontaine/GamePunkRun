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