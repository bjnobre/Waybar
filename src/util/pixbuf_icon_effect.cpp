#include "util/pixbuf_icon_effect.hpp"

#include <algorithm>
#include <cmath>

namespace waybar::util {

Glib::RefPtr<Gdk::Pixbuf> apply_icon_mask(const Glib::RefPtr<Gdk::Pixbuf>& source,
                                          const Gdk::RGBA& color) {
  if (!source) {
    return {};
  }

  auto result = source->copy();
  if (!result) {
    return source;
  }

  const auto to_byte = [](double channel) {
    return static_cast<guchar>(std::lround(std::clamp(channel, 0.0, 1.0) * 255.0));
  };
  const guchar red = to_byte(color.get_red());
  const guchar green = to_byte(color.get_green());
  const guchar blue = to_byte(color.get_blue());
  const double mask_alpha = std::clamp(color.get_alpha(), 0.0, 1.0);

  const int channels = result->get_n_channels();
  const bool has_alpha = result->get_has_alpha();
  guchar* pixels = result->get_pixels();

  for (int y = 0; y < result->get_height(); ++y) {
    guchar* row = pixels + y * result->get_rowstride();
    for (int x = 0; x < result->get_width(); ++x) {
      guchar* pixel = row + x * channels;
      pixel[0] = red;
      pixel[1] = green;
      pixel[2] = blue;
      if (has_alpha) {
        pixel[3] = static_cast<guchar>(std::lround(pixel[3] * mask_alpha));
      }
    }
  }

  return result;
}

}  // namespace waybar::util
