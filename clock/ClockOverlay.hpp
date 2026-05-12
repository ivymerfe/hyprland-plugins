#pragma once

#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/Texture.hpp>

class CClockOverlay {
  struct SRenderCache {
    SP<CTexture> texture;
    float scale = 1.0;
    int hour;
    int minute;
    CBox bbox;
    bool valid;
  };
  int m_hour;
  int m_minute;
  std::unordered_map<PHLMONITOR, SRenderCache> m_cache;

 public:
  CClockOverlay();

  ~CClockOverlay();

  void tick();

  void render(const PHLMONITOR& pMonitor);

  SRenderCache& ensureCache(const PHLMONITOR& pMonitor);

 private:
  SP<CTexture> renderText(const std::string& text, CHyprColor col,
                                       int pt, int weight,
                                       CHyprColor outlineCol,
                                       double outlineWidth);
};
