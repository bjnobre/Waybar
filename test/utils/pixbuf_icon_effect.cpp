#include "util/pixbuf_icon_effect.hpp"

#include <gdkmm/wrap_init.h>

#if __has_include(<catch2/catch_test_macros.hpp>)
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

namespace {
void initialize_gdkmm() {
  static const bool initialized = [] {
    Gdk::wrap_init();
    return true;
  }();
  (void)initialized;
}
}  // namespace

TEST_CASE("Icon mask preserves intensity and alpha", "[pixbuf-icon-effect]") {
  initialize_gdkmm();
  auto source = Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, true, 8, 4, 1);
  auto* pixels = source->get_pixels();
  pixels[0] = 255;
  pixels[1] = 0;
  pixels[2] = 0;
  pixels[3] = 255;
  pixels[4] = 0;
  pixels[5] = 255;
  pixels[6] = 0;
  pixels[7] = 128;
  pixels[8] = 0;
  pixels[9] = 0;
  pixels[10] = 255;
  pixels[11] = 64;
  pixels[12] = 255;
  pixels[13] = 255;
  pixels[14] = 255;
  pixels[15] = 255;

  Gdk::RGBA color;
  REQUIRE(color.set("rgba(17, 34, 51, 1)"));
  auto masked = waybar::util::apply_icon_mask(source, color);
  const auto* result = masked->get_pixels();

  REQUIRE(result[0] == 4);
  REQUIRE(result[1] == 7);
  REQUIRE(result[2] == 11);
  REQUIRE(result[3] == 128);
  REQUIRE(result[4] == 12);
  REQUIRE(result[5] == 24);
  REQUIRE(result[6] == 36);
  REQUIRE(result[7] == 128);
  REQUIRE(result[8] == 1);
  REQUIRE(result[9] == 2);
  REQUIRE(result[10] == 4);
  REQUIRE(result[11] == 64);
  REQUIRE(result[12] == 17);
  REQUIRE(result[13] == 34);
  REQUIRE(result[14] == 51);
  REQUIRE(result[15] == 255);

  REQUIRE(pixels[0] == 255);
  REQUIRE(pixels[3] == 255);
}

TEST_CASE("Icon mask supports RGB pixbufs", "[pixbuf-icon-effect]") {
  initialize_gdkmm();
  auto source = Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, false, 8, 1, 1);
  auto* pixels = source->get_pixels();
  pixels[0] = 255;
  pixels[1] = 255;
  pixels[2] = 255;

  Gdk::RGBA color;
  REQUIRE(color.set("#abcdef"));
  auto masked = waybar::util::apply_icon_mask(source, color);
  const auto* result = masked->get_pixels();

  REQUIRE(result[0] == 0xab);
  REQUIRE(result[1] == 0xcd);
  REQUIRE(result[2] == 0xef);
}

TEST_CASE("Icon mask keeps alpha-only pixbufs visible", "[pixbuf-icon-effect]") {
  initialize_gdkmm();
  auto source = Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, true, 8, 1, 1);
  auto* pixels = source->get_pixels();
  pixels[0] = 0;
  pixels[1] = 0;
  pixels[2] = 0;
  pixels[3] = 128;

  Gdk::RGBA color;
  REQUIRE(color.set("#abcdef"));
  auto masked = waybar::util::apply_icon_mask(source, color);
  const auto* result = masked->get_pixels();

  REQUIRE(result[0] == 0xab);
  REQUIRE(result[1] == 0xcd);
  REQUIRE(result[2] == 0xef);
  REQUIRE(result[3] == 128);
}
