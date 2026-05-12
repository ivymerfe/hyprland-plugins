#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/managers/eventLoop/EventLoopManager.hpp>
#include <hyprland/src/managers/eventLoop/EventLoopTimer.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <string>

#include "ClockOverlay.hpp"

inline HANDLE PHANDLE = nullptr;

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

inline CFunctionHook* g_pRenderWorkspaceHook = nullptr;

typedef void (*origRenderWorkspace)(void*, PHLMONITOR, PHLWORKSPACE,
                                    const Time::steady_tp&, const CBox&);

static UP<CClockOverlay> g_clockOverlay;
static SP<CEventLoopTimer> g_clockTimer;

static void hkRenderWorkspace(void* thisptr, PHLMONITOR pMonitor,
                              PHLWORKSPACE pWorkspace,
                              const Time::steady_tp& now,
                              const CBox& geometry) {
  ((origRenderWorkspace)(g_pRenderWorkspaceHook->m_original))(
      thisptr, pMonitor, pWorkspace, now, geometry);
  g_clockOverlay->render(pMonitor);
}

void notify(const std::string& text) {
  HyprlandAPI::addNotificationV2(PHANDLE,
                                 {{"text", text},
                                  {"time", uint64_t(3000)},
                                  {"color", CHyprColor{0.8, 0.5, 0., 1.0}}});
}

static void onTimer(SP<CEventLoopTimer>, void*) {
  g_clockOverlay->tick();
  g_clockTimer->updateTimeout(std::chrono::milliseconds(1000));
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  g_clockOverlay = makeUnique<CClockOverlay>();
  g_clockOverlay->tick();

  auto FNS = HyprlandAPI::findFunctionsByName(PHANDLE, "renderWorkspace");
  if (FNS.empty()) {
    notify("[clock] no fns for hook renderWorkspace");
    throw std::runtime_error("[clock] No fns for hook renderWorkspace");
  }

  g_pRenderWorkspaceHook = HyprlandAPI::createFunctionHook(
      PHANDLE, FNS[0].address, (void*)hkRenderWorkspace);

  bool success = g_pRenderWorkspaceHook->hook();
  if (!success) {
    notify("[clock] Failed initializing hooks");
    throw std::runtime_error("[clock] Failed initializing hooks");
  }

  g_clockTimer = makeShared<CEventLoopTimer>(std::chrono::milliseconds(1000),
                                             onTimer, nullptr);
  g_pEventLoopManager->addTimer(g_clockTimer);

  return {"clock", "Clock overlay", "Ivy", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
  g_pEventLoopManager->removeTimer(g_clockTimer);
  g_pHyprRenderer->m_renderPass.removeAllOfType("CClockPassElement");
}
