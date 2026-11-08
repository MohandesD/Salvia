#ifndef SALVIA_THEME_RUNTIME_H
#define SALVIA_THEME_RUNTIME_H

#include <string>

#include "theme.h"

class CfgLoader;
class GameMenu;
class ListMenu;

/* Active theme. Before Theme_boot this is the builtin Salvia look. */
const SalviaTheme& Theme_current();

/* Load assets/themes/<id>/theme.cfg. Missing file keeps the builtin look. */
void Theme_boot(const std::string& appDir, const std::string& themeId);

/* Copy present color slots into Constant::colors. Does not map them to the
 * screen format; call Engine::initColors afterwards. */
void Theme_applyColors();

/* Theme background is drawn on top of animBG and is not written back to
 * salvia.cfg. A later call to Theme_noteUserBackground() lets the Video menu
 * win until the next theme load. */
void Theme_applyBackground(CfgLoader* cfg);
int Theme_effectiveBackground(int configured);
void Theme_noteUserBackground();
/* Reload the menu picture after the user changes Background. */
void Theme_refreshBackground();

/* Remember the live menu so a theme change from the options menu can restyle
 * without restarting. Fonts are reloaded when the size or file changed. */
void Theme_bind(GameMenu* menu, ListMenu* list);
void Theme_applyLive(const std::string& themeId);

/* enumLayout value for the active theme. */
int Theme_toLayout(const SalviaTheme& theme);

/* Pixel size at this screen height. font_size 0 uses the built-in 24px at 720p. */
int Theme_fontSizeForHeight(int videoHeight);
int Theme_fontSmallDelta();

/* Empty when the theme keeps the embedded Arimo face. */
std::string Theme_resolvedFontPath();

/* Historical alpha when selection_alpha is -1. */
int Theme_selectionAlpha(int fallback);

#endif
