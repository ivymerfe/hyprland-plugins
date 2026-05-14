#pragma once

#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/Texture.hpp>

class CClockOverlay {
  struct SRenderCache {
    SP<Render::ITexture> texture;
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

private:
  static CBox getBBox(const PHLMONITOR &pMonitor,
                      const SP<Render::ITexture> &texture);

  std::string getTimeString(int hour, int minute);

  SP<Render::ITexture> getTexture(const PHLMONITOR &pMonitor);

  static SP<Render::ITexture> renderText(const std::string &text,
                                         CHyprColor col, int pt, int weight,
                                         CHyprColor outlineCol,
                                         double outlineWidth);
};
