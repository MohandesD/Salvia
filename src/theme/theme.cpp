#include "theme.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace {

struct ColorName {
	const char* name;
	int id;
};

/* Names accepted after the "color." prefix. Aliases share an id. */
const ColorName kColorNames[] = {
	{"background", TH_COLOR_BACKGROUND},
	{"black", TH_COLOR_BLACK},
	{"white", TH_COLOR_WHITE},
	{"yellow", TH_COLOR_YELLOW},
	{"red", TH_COLOR_RED},
	{"blue", TH_COLOR_BLUE},
	{"menu", TH_COLOR_MENU},
	{"menu_bkg", TH_COLOR_MENU},
	{"bkg", TH_COLOR_MENU},
	{"menu_lighter", TH_COLOR_MENU_LIGHTER},
	{"panel", TH_COLOR_PANEL},
	{"border", TH_COLOR_BORDER},
	{"bars", TH_COLOR_BARS},
	{"nav", TH_COLOR_NAV},
	{"text_dim", TH_COLOR_TEXT_DIM},
	{"switch_on", TH_COLOR_SWITCH_ON},
	{"switch_off", TH_COLOR_SWITCH_OFF},
	{"pale_blue", TH_COLOR_PALE_BLUE},
	{"dialog_title", TH_COLOR_DIALOG_TITLE},
	{"dialog_bg", TH_COLOR_DIALOG_BG},
	{"dialog_line", TH_COLOR_DIALOG_LINE},
	{"dialog_text", TH_COLOR_DIALOG_TEXT},
	{"highlight", TH_COLOR_HIGHLIGHT},
	{"list_text", TH_COLOR_LIST_TEXT},
	{"text", TH_COLOR_LIST_TEXT},
	{"list_text_selected", TH_COLOR_LIST_TEXT_SELECTED},
	{"text_selected", TH_COLOR_LIST_TEXT_SELECTED},
	{"header", TH_COLOR_HEADER},
	{"footer", TH_COLOR_FOOTER},
	{"selection", TH_COLOR_SELECTION},
	{"selector", TH_COLOR_SELECTION}
};

const char* kColorKeys[TH_COLOR_COUNT] = {
	"background",
	"black",
	"white",
	"yellow",
	"red",
	"blue",
	"menu",
	"menu_lighter",
	"panel",
	"border",
	"bars",
	"nav",
	"text_dim",
	"switch_on",
	"switch_off",
	"pale_blue",
	"dialog_title",
	"dialog_bg",
	"dialog_line",
	"dialog_text",
	"highlight",
	"list_text",
	"list_text_selected",
	"header",
	"footer",
	"selection"
};

std::string theme_lower(const std::string& in) {
	std::string out;
	out.reserve(in.size());
	for (std::size_t i = 0; i < in.size(); ++i) {
		unsigned char c = (unsigned char)in[i];
		out.push_back((char)std::tolower(c));
	}
	return out;
}

void theme_trim(std::string* s) {
	if (s == NULL) {
		return;
	}
	std::size_t begin = 0;
	while (begin < s->size()) {
		unsigned char c = (unsigned char)(*s)[begin];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
			break;
		}
		++begin;
	}
	std::size_t end = s->size();
	while (end > begin) {
		unsigned char c = (unsigned char)(*s)[end - 1];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
			break;
		}
		--end;
	}
	*s = s->substr(begin, end - begin);
}

void theme_unquote(std::string* s) {
	if (s == NULL || s->size() < 2) {
		return;
	}
	char a = (*s)[0];
	char b = (*s)[s->size() - 1];
	if ((a == '"' && b == '"') || (a == '\'' && b == '\'')) {
		*s = s->substr(1, s->size() - 2);
		theme_trim(s);
	}
}

void theme_warn(SalviaTheme* theme, const std::string& msg) {
	if (theme != NULL) {
		theme->warnings.push_back(msg);
	}
}

int theme_clamp(int v, int lo, int hi) {
	if (v < lo) {
		return lo;
	}
	if (v > hi) {
		return hi;
	}
	return v;
}

