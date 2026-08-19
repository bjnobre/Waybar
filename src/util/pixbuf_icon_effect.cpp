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
  const double mask_red = std::clamp(color.get_red(), 0.0, 1.0);
  const double mask_green = std::clamp(color.get_green(), 0.0, 1.0);
  const double mask_blue = std::clamp(color.get_blue(), 0.0, 1.0);
  const double mask_alpha = std::clamp(color.get_alpha(), 0.0, 1.0);

  const int channels = result->get_n_channels();
  const bool has_alpha = result->get_has_alpha();
  guchar* pixels = result->get_pixels();

  // Some tray icons store their shape only in alpha and leave RGB at zero.
  // Keep those icons visible instead of turning them into black pixels.
  bool has_visible_rgb = false;
  for (int y = 0; y < result->get_height() && !has_visible_rgb; ++y) {
    const guchar* row = pixels + y * result->get_rowstride();
    for (int x = 0; x < result->get_width(); ++x) {
      const guchar* pixel = row + x * channels;
      if ((!has_alpha || pixel[3] != 0) && (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)) {
        has_visible_rgb = true;
        break;
      }
    }
  }

  for (int y = 0; y < result->get_height(); ++y) {
    guchar* row = pixels + y * result->get_rowstride();
    for (int x = 0; x < result->get_width(); ++x) {
      guchar* pixel = row + x * channels;
      // Preserve the source icon's perceived intensity while applying the mask color.
      const double intensity =
          has_visible_rgb ? std::clamp(0.2126 * pixel[0] / 255.0 + 0.7152 * pixel[1] / 255.0 +
                                           0.0722 * pixel[2] / 255.0,
                                       0.0, 1.0)
                          : 1.0;
      pixel[0] = to_byte(mask_red * intensity);
      pixel[1] = to_byte(mask_green * intensity);
      pixel[2] = to_byte(mask_blue * intensity);
      if (has_alpha) {
        pixel[3] = static_cast<guchar>(std::lround(pixel[3] * mask_alpha));
      }
    }
  }

  return result;
}

}  // namespace waybar::util
