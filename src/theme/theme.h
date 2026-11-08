#ifndef SALVIA_THEME_H
#define SALVIA_THEME_H

#include <string>
#include <vector>

/* Data-driven frontend theme.
 *
 * The parser is intentionally free of SDL, Windows and the Xbox SDK so it can
 * be unit-tested on a host that cannot build Salvia.exe or a .xex. Numeric
 * background ids match ANIM_BACKGROUNDS in const/menuconst.h:
 *   0 tiles, 1 image, 2 plasma, 3 purple, 4 wormhole, 5 none.
 * TH_BG_KEEP (-1) leaves salvia.cfg animBG alone.
 *
 * Color slot order is the order theme files use. themeruntime.cpp maps each
 * slot onto enumColors; the two lists must stay aligned.
 */

enum {
	TH_BG_KEEP = -1
};

enum ThemeColorId {
	TH_COLOR_BACKGROUND = 0,
	TH_COLOR_BLACK,
	TH_COLOR_WHITE,
	TH_COLOR_YELLOW,
	TH_COLOR_RED,
	TH_COLOR_BLUE,
	TH_COLOR_MENU,
	TH_COLOR_MENU_LIGHTER,
	TH_COLOR_PANEL,
	TH_COLOR_BORDER,
	TH_COLOR_BARS,
	TH_COLOR_NAV,
	TH_COLOR_TEXT_DIM,
	TH_COLOR_SWITCH_ON,
	TH_COLOR_SWITCH_OFF,
	TH_COLOR_PALE_BLUE,
	TH_COLOR_DIALOG_TITLE,
	TH_COLOR_DIALOG_BG,
	TH_COLOR_DIALOG_LINE,
	TH_COLOR_DIALOG_TEXT,
	TH_COLOR_HIGHLIGHT,
	TH_COLOR_LIST_TEXT,
	TH_COLOR_LIST_TEXT_SELECTED,
	TH_COLOR_HEADER,
	TH_COLOR_FOOTER,
	TH_COLOR_SELECTION,
	TH_COLOR_COUNT
};

enum ThemeView {
	TH_VIEW_BOXES = 0,
	TH_VIEW_LIST,
	TH_VIEW_GRID,
	TH_VIEW_CAROUSEL
};

enum ThemeAlign {
	TH_ALIGN_LEFT = 0,
	TH_ALIGN_CENTER,
	TH_ALIGN_RIGHT
};

enum ThemeSelection {
	TH_SEL_BAR = 0,
	TH_SEL_OUTLINE,
	TH_SEL_UNDERLINE,
	TH_SEL_NONE
};

enum ThemeSystemTitle {
	TH_TITLE_BOTH = 0,
	TH_TITLE_TEXT,
	TH_TITLE_IMAGE
};

struct ThemeColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	int present; /* 1 when the theme file set this slot */
};

struct SalviaTheme {
	std::string id;
	std::string name;
	std::string author;
	std::string directory;

	int view;
	int align;
	int selection;
	int selection_alpha;       /* -1 keeps the historical per-widget alpha */
	int system_title;
	int background;            /* TH_BG_KEEP or an ANIM_BACKGROUNDS value */
	int background_dim;        /* -1 keeps the historical veil (150); 0..255 otherwise */
	int background_per_system; /* 1: assets/<system>/background.jpg wins over the theme image */
	int margin_x;              /* -1 = screen width / 100 */
	int margin_y;              /* -1 = derived from the font and header flag */
	int item_spacing;
	int item_padding;
	int grid_columns;
	int carousel_visible;
	int font_size;             /* 0 = scale the built-in 24px-at-720p size */
	int font_small_delta;
	int show_header;
	int show_footer;
	int show_clock;
	int show_nav_bar;
	int show_scrollbar;
	int show_icons;
	int show_separator;
	int show_artwork;
	int loaded_from_file;

	std::string font_file;
	std::string background_image;
	std::string header_text;
	std::string footer_text;

	ThemeColor colors[TH_COLOR_COUNT];
	std::vector<std::string> warnings;
};

/* Fills *theme with the historical Salvia look. No color slot is marked present,
 * so applying the result does not overwrite Constant::colors. */
void theme_set_builtin(SalviaTheme* theme);

/* Parses theme.cfg text over a fresh builtin theme. directory is stored as-is
 * (the folder that contained the file). Returns 1 on success, 0 if theme is NULL.
 * Unknown keys and bad values are recorded in warnings and skipped. */
int theme_parse(SalviaTheme* theme, const std::string& text, const std::string& directory);

/* Reads path. On failure *theme is left unchanged and 0 is returned. */
int theme_load_file(SalviaTheme* theme, const std::string& path);

/* id empty or "default" still looks for assets/themes/default/theme.cfg.
 * A missing file keeps the builtin look and returns 0. An invalid id (anything
 * other than letters, digits, '_' and '-') is rejected the same way.
 * sep is the path separator; pass "/" or "\\". */
int theme_resolve(SalviaTheme* theme, const std::string& appDir, const std::string& id, const std::string& sep);

int theme_id_valid(const std::string& id);
int theme_color_from_hex(const std::string& text, unsigned char* r, unsigned char* g, unsigned char* b);
const char* theme_color_key(int id);

#endif