int theme_parse_bool(const std::string& v, int* out) {
	std::string s = theme_lower(v);
	if (s == "yes" || s == "true" || s == "on" || s == "1") {
		*out = 1;
		return 1;
	}
	if (s == "no" || s == "false" || s == "off" || s == "0") {
		*out = 0;
		return 1;
	}
	return 0;
}

int theme_parse_int(const std::string& v, int* out) {
	if (v.empty()) {
		return 0;
	}
	char* end = NULL;
	long n = std::strtol(v.c_str(), &end, 10);
	if (end == v.c_str() || (end != NULL && *end != '\0')) {
		return 0;
	}
	*out = (int)n;
	return 1;
}

int theme_parse_view(const std::string& v, int* out) {
	std::string s = theme_lower(v);
	if (s == "boxes" || s == "split" || s == "salvia") {
		*out = TH_VIEW_BOXES;
		return 1;
	}
	if (s == "list" || s == "simple") {
		*out = TH_VIEW_LIST;
		return 1;
	}
	if (s == "grid") {
		*out = TH_VIEW_GRID;
		return 1;
	}
	if (s == "carousel" || s == "wheel") {
		*out = TH_VIEW_CAROUSEL;
		return 1;
	}
	return 0;
}

int theme_parse_align(const std::string& v, int* out) {
	std::string s = theme_lower(v);
	if (s == "left") {
		*out = TH_ALIGN_LEFT;
		return 1;
	}
	if (s == "center" || s == "centre" || s == "middle") {
		*out = TH_ALIGN_CENTER;
		return 1;
	}
	if (s == "right") {
		*out = TH_ALIGN_RIGHT;
		return 1;
	}
	return 0;
}

int theme_parse_selection(const std::string& v, int* out) {
	std::string s = theme_lower(v);
	if (s == "bar" || s == "fill") {
		*out = TH_SEL_BAR;
		return 1;
	}
	if (s == "outline" || s == "box") {
		*out = TH_SEL_OUTLINE;
		return 1;
	}
	if (s == "underline" || s == "line") {
		*out = TH_SEL_UNDERLINE;
		return 1;
	}
	if (s == "none" || s == "off") {
		*out = TH_SEL_NONE;
		return 1;
	}
	return 0;
}

int theme_parse_title(const std::string& v, int* out) {
	std::string s = theme_lower(v);
	if (s == "both") {
		*out = TH_TITLE_BOTH;
		return 1;
	}
	if (s == "text" || s == "name") {
		*out = TH_TITLE_TEXT;
		return 1;
	}
	if (s == "image" || s == "logo") {
		*out = TH_TITLE_IMAGE;
		return 1;
	}
	return 0;
}

/* Values match ANIM_BACKGROUNDS. Keep them in sync with menuconst.h. */
int theme_parse_background(const std::string& v, int* out) {
	std::string s = theme_lower(v);
	if (s == "keep") {
		*out = TH_BG_KEEP;
		return 1;
	}
	if (s == "tiles" || s == "animated" || s == "bricks") {
		*out = 0;
		return 1;
	}
	if (s == "image" || s == "static") {
		*out = 1;
		return 1;
	}
	if (s == "plasma" || s == "hlsl") {
		*out = 2;
		return 1;
	}
	if (s == "purple" || s == "drops") {
		*out = 3;
		return 1;
	}
	if (s == "wormhole" || s == "wormholes") {
		*out = 4;
		return 1;
	}
	if (s == "none" || s == "solid") {
		*out = 5;
		return 1;
	}
	return 0;
}

int theme_color_id(const std::string& name) {
	std::string s = theme_lower(name);
	const int count = (int)(sizeof(kColorNames) / sizeof(kColorNames[0]));
	for (int i = 0; i < count; ++i) {
		if (s == kColorNames[i].name) {
			return kColorNames[i].id;
		}
	}
	return -1;
}

std::string theme_parent_dir(const std::string& path) {
	std::size_t slash = path.find_last_of("/\\");
	if (slash == std::string::npos) {
		return std::string();
	}
	return path.substr(0, slash);
}

