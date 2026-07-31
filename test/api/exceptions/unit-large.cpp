#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "../base.h"

using Catch::Matchers::ContainsSubstring;

TEST_CASE("too large image", "[large]") {
    SECTION("input") {
        if (vips_type_find("VipsOperation", "svgload_source") == 0) {
            SUCCEED("no svg support, skipping test");
            return;
        }

        auto test_image = fixtures->input_svg_giant;
        Status status = process_file(test_image);

        CHECK(!status.ok());
        CHECK(status.code() == static_cast<int>(Status::Code::ImageTooLarge));
        CHECK(status.error_cause() == Status::ErrorCause::Application);
        CHECK_THAT(status.message(),
                   ContainsSubstring("Input image exceeds pixel limit"));
    }

    SECTION("output") {
        auto test_image = fixtures->input_jpg;
        auto params = "w=10000000&h=10000000&fit=fill";

        std::string out_buf;
        Status status = process_file(test_image, &out_buf, params);

        CHECK(!status.ok());
        CHECK(status.code() == static_cast<int>(Status::Code::ImageTooLarge));
        CHECK(status.error_cause() == Status::ErrorCause::Application);
        CHECK_THAT(status.message(),
                   ContainsSubstring("Output image exceeds pixel limit"));
        CHECK(out_buf.empty());
    }
}

TEST_CASE("too many channels", "[large]") {
    SECTION("input") {
        if (vips_type_find("VipsOperation", "tiffload_source") == 0) {
            SUCCEED("no tiff support, skipping test");
            return;
        }

        auto test_image = fixtures->input_tiff_5_channel;
        auto config = Config();
        config.limit_input_channels = 4;

        std::string out_buf;
        Status status = process_file(test_image, &out_buf, "", config);

        CHECK(!status.ok());
        CHECK(status.code() == static_cast<int>(Status::Code::ImageTooLarge));
        CHECK(status.error_cause() == Status::ErrorCause::Application);
        CHECK_THAT(status.message(),
                   ContainsSubstring("Input image exceeds 4-channel limit"));
        CHECK(out_buf.empty());
    }
}

TEST_CASE("too many pages", "[large]") {
    SECTION("input") {
        if (vips_type_find("VipsOperation", "gifload_source") == 0) {
            SUCCEED("no gif support, skipping test");
            return;
        }

        auto test_image = fixtures->input_gif_1024_pages;
        auto params = "n=-1";

        std::string out_buf;
        Status status = process_file(test_image, &out_buf, params);

        CHECK(!status.ok());
        CHECK(status.code() == static_cast<int>(Status::Code::ImageTooLarge));
        CHECK(status.error_cause() == Status::ErrorCause::Application);
        CHECK_THAT(status.message(),
                   ContainsSubstring("Input image exceeds 256-page limit"));
        CHECK(out_buf.empty());
    }
}
