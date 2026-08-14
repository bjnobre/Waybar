#pragma once

#include <gdkmm/pixbuf.h>
#include <gdkmm/rgba.h>
#include <glibmm/refptr.h>

namespace waybar::util {

Glib::RefPtr<Gdk::Pixbuf> apply_icon_mask(const Glib::RefPtr<Gdk::Pixbuf>& source,
                                          const Gdk::RGBA& color);

}  // namespace waybar::util