void theme_apply_key(SalviaTheme* theme, const std::string& keyRaw, const std::string& valueRaw) {
	std::string key = theme_lower(keyRaw);
	std::string value = valueRaw;
	theme_unquote(&value);
	int n = 0;
	int ok = 0;

	if (key == "name") {
		theme->name = value;
		return;
	}
	if (key == "author") {
		theme->author = value;
		return;
	}
	if (key == "view" || key == "layout") {
		ok = theme_parse_view(value, &n);
		if (ok) {
			theme->view = n;
		} else {
			theme_warn(theme, "bad view: " + value);
		}
		return;
	}
	if (key == "align" || key == "alignment") {
		ok = theme_parse_align(value, &n);
		if (ok) {
			theme->align = n;
		} else {
			theme_warn(theme, "bad align: " + value);
		}
		return;
	}
	if (key == "selection" || key == "highlight_style") {
		ok = theme_parse_selection(value, &n);
		if (ok) {
			theme->selection = n;
		} else {
			theme_warn(theme, "bad selection: " + value);
		}
		return;
	}
	if (key == "system_title" || key == "title") {
		ok = theme_parse_title(value, &n);
		if (ok) {
			theme->system_title = n;
		} else {
			theme_warn(theme, "bad system_title: " + value);
		}
		return;
	}
	if (key == "background" || key == "background_mode") {
		ok = theme_parse_background(value, &n);
		if (ok) {
			theme->background = n;
		} else {
			theme_warn(theme, "bad background: " + value);
		}
		return;
	}
	if (key == "font" || key == "font_file") {
		theme->font_file = value;
		return;
	}
	if (key == "background_image") {
		theme->background_image = value;
		return;
	}
	if (key == "header_text") {
		theme->header_text = value;
		return;
	}
	if (key == "footer_text") {
		theme->footer_text = value;
		return;
	}
	if (key == "selection_alpha") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->selection_alpha = (n < 0) ? -1 : theme_clamp(n, 0, 255);
		} else {
			theme_warn(theme, "bad selection_alpha: " + value);
		}
		return;
	}
	if (key == "background_dim") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->background_dim = (n < 0) ? -1 : theme_clamp(n, 0, 255);
		} else {
			theme_warn(theme, "bad background_dim: " + value);
		}
		return;
	}
	if (key == "background_per_system") {
		ok = theme_parse_bool(value, &n);
		if (ok) {
			theme->background_per_system = n;
		} else {
			theme_warn(theme, "bad background_per_system: " + value);
		}
		return;
	}
	if (key == "margin_x") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->margin_x = (n < 0) ? -1 : theme_clamp(n, 0, 2000);
		} else {
			theme_warn(theme, "bad margin_x: " + value);
		}
		return;
	}
	if (key == "margin_y") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->margin_y = (n < 0) ? -1 : theme_clamp(n, 0, 2000);
		} else {
			theme_warn(theme, "bad margin_y: " + value);
		}
		return;
	}
	if (key == "item_spacing") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->item_spacing = theme_clamp(n, 0, 400);
		} else {
			theme_warn(theme, "bad item_spacing: " + value);
		}
		return;
	}
	if (key == "item_padding") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->item_padding = theme_clamp(n, 0, 200);
		} else {
			theme_warn(theme, "bad item_padding: " + value);
		}
		return;
	}
	if (key == "grid_columns") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->grid_columns = theme_clamp(n, 1, 12);
		} else {
			theme_warn(theme, "bad grid_columns: " + value);
		}
		return;
	}
	if (key == "carousel_visible") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			n = theme_clamp(n, 3, 15);
			if ((n % 2) == 0) {
				++n;
			}
			theme->carousel_visible = n;
		} else {
			theme_warn(theme, "bad carousel_visible: " + value);
		}
		return;
	}
	if (key == "font_size") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->font_size = theme_clamp(n, 0, 128);
		} else {
			theme_warn(theme, "bad font_size: " + value);
		}
		return;
	}
	if (key == "font_small_delta") {
		ok = theme_parse_int(value, &n);
		if (ok) {
			theme->font_small_delta = theme_clamp(n, 1, 64);
		} else {
			theme_warn(theme, "bad font_small_delta: " + value);
		}
		return;
	}

	const char* flags[] = {
		"show_header", "show_footer", "show_clock", "show_nav_bar",
		"show_scrollbar", "show_icons", "show_separator", "show_artwork"
	};
	int* slots[] = {
		&theme->show_header, &theme->show_footer, &theme->show_clock, &theme->show_nav_bar,
		&theme->show_scrollbar, &theme->show_icons, &theme->show_separator, &theme->show_artwork
	};
	const int flagCount = (int)(sizeof(flags) / sizeof(flags[0]));
	for (int i = 0; i < flagCount; ++i) {
		if (key == flags[i]) {
			ok = theme_parse_bool(value, &n);
			if (ok) {
				*slots[i] = n;
			} else {
				theme_warn(theme, std::string("bad ") + flags[i] + ": " + value);
			}
			return;
		}
	}

	const std::string colorPrefix = "color.";
	if (key.size() > colorPrefix.size() && key.compare(0, colorPrefix.size(), colorPrefix) == 0) {
		std::string name = key.substr(colorPrefix.size());
		int id = theme_color_id(name);
		unsigned char r = 0;
		unsigned char g = 0;
		unsigned char b = 0;
		if (id < 0) {
			theme_warn(theme, "unknown color: " + name);
			return;
		}
		if (!theme_color_from_hex(value, &r, &g, &b)) {
			theme_warn(theme, "bad color " + name + ": " + value);
			return;
		}
		theme->colors[id].r = r;
		theme->colors[id].g = g;
		theme->colors[id].b = b;
		theme->colors[id].present = 1;
		return;
	}

	theme_warn(theme, "unknown key: " + keyRaw);
}

} /* namespace */

