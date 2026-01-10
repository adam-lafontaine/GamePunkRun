#pragma once

#include "../utils/tools_include.hpp"


namespace icon
{
    class ImageResult
    {
    public:
        std::string name;
        sfs::path file;
        img::Image image;

        u32 n_expected = 0;
        u32 n_read = 0;

        ~ImageResult() { img::destroy_image(image); }
    };


    static ImageResult get_icon_image(sfs::path const& dir)
    {
        ImageResult result;

        auto path = dir / "game_icon.png"; // Magic!

        result.name = path.filename();        
        result.n_expected = 1;
        result.n_read = 0;

        result.n_read += util::read_image(path, result.image);

        assert(result.n_read == result.n_expected);

        return result;
    }


    static u32 write_icon_res_cpp(ImageResult& res, sfs::path const& out)
    {
        auto out_file_path = out / "icon_64.cpp"; // Magic!

        auto icon = img::make_view(res.image);

        if (icon.width != 64 || icon.height != 64)
        {
            return 0;
        }

        auto span = img::to_span(res.image);

        std::ofstream out_file(out_file_path, std::ios::trunc);

        out_file << "static const struct {\n";
        out_file << "    unsigned int  width;\n";
        out_file << "    unsigned int  height;\n";
        out_file << "    unsigned int  bytes_per_pixel;\n";
        out_file << "    unsigned char pixel_data[64 * 64 * 4 + 1];\n";
        out_file << "} icon_64 = {\n";
        out_file << "    64, 64, 4,\n";

        out_file << "    {\n";

        for (u32 y = 0; y < icon.height; y++)
        {
            for (u32 x = 0; x < icon.width; x++)
            {
                auto p = img::pixel_at(icon, x, y);

                out_file << (int)p.red << ", " << (int)p.green << ", " << (int)p.blue << ", " << (int)p.alpha << ", ";
            }

            out_file << "\n";
        }


        out_file << "    0}\n";

        out_file << "};";


        out_file.close();

        return 1;
    }


    static void print_result(auto const& result, u32 n_written)
    {
        printf("%s: %u/%u/%u\n", result.name.c_str(), result.n_expected, result.n_read, n_written);
    }
}


namespace icon
{
    void generate_icon()
    {
        auto out_dir = sfs::path(OUT_DIR);
        auto in_dir = sfs::path(SRC_DIR);

        printf("\n--- icon ---\n");

        auto res = get_icon_image(in_dir);

        auto count = write_icon_res_cpp(res, out_dir);

        print_result(res, count);
    }
}