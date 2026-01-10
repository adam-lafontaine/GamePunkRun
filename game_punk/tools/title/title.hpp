#pragma once

#include "../utils/tools_include.hpp"


namespace title
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


    static ImageResult get_title_image(sfs::path const& dir)
    {
        ImageResult result;

        auto path = dir / "title_game.png"; // Magic!

        result.name = path.filename();        
        result.n_expected = 1;
        result.n_read = 0;

        result.n_read += util::count_read_image_rotate_90(path, result.image);

        assert(result.n_read == result.n_expected);

        return result;
    }


    static u32 write_title_res_cpp(ImageResult& res, sfs::path const& out)
    {
        auto out_file_path = out / "title_game.cpp"; // Magic!

        std::ofstream out_file(out_file_path, std::ios::trunc);

        out_file << util::image_cpp_string(res.image, "title_game");

        out_file.close();

        return 1;
    }


    static void print_result(auto const& result, u32 n_written)
    {
        printf("%s: %u/%u/%u\n", result.name.c_str(), result.n_expected, result.n_read, n_written);
    }
}


namespace title
{
    void generate_title()
    {
        auto out_dir = sfs::path(OUT_DIR);
        auto in_dir = sfs::path(SRC_DIR);

        printf("\n--- title ---\n");

        auto res = get_title_image(in_dir);

        auto count = write_title_res_cpp(res, out_dir);

        print_result(res, count);
    }
}