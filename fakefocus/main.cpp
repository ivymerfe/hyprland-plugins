#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#define private public
#include <hyprland/src/protocols/core/Compositor.hpp>
#include <hyprland/src/protocols/core/Seat.hpp>
#undef private

inline HANDLE PHANDLE = nullptr;

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

typedef void (*tKeyboardLeave)(CWLKeyboardResource*);

inline CFunctionHook* g_pKeyboardLeaveHook = nullptr;

uint32_t g_fakeFocusEnabledIdx;

PHLWINDOW getWindowFromSurface(SP<CWLSurfaceResource> pSurface) {
  if (!pSurface || !pSurface->m_hlSurface) return nullptr;

  auto view = pSurface->m_hlSurface->view();
  return Desktop::View::CWindow::fromView(view);
}

void hkKeyboardLeave(CWLKeyboardResource* thisptr) {
  if (thisptr->m_currentSurface.valid()) {
    const auto pWindow = getWindowFromSurface(thisptr->m_currentSurface.lock());
    if (pWindow &&
        pWindow->m_ruleApplicator->m_tagKeeper.isTagged("fakefocus")) {
      return;
    }
  }
  ((tKeyboardLeave)g_pKeyboardLeaveHook->m_original)(thisptr);
}

void notify(const std::string& text) {
  HyprlandAPI::addNotificationV2(PHANDLE,
                                 {{"text", text},
                                  {"time", uint64_t(3000)},
                                  {"color", CHyprColor{0.8, 0.5, 0., 1.0}}});
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  const std::string COMPOSITOR_HASH = __hyprland_api_get_hash();
  const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

  if (COMPOSITOR_HASH != CLIENT_HASH) {
    notify("[fakefocus] Version mismatch, plugin not loaded");
    throw std::runtime_error("[fakefocus] Version mismatch");
  }

  auto FNS = HyprlandAPI::findFunctionsByName(PHANDLE, "sendLeave");
  for (auto& fn : FNS) {
    if (!fn.demangled.contains("CWLKeyboardResource")) continue;

    g_pKeyboardLeaveHook = HyprlandAPI::createFunctionHook(
        PHANDLE, fn.address, (void*)&hkKeyboardLeave);
    if (!g_pKeyboardLeaveHook->hook()) {
      notify("[fakefocus] Failed initializing hooks");
      throw std::runtime_error("[fakefocus] Failed initializing hooks");
    }
    break;
  }
  return {"fakefocus", "Suppress keyboard leave events", "Ivy", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
  // ...
}
