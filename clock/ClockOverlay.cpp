#include "ClockOverlay.hpp"

#include <cairo/cairo.h>
#include <pango/pangocairo.h>

#include <src/Compositor.hpp>
#include <src/render/OpenGL.hpp>
#include <src/render/Renderer.hpp>
#include <utility>

#include "ClockPassElement.hpp"

CClockOverlay::CClockOverlay() = default;

CClockOverlay::~CClockOverlay() = default;

bool CClockOverlay::updateTime() {
  const std::time_t t = std::time(nullptr);
  const std::tm *tm = std::localtime(&t);
  if (m_hour != tm->tm_hour || m_minute != tm->tm_min) {
    m_hour = tm->tm_hour;
    m_minute = tm->tm_min;
    return true;
  }
  return false;
}

void CClockOverlay::addDamage() {
  for (const auto &monitor : g_pCompositor->m_monitors) {
    const auto &cache = m_cache[monitor];
    if (cache.texture) {
      monitor->addDamage(getBBox(monitor, cache.texture));
    }
  }
}

void CClockOverlay::toggle() {
  m_shown = !m_shown;
  if (m_shown) {
    updateTime();
    for (const auto &monitor : g_pCompositor->m_monitors) {
      g_pHyprRenderer->damageMonitor(monitor);
    }
  } else {
    addDamage();
  }
}

void CClockOverlay::tick() {
  if (!m_shown) {
    return;
  }
  if (updateTime()) {
    addDamage();
  }
}

void CClockOverlay::render(const PHLMONITOR &pMonitor) {
  if (!m_shown) {
    return;
  }
  const auto texture = getTexture(pMonitor);
  CClockPassElement::SRenderData textData;
  textData.texture = texture;
  textData.box = getBBox(pMonitor, texture);
  g_pHyprRenderer->m_renderPass.add(
      makeUnique<CClockPassElement>(std::move(textData)));
}

CBox CClockOverlay::getBBox(const PHLMONITOR &pMonitor,
                            const SP<CTexture> &texture) {
  const auto texSize = texture->m_size;
  const auto monSize = pMonitor->m_transformedSize;
  return {monSize.x - texSize.x, monSize.y - texSize.y, texSize.x, texSize.y};
}

SP<CTexture> CClockOverlay::getTexture(const PHLMONITOR &pMonitor) {
  auto &cache = m_cache[pMonitor];
  const float scale = pMonitor->m_scale;
  const float fontSize = 19.0f * scale;
  if (cache.fontSize != fontSize || cache.hour != m_hour ||
      cache.minute != m_minute) {
    cache.hour = m_hour;
    cache.minute = m_minute;
    const std::string text = (m_hour < 10 ? "0" : "") + std::to_string(m_hour) +
                             ":" + (m_minute < 10 ? "0" : "") +
                             std::to_string(m_minute);
    cache.texture = renderText(text, {1.0, 1.0, 1.0, 1.0}, fontSize, 400,
                               {0., 0., 0., 1.0}, 1.5 * scale);
    cache.fontSize = fontSize;
  }
  return cache.texture;
}

SP<CTexture> CClockOverlay::renderText(const std::string &text, CHyprColor col,
                                       int pt, int weight,
                                       CHyprColor outlineCol,
                                       double outlineWidth) {
  SP<CTexture> tex = makeShared<CTexture>();

  const auto FONTFAMILY = "monospace";
  const auto FONTSIZE = pt;
  const auto COLOR = col;

  auto setupLayout = [&](cairo_t *cr) -> PangoLayout * {
    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *fd = pango_font_description_new();
    pango_font_description_set_family_static(fd, FONTFAMILY);
    pango_font_description_set_absolute_size(fd, FONTSIZE * PANGO_SCALE);
    pango_font_description_set_style(fd, PANGO_STYLE_NORMAL);
    pango_font_description_set_weight(fd, sc<PangoWeight>(weight));
    pango_layout_set_font_description(layout, fd);
    pango_layout_set_text(layout, text.c_str(), -1);
    pango_font_description_free(fd);
    return layout;
  };

  auto CAIROSURFACE =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1920, 1080);
  auto CAIRO = cairo_create(CAIROSURFACE);
  auto layout = setupLayout(CAIRO);

  int textW = 0, textH = 0;
  pango_layout_get_size(layout, &textW, &textH);
  textW /= PANGO_SCALE;
  textH /= PANGO_SCALE;

  int padding = (int)std::ceil(outlineWidth);
  textW += padding * 2;
  textH += padding * 2;

  g_object_unref(layout);
  cairo_destroy(CAIRO);
  cairo_surface_destroy(CAIROSURFACE);

  CAIROSURFACE = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, textW, textH);
  CAIRO = cairo_create(CAIROSURFACE);
  layout = setupLayout(CAIRO);

  cairo_move_to(CAIRO, padding, padding);
  pango_cairo_layout_path(CAIRO, layout);

  if (outlineWidth > 0.0) {
    cairo_set_source_rgba(CAIRO, outlineCol.r, outlineCol.g, outlineCol.b,
                          outlineCol.a);
    cairo_set_line_width(CAIRO, outlineWidth * 2.0);
    cairo_set_line_join(CAIRO, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke_preserve(CAIRO);
  }

  cairo_set_source_rgba(CAIRO, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
  cairo_fill(CAIRO);

  g_object_unref(layout);
  cairo_surface_flush(CAIROSURFACE);

  tex->allocate();
  tex->m_size = {cairo_image_surface_get_width(CAIROSURFACE),
                 cairo_image_surface_get_height(CAIROSURFACE)};

  const auto DATA = cairo_image_surface_get_data(CAIROSURFACE);
  tex->bind();
  tex->setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  tex->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  tex->setTexParameter(GL_TEXTURE_SWIZZLE_R, GL_BLUE);
  tex->setTexParameter(GL_TEXTURE_SWIZZLE_B, GL_RED);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->m_size.x, tex->m_size.y, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, DATA);

  cairo_destroy(CAIRO);
  cairo_surface_destroy(CAIROSURFACE);

  return tex;
}
