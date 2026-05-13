#pragma once

#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/Texture.hpp>

class CClockOverlay {
  struct SRenderCache {
    SP<CTexture> texture;
    float fontSize;
    int hour;
    int minute;
  };

  int m_hour = 0;
  int m_minute = 0;
  bool m_shown = true;
  std::unordered_map<PHLMONITOR, SRenderCache> m_cache;

public:
  CClockOverlay();

  ~CClockOverlay();

  bool updateTime();

  void addDamage();

  void toggle();

  void tick();

  void render(const PHLMONITOR &pMonitor);

  static CBox getBBox(const PHLMONITOR &pMonitor, const SP<CTexture>& texture);

  SP<CTexture> getTexture(const PHLMONITOR &pMonitor);

private:
  static SP<CTexture> renderText(const std::string &text, CHyprColor col,
                                 int pt, int weight, CHyprColor outlineCol,
                                 double outlineWidth);
};
