#include "themeruntime.h"

#include <const/constant.h>
#include <const/menuconst.h>
#include <font/fonts.h>
#include <io/cfgloader.h>
#include <io/dirutil.h>
#include <menus/gamemenu.h>
#include <uiobjects/listmenu.h>
#include <utils/logger.h>

namespace {

SalviaTheme g_theme;
int g_ready = 0;
GameMenu* g_menu = NULL;
ListMenu* g_list = NULL;
std::string g_appliedId;
int g_userBg = 0;

/* Must stay aligned with enumColors. */
const int kColorSlot[TH_COLOR_COUNT] = {
	clBackground,
	clBlack,
	clWhite,
	clYellow,
	clRed,
	clBlue,
	clBkgMenu,
	clBkgMenuLighter,
	clBG,
	clBorder,
	clMenuBars,
	clTxtNavBar,
	clDarkGray,
	clSwitchEnabled,
	clSwitchDisabled,
	clPaleBlue,
	clAskTitle,
	clAskBg,
	clAskLine,
	clAskText,
	clHighligtOption,
	clListText,
	clListTextSelected,
	clHeader,
	clFooter,
	clSelection
};

void theme_ensure() {
	if (!g_ready) {
		theme_set_builtin(&g_theme);
		g_ready = 1;
	}
}

std::string theme_join(const std::string& dir, const std::string& file) {
	if (file.empty()) {
		return std::string();
	}
	if (file.size() > 1 && (file[1] == ':' || file[0] == '\\' || file[0] == '/')) {
		return file;
	}
	if (dir.empty()) {
		return file;
	}
	char last = dir[dir.size() - 1];
	if (last == '\\' || last == '/') {
		return dir + file;
	}
	return dir + Constant::getFileSep() + file;
}

} /* namespace */

const SalviaTheme& Theme_current() {
	theme_ensure();
	return g_theme;
}

void Theme_boot(const std::string& appDir, const std::string& themeId) {
	std::string sep = Constant::getFileSep();
	int loaded = theme_resolve(&g_theme, appDir, themeId, sep);
	g_ready = 1;
	g_userBg = 0;
	g_appliedId = g_theme.id;
	if (!loaded && !themeId.empty() && themeId != "default") {
		LOG_ERROR("Theme '%s' was not loaded; using the built-in Salvia look\n", themeId.c_str());
	} else if (loaded) {
		LOG_DEBUG("Theme '%s' loaded from %s\n", g_theme.id.c_str(), g_theme.directory.c_str());
	}
	for (std::size_t i = 0; i < g_theme.warnings.size(); ++i) {
		LOG_ERROR("Theme: %s\n", g_theme.warnings[i].c_str());
	}
}

void Theme_applyColors() {
	theme_ensure();
	for (int i = 0; i < TH_COLOR_COUNT; ++i) {
		if (!g_theme.colors[i].present) {
			continue;
		}
		int slot = kColorSlot[i];
		if (slot < 0 || slot >= clTotalColors) {
			continue;
		}
		Constant::colors[slot].sdlColor.r = g_theme.colors[i].r;
		Constant::colors[slot].sdlColor.g = g_theme.colors[i].g;
		Constant::colors[slot].sdlColor.b = g_theme.colors[i].b;
	}
}

void Theme_applyBackground(CfgLoader* cfg) {
	(void)cfg;
	theme_ensure();
	g_userBg = 0;
}

int Theme_effectiveBackground(int configured) {
	theme_ensure();
	if (g_userBg) {
		return configured;
	}
	if (g_theme.background < 0 || g_theme.background >= BG_MAX) {
		return configured;
	}
	return g_theme.background;
}

void Theme_noteUserBackground() {
	g_userBg = 1;
}

void Theme_refreshBackground() {
	if (g_menu == NULL) {
		return;
	}
	g_menu->loadBgImage();
	g_menu->applyMenuBackground();
}

void Theme_bind(GameMenu* menu, ListMenu* list) {
	g_menu = menu;
	g_list = list;
}

int Theme_toLayout(const SalviaTheme& theme) {
	if (theme.view == TH_VIEW_LIST) {
		return LAYTEXT;
	}
	if (theme.view == TH_VIEW_GRID) {
		return LAYGRID;
	}
	if (theme.view == TH_VIEW_CAROUSEL) {
		return LAYCAROUSEL;
	}
	return LAYBOXES;
}

int Theme_fontSizeForHeight(int videoHeight) {
	theme_ensure();
	int base = g_theme.font_size;
	if (base <= 0) {
		base = 24; /* BASE_FONT_HEIGHT in engine.h, 24px at 720p */
	}
	if (videoHeight <= 0) {
		videoHeight = 720;
	}
	int size = (int)((float)base * (float)videoHeight / 720.0f);
	if (size < 8) {
		size = 8;
	}
	if (size > 128) {
		size = 128;
	}
	return size;
}

int Theme_fontSmallDelta() {
	theme_ensure();
	if (g_theme.font_small_delta < 1) {
		return 9;
	}
	return g_theme.font_small_delta;
}

std::string Theme_resolvedFontPath() {
	theme_ensure();
	if (g_theme.font_file.empty()) {
		return std::string();
	}
	std::string direct = g_theme.font_file;
	if (dirutil::fileExists(direct.c_str())) {
		return direct;
	}
	std::string inTheme = theme_join(g_theme.directory, g_theme.font_file);
	if (!inTheme.empty() && dirutil::fileExists(inTheme.c_str())) {
		return inTheme;
	}
	std::string inApp = theme_join(Constant::getAppDir(), g_theme.font_file);
	if (!inApp.empty() && dirutil::fileExists(inApp.c_str())) {
		return inApp;
	}
	LOG_ERROR("Theme font not found: %s\n", g_theme.font_file.c_str());
	return std::string();
}

int Theme_selectionAlpha(int fallback) {
	theme_ensure();
	if (g_theme.selection_alpha < 0) {
		return fallback;
	}
	if (g_theme.selection_alpha > 255) {
		return 255;
	}
	return g_theme.selection_alpha;
}

void Theme_applyLive(const std::string& themeId) {
	std::string previousFont = Theme_resolvedFontPath();
	int previousSize = 0;
	int previousDelta = Theme_fontSmallDelta();
	if (g_menu != NULL && g_menu->overlay != NULL) {
		previousSize = Theme_fontSizeForHeight(g_menu->overlay->h);
	}

	Theme_boot(Constant::getAppDir(), themeId);
	Theme_applyColors();
	if (g_menu != NULL) {
		Theme_applyBackground(g_menu->getCfgLoader());
		if (g_menu->overlay != NULL) {
			g_menu->initColors(g_menu->overlay);
		}
		g_menu->loadBgImage();
		g_menu->applyMenuBackground();
		if (g_menu->joystick != NULL) {
			g_menu->joystick->infoButtonsDirty = true;
		}
	}
	if (g_list != NULL && g_menu != NULL && g_menu->overlay != NULL) {
		g_list->invalidateThemeSurfaces();
		int h = g_menu->overlay->h;
		int w = g_menu->overlay->w;
		std::string fontPath = Theme_resolvedFontPath();
		int fontSize = Theme_fontSizeForHeight(h);
		int delta = Theme_fontSmallDelta();
		if (fontPath != previousFont || fontSize != previousSize || delta != previousDelta) {
			Fonts::initFonts(fontSize, fontPath, delta);
			g_list->syncFontMetrics();
			g_menu->syncFontMetrics();
		}
		g_list->setLayout(Theme_toLayout(Theme_current()), w, h);
	}
	(void)g_appliedId;
}
