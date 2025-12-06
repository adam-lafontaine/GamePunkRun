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