/* Host test for the theme parser. No SDL, Windows headers, or Xbox SDK. */
#include "theme/theme.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

static int g_failed = 0;

static void expect_true(int cond, const char* msg) {
	if (!cond) {
		std::fprintf(stderr, "FAIL: %s\n", msg);
		++g_failed;
	}
}

static void expect_int(int got, int want, const char* msg) {
	if (got != want) {
		std::fprintf(stderr, "FAIL: %s (got %d want %d)\n", msg, got, want);
		++g_failed;
	}
}

static void expect_str(const std::string& got, const char* want, const char* msg) {
	if (got != want) {
		std::fprintf(stderr, "FAIL: %s (got '%s' want '%s')\n", msg, got.c_str(), want);
		++g_failed;
	}
}

static int color_is(const SalviaTheme& theme, int id, int r, int g, int b) {
	if (id < 0 || id >= TH_COLOR_COUNT) {
		return 0;
	}
	if (!theme.colors[id].present) {
		return 0;
	}
	return theme.colors[id].r == (unsigned char)r
		&& theme.colors[id].g == (unsigned char)g
		&& theme.colors[id].b == (unsigned char)b;
}

static void test_builtin() {
	SalviaTheme theme;
	theme_set_builtin(&theme);
	expect_str(theme.id, "default", "builtin id");
	expect_int(theme.view, TH_VIEW_BOXES, "builtin view");
	expect_int(theme.align, TH_ALIGN_LEFT, "builtin align");
	expect_int(theme.selection, TH_SEL_BAR, "builtin selection");
	expect_int(theme.selection_alpha, -1, "builtin alpha");
	expect_int(theme.background, TH_BG_KEEP, "builtin background");
	expect_int(theme.background_per_system, 1, "builtin per-system");
	expect_int(theme.show_artwork, 1, "builtin artwork");
	expect_int(theme.show_icons, 1, "builtin icons");
	expect_int(theme.font_size, 0, "builtin font size");
	expect_int(theme.loaded_from_file, 0, "builtin not from file");
	expect_int(theme.colors[TH_COLOR_SELECTION].present, 0, "builtin does not override colors");
}

static void test_hex() {
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	expect_true(theme_color_from_hex("#F7DD72", &r, &g, &b), "hex 6");
	expect_int(r, 247, "hex r");
	expect_int(g, 221, "hex g");
	expect_int(b, 114, "hex b");
	expect_true(theme_color_from_hex("010101", &r, &g, &b), "hex no hash");
	expect_int(r, 1, "near black r");
	expect_int(g, 1, "near black g");
	expect_int(b, 1, "near black b");
	expect_true(theme_color_from_hex("#abc", &r, &g, &b), "hex 3");
	expect_int(r, 0xAA, "short r");
	expect_int(g, 0xBB, "short g");
	expect_int(b, 0xCC, "short b");
	expect_true(theme_color_from_hex("#1C6DD0FF", &r, &g, &b), "hex 8 ignores alpha");
	expect_int(r, 0x1C, "hex8 r");
	expect_int(g, 0x6D, "hex8 g");
	expect_int(b, 0xD0, "hex8 b");
	expect_true(!theme_color_from_hex("zz", &r, &g, &b), "bad hex");
	expect_true(!theme_color_from_hex("", &r, &g, &b), "empty hex");
}

static void test_parse_sample() {
	const char* text =
		"# comment\r\n"
		"\n"
		"name = \"Clean list\"\n"
		"view = carousel\n"
		"align = centre\n"
		"selection = underline\n"
		"selection_alpha = 400\n"
		"system_title = image\n"
		"background = wormholes\n"
		"background_dim = 12\n"
		"background_per_system = no\n"
		"margin_x = 40\n"
		"margin_y = -5\n"
		"item_spacing = 9\n"
		"grid_columns = 99\n"
		"carousel_visible = 4\n"
		"font = faces/ui.ttf\n"
		"font_size = 30\n"
		"show_icons = off\n"
		"show_artwork = false\n"
		"show_header = 1\n"
		"header_text = HELLO\n"
		"footer_text = 'A Start'\n"
		"color.text = #112233\n"
		"color.selector = #ABCDEF\n"
		"not_a_key = 1\n"
		"color.nope = #ffffff\n"
		"view = not-a-view\n";
	SalviaTheme theme;
	expect_int(theme_parse(&theme, text, "themes/sample"), 1, "parse returns 1");
	expect_str(theme.name, "Clean list", "quoted name");
	expect_str(theme.directory, "themes/sample", "directory stored");
	expect_int(theme.view, TH_VIEW_CAROUSEL, "view carousel");
	expect_int(theme.align, TH_ALIGN_CENTER, "centre alias");
	expect_int(theme.selection, TH_SEL_UNDERLINE, "underline");
	expect_int(theme.selection_alpha, 255, "alpha clamped");
	expect_int(theme.system_title, TH_TITLE_IMAGE, "title image");
	expect_int(theme.background, 4, "wormhole id");
	expect_int(theme.background_dim, 12, "dim");
	expect_int(theme.background_per_system, 0, "per system off");
	expect_int(theme.margin_x, 40, "margin x");
	expect_int(theme.margin_y, -1, "negative margin means default");
	expect_int(theme.item_spacing, 9, "spacing");
	expect_int(theme.grid_columns, 12, "columns clamped");
	expect_int(theme.carousel_visible, 5, "even carousel becomes next odd");
	expect_str(theme.font_file, "faces/ui.ttf", "font");
	expect_int(theme.font_size, 30, "font size");
	expect_int(theme.show_icons, 0, "icons off");
	expect_int(theme.show_artwork, 0, "artwork off");
	expect_int(theme.show_header, 1, "header on");
	expect_str(theme.header_text, "HELLO", "header text");
	expect_str(theme.footer_text, "A Start", "footer quotes");
	expect_true(color_is(theme, TH_COLOR_LIST_TEXT, 0x11, 0x22, 0x33), "text alias");
	expect_true(color_is(theme, TH_COLOR_SELECTION, 0xAB, 0xCD, 0xEF), "selector alias");
	expect_int(theme.colors[TH_COLOR_WHITE].present, 0, "unset color stays absent");
	expect_true(theme.warnings.size() >= 3, "warnings recorded");
	expect_str(theme_color_key(TH_COLOR_SELECTION), "selection", "color key name");
	expect_int(theme_parse(NULL, text, ""), 0, "null theme");
}