const char* theme_color_key(int id) {
	if (id < 0 || id >= TH_COLOR_COUNT) {
		return "";
	}
	return kColorKeys[id];
}

int theme_id_valid(const std::string& id) {
	if (id.empty() || id.size() > 64) {
		return 0;
	}
	for (std::size_t i = 0; i < id.size(); ++i) {
		unsigned char c = (unsigned char)id[i];
		if (!std::isalnum(c) && c != '_' && c != '-') {
			return 0;
		}
	}
	return 1;
}

int theme_color_from_hex(const std::string& text, unsigned char* r, unsigned char* g, unsigned char* b) {
	if (r == NULL || g == NULL || b == NULL) {
		return 0;
	}
	std::string s = text;
	theme_trim(&s);
	if (!s.empty() && s[0] == '#') {
		s = s.substr(1);
	}
	if (s.size() == 8) {
		/* RRGGBBAA: the alpha byte is ignored. Selection opacity has its own key. */
		s = s.substr(0, 6);
	}
	if (s.size() != 3 && s.size() != 6) {
		return 0;
	}
	for (std::size_t i = 0; i < s.size(); ++i) {
		unsigned char c = (unsigned char)s[i];
		if (!std::isxdigit(c)) {
			return 0;
		}
	}
	unsigned int rv = 0;
	unsigned int gv = 0;
	unsigned int bv = 0;
	if (s.size() == 3) {
		rv = (unsigned int)std::strtoul(s.substr(0, 1).c_str(), NULL, 16);
		gv = (unsigned int)std::strtoul(s.substr(1, 1).c_str(), NULL, 16);
		bv = (unsigned int)std::strtoul(s.substr(2, 1).c_str(), NULL, 16);
		rv = rv * 17;
		gv = gv * 17;
		bv = bv * 17;
	} else {
		rv = (unsigned int)std::strtoul(s.substr(0, 2).c_str(), NULL, 16);
		gv = (unsigned int)std::strtoul(s.substr(2, 2).c_str(), NULL, 16);
		bv = (unsigned int)std::strtoul(s.substr(4, 2).c_str(), NULL, 16);
	}
	*r = (unsigned char)rv;
	*g = (unsigned char)gv;
	*b = (unsigned char)bv;
	return 1;
}

