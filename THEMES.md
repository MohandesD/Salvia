# Frontend themes

Salvia's menus, colors, and list layout can be restyled from files. No rebuild is required. The built-in look stays in place until a theme is selected.

```
assets/themes/<id>/theme.cfg
assets/themes/<id>/          optional images and a .ttf, referenced by the cfg
```

`<id>` is a folder name made of letters, digits, `_` and `-`. Two themes ship with Salvia:

| id | What it looks like |
| --- | --- |
| `default` | The historical Salvia frontend: split list and artwork, gold selection bar, moving-tile background left to `salvia.cfg`. |
| `emulationstation` | A clean system list: dark field, centered names, blue selection bar, no artwork column. Inspired by EmulationStation's system list. It does not reuse EmulationStation's fonts, logos, or artwork. |

## Select a theme

In `salvia.cfg`, next to the other frontend keys:

```
theme=emulationstation
```

Or, while Salvia is running: **Options > Video > Theme**. The list is every folder under `assets/themes` that contains `theme.cfg`. Colors, layout, spacing, and the background mode apply immediately. A font file is loaded at the same time; if it cannot be opened, Salvia keeps the embedded Arimo face.

`theme=default`, or deleting the key, loads `assets/themes/default/theme.cfg`. If that file is missing too, the same look is built into the program.

The choice is saved with the rest of `salvia.cfg` when options are saved. `background` does not rewrite `animBG` on disk. A value other than `keep` is drawn instead of `animBG` for that session and again on the next launch, for as long as that theme stays selected. Changing **Options > Video > Background** wins until the next launch.

## Author a theme

Copy `assets/themes/default` to `assets/themes/my-look` and edit `theme.cfg`. Lines starting with `#` or `;` are comments. Keys are `name = value`. Colors are `#RRGGBB` or `#RGB`.

### Layout

| Key | Values | Default |
| --- | --- | --- |
| `view` | `boxes`, `list`, `grid`, `carousel` | `boxes` |
| `align` | `left`, `center`, `right` | `left` |
| `margin_x`, `margin_y` | pixels, or `-1` for the built-in margin | `-1` |
| `item_spacing` | extra pixels between rows | `0` |
| `item_padding` | padding inside a row, grid cell, or carousel slot | `8` |
| `grid_columns` | 1..12, used when `view = grid` | `4` |
| `carousel_visible` | 3..15, forced to an odd count so one item sits in the center | `5` |
| `show_header` | `yes` / `no` | `yes` |
| `show_footer` | draw the button-glyph bar | `yes` |
| `show_clock` | clock (and the mouse icon on Xbox) in the header | `yes` |
| `show_nav_bar` | path line while browsing a folder or zip | `yes` |
| `show_scrollbar` | `yes` / `no` | `yes` |
| `show_icons` | cartridge and file icons beside names | `yes` |
| `show_separator` | header and column rules | `yes` |
| `show_artwork` | right-hand snap / box / synopsis column (`boxes` only) | `yes` |
| `system_title` | `both`, `text`, `image` | `both` |
| `header_text` | optional line at the left of the header | empty |
| `footer_text` | optional help line in the footer | empty |

`boxes` is the current Salvia browser: names on the left, scraped artwork on the right. `list` is a full-width text list (the example theme). `grid` is a cell grid; up and down move a row, left and right move one cell. `carousel` puts the selected name in the center and its neighbors on either side; left and right both move one entry.

`system_title = image` uses `title.png` from the system's asset folder when that file exists, and falls back to the system name otherwise. `both` is the historical behavior (image plus name). Per-system background music is unchanged: each core's `music_file` still overrides the general `musicFile` in `salvia.cfg`.

### Selection

| Key | Values | Default |
| --- | --- | --- |
| `selection` | `bar`, `outline`, `underline`, `none` | `bar` |
| `selection_alpha` | 0..255, or `-1` to keep the historical opacity | `-1` |

`color.selection` is the highlight. `color.list_text` and `color.list_text_selected` are the row colors. The options menu uses the same three, so a theme restyles both the game list and the settings list.

On Xbox the list bar is drawn opaque. The console already skips per-pixel alpha on that surface for speed; the color still changes.

### Backgrounds

| Key | Values | Default |
| --- | --- | --- |
| `background` | `keep`, `tiles`, `image`, `plasma`, `purple`, `wormhole`, `none` | `keep` |
| `background_image` | png or jpg, relative to the theme folder (or an absolute path) | empty |
| `background_dim` | 0..255 veil over the image, or `-1` for the historical dim of 150. `0` leaves the image alone | `-1` |
| `background_per_system` | `yes` / `no` | `yes` |

`keep` does not touch `animBG` in `salvia.cfg` (moving tiles unless you changed it). The other names are the same modes as **Options > Video > Background**:

| Theme value | Existing `animBG` |
| --- | --- |
| `tiles` | moving bricks |
| `image` | static picture |
| `plasma` | plasma shader |
| `purple` | purple-drop shader |
| `wormhole` | wormhole shader |
| `none` | flat `color.background` |

When the mode is `image` and `background_per_system` is `yes`, `assets/<system>/background.jpg` wins over the theme image. That is the per-system background Salvia already supported. Set `background_per_system` to `no` when the theme picture should stay up on every platform. If the theme image is missing, Salvia still falls back to the per-system jpg, then to a flat color.

### Fonts

| Key | Meaning |
| --- | --- |
| `font` | `.ttf` path, relative to the theme folder, then to the Salvia folder. Empty uses the embedded Arimo face. |
| `font_size` | pixel size at 720p. `0` keeps 24, scaled with the screen height. |
| `font_small_delta` | how much smaller the secondary face is. Historical value is 9. |

Any face SDL_ttf can open works. Salvia does not ship a second font.

### Colors

`color.<name> = #RRGGBB`

| Name | What it paints |
| --- | --- |
| `background` | flat backdrop when no image or shader is active |
| `list_text`, `list_text_selected`, `selection` | game list and options list |
| `header`, `footer` | title, clock, help line, footer glyphs |
| `menu`, `menu_lighter` | older menu chrome that still reads these slots |
| `panel` | nav-path backing |
| `bars` | separator lines |
| `nav` | nav accent |
| `text_dim` | dimmed rows when the list does not have focus |
| `border` | generic borders |
| `white`, `black`, `red`, `yellow`, `blue` | status and dialog accents. Leave `black` at `#010101`. Pure `#000000` is what the alpha-fixup shader treats as an empty pixel. |
| `switch_on`, `switch_off` | boolean switches |
| `dialog_title`, `dialog_bg`, `dialog_line`, `dialog_text` | confirm dialogs |
| `highlight` | read-only option values |
| `pale_blue` | secondary panels |

Unknown keys are skipped and reported in `salvia.log`. A bad color does not change that slot.

## What a theme does not change

These stay as they are in code or in `salvia.cfg`:

- The game picture, shaders, integer scale, and aspect ratio (`SHADERS.md`).
- Button mapping, hotkeys, and the on-screen keyboard geometry.
- Achievement toasts, the lightgun crosshair, and the FPS counter.
- How ROMs are launched. There is no return-to-another-frontend handoff.
- Icon artwork under `assets/xmb`. `show_icons = no` hides them; it does not load a replacement set.
- Per-core `music_file` and the general `musicFile` track. Themes do not carry audio.

The options menu's row geometry still follows the font metrics more than `margin_x` / `view`. `view` applies to the game and system browser.