static void test_shipped(const char* root) {
	SalviaTheme theme;
	int loaded = theme_resolve(&theme, root, "default", "/");
	expect_int(loaded, 1, "default theme file loads");
	expect_int(theme.view, TH_VIEW_BOXES, "default file keeps boxes");
	expect_int(theme.align, TH_ALIGN_LEFT, "default file keeps left");
	expect_int(theme.background, TH_BG_KEEP, "default file keeps animBG");
	expect_int(theme.show_artwork, 1, "default file keeps artwork");
	expect_int(theme.show_footer, 1, "default file keeps footer");
	expect_true(color_is(theme, TH_COLOR_BLACK, 1, 1, 1), "default black is 010101");
	expect_true(color_is(theme, TH_COLOR_SELECTION, 247, 221, 114), "default selection is the gold bar");
	expect_true(color_is(theme, TH_COLOR_LIST_TEXT, 255, 255, 255), "default list text");
	expect_true(color_is(theme, TH_COLOR_LIST_TEXT_SELECTED, 1, 1, 1), "default selected text");
	expect_true(color_is(theme, TH_COLOR_MENU, 247, 221, 114), "default menu gold");

	loaded = theme_resolve(&theme, root, "emulationstation", "/");
	expect_int(loaded, 1, "example theme loads");
	expect_str(theme.name, "EmulationStation", "example name");
	expect_int(theme.view, TH_VIEW_LIST, "example is a list");
	expect_int(theme.align, TH_ALIGN_CENTER, "example is centered");
	expect_int(theme.selection, TH_SEL_BAR, "example bar");
	expect_int(theme.background, 1, "example static image");
	expect_int(theme.background_per_system, 0, "example ignores per-system art");
	expect_str(theme.background_image, "background.png", "example image name");
	expect_int(theme.show_icons, 0, "example hides icons");
	expect_int(theme.show_artwork, 0, "example hides the art column");
	expect_int(theme.show_separator, 0, "example hides rules");
	expect_int(theme.font_size, 32, "example font size");
	expect_true(color_is(theme, TH_COLOR_SELECTION, 0x1C, 0x6D, 0xD0), "example blue bar");
	expect_true(color_is(theme, TH_COLOR_LIST_TEXT_SELECTED, 255, 255, 255), "example selected text stays light");
	expect_true(!theme.footer_text.empty(), "example help line");

	loaded = theme_resolve(&theme, root, "../escape", "/");
	expect_int(loaded, 0, "path id rejected");
	expect_str(theme.id, "default", "rejected id falls back");
	expect_int(theme.loaded_from_file, 0, "rejected id is builtin");

	loaded = theme_resolve(&theme, root, "missing-theme", "/");
	expect_int(loaded, 0, "missing theme");
	expect_int(theme.view, TH_VIEW_BOXES, "missing theme keeps boxes");
	expect_true(!theme.warnings.empty(), "missing theme warns");

	loaded = theme_resolve(&theme, root, "", "/");
	expect_int(loaded, 1, "empty id loads default file");
	expect_str(theme.id, "default", "empty id name");
}

int main(int argc, char** argv) {
	const char* root = ".";
	if (argc > 1) {
		root = argv[1];
	}
	test_builtin();
	test_hex();
	test_parse_sample();
	test_shipped(root);
	if (g_failed != 0) {
		std::fprintf(stderr, "%d theme test(s) failed\n", g_failed);
		return 1;
	}
	std::printf("theme tests passed\n");
	return 0;
}