void theme_set_builtin(SalviaTheme* theme) {
	if (theme == NULL) {
		return;
	}
	theme->id = "default";
	theme->name = "Salvia";
	theme->author.clear();
	theme->directory.clear();
	theme->view = TH_VIEW_BOXES;
	theme->align = TH_ALIGN_LEFT;
	theme->selection = TH_SEL_BAR;
	theme->selection_alpha = -1;
	theme->system_title = TH_TITLE_BOTH;
	theme->background = TH_BG_KEEP;
	theme->background_dim = -1;
	theme->background_per_system = 1;
	theme->margin_x = -1;
	theme->margin_y = -1;
	theme->item_spacing = 0;
	theme->item_padding = 8;
	theme->grid_columns = 4;
	theme->carousel_visible = 5;
	theme->font_size = 0;
	theme->font_small_delta = 9;
	theme->show_header = 1;
	theme->show_footer = 1;
	theme->show_clock = 1;
	theme->show_nav_bar = 1;
	theme->show_scrollbar = 1;
	theme->show_icons = 1;
	theme->show_separator = 1;
	theme->show_artwork = 1;
	theme->loaded_from_file = 0;
	theme->font_file.clear();
	theme->background_image.clear();
	theme->header_text.clear();
	theme->footer_text.clear();
	theme->warnings.clear();
	for (int i = 0; i < TH_COLOR_COUNT; ++i) {
		theme->colors[i].r = 0;
		theme->colors[i].g = 0;
		theme->colors[i].b = 0;
		theme->colors[i].present = 0;
	}
}

int theme_parse(SalviaTheme* theme, const std::string& text, const std::string& directory) {
	if (theme == NULL) {
		return 0;
	}
	theme_set_builtin(theme);
	theme->directory = directory;

	std::istringstream in(text);
	std::string line;
	int lineNo = 0;
	while (std::getline(in, line)) {
		++lineNo;
		theme_trim(&line);
		if (line.empty()) {
			continue;
		}
		if (line[0] == '#' || line[0] == ';') {
			continue;
		}
		std::size_t eq = line.find('=');
		if (eq == std::string::npos) {
			std::ostringstream msg;
			msg << "line " << lineNo << ": missing '='";
			theme_warn(theme, msg.str());
			continue;
		}
		std::string key = line.substr(0, eq);
		std::string value = line.substr(eq + 1);
		theme_trim(&key);
		theme_trim(&value);
		if (key.empty()) {
			std::ostringstream msg;
			msg << "line " << lineNo << ": empty key";
			theme_warn(theme, msg.str());
			continue;
		}
		theme_apply_key(theme, key, value);
	}
	return 1;
}

int theme_load_file(SalviaTheme* theme, const std::string& path) {
	if (theme == NULL) {
		return 0;
	}
	std::ifstream in(path.c_str());
	if (!in) {
		return 0;
	}
	std::stringstream buffer;
	buffer << in.rdbuf();
	in.close();
	return theme_parse(theme, buffer.str(), theme_parent_dir(path));
}

int theme_resolve(SalviaTheme* theme, const std::string& appDir, const std::string& id, const std::string& sep) {
	if (theme == NULL) {
		return 0;
	}
	theme_set_builtin(theme);
	std::string use = id;
	theme_trim(&use);
	if (use.empty()) {
		use = "default";
	}
	if (!theme_id_valid(use)) {
		theme_warn(theme, "invalid theme id: " + use);
		theme->id = "default";
		return 0;
	}
	std::string slash = sep.empty() ? std::string("/") : sep;
	std::string path = appDir + slash + "assets" + slash + "themes" + slash + use + slash + "theme.cfg";
	if (!theme_load_file(theme, path)) {
		theme_set_builtin(theme);
		theme->id = use;
		if (use != "default") {
			theme_warn(theme, "theme file not found: " + path);
		}
		return 0;
	}
	theme->id = use;
	theme->loaded_from_file = 1;
	return 1;
}
