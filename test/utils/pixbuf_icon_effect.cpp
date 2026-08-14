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

TEST_CASE("Icon mask replaces RGB and preserves alpha", "[pixbuf-icon-effect]") {
  initialize_gdkmm();
  auto source = Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, true, 8, 2, 1);
  auto* pixels = source->get_pixels();
  pixels[0] = 1;
  pixels[1] = 2;
  pixels[2] = 3;
  pixels[3] = 255;
  pixels[4] = 250;
  pixels[5] = 240;
  pixels[6] = 230;
  pixels[7] = 128;

  Gdk::RGBA color;
  REQUIRE(color.set("rgba(17, 34, 51, 0.5)"));
  auto masked = waybar::util::apply_icon_mask(source, color);
  const auto* result = masked->get_pixels();

  REQUIRE(result[0] == 17);
  REQUIRE(result[1] == 34);
  REQUIRE(result[2] == 51);
  REQUIRE(result[3] == 128);
  REQUIRE(result[4] == 17);
  REQUIRE(result[5] == 34);
  REQUIRE(result[6] == 51);
  REQUIRE(result[7] == 64);

  REQUIRE(pixels[0] == 1);
  REQUIRE(pixels[3] == 255);
}

TEST_CASE("Icon mask supports RGB pixbufs", "[pixbuf-icon-effect]") {
  initialize_gdkmm();
  auto source = Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB, false, 8, 1, 1);
  auto* pixels = source->get_pixels();
  pixels[0] = 1;
  pixels[1] = 2;
  pixels[2] = 3;

  Gdk::RGBA color;
  REQUIRE(color.set("#abcdef"));
  auto masked = waybar::util::apply_icon_mask(source, color);
  const auto* result = masked->get_pixels();

  REQUIRE(result[0] == 0xab);
  REQUIRE(result[1] == 0xcd);
  REQUIRE(result[2] == 0xef);
}
