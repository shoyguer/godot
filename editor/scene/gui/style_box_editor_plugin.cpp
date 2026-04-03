/**************************************************************************/
/*  style_box_editor_plugin.cpp                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "style_box_editor_plugin.h"

#include "core/object/callable_mp.h"
#include "editor/editor_string_names.h"
#include "editor/gui/editor_spin_slider.h"
#include "editor/settings/editor_settings.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/texture_button.h"
#include "scene/gui/texture_rect.h"
#include "scene/resources/style_box_flat.h"
#include "scene/resources/style_box_texture.h"

// EditorPropertySideGroup

void EditorPropertySideGroup::_set_read_only(bool p_read_only) {
	for (EditorSpinSlider *spin : spin_sliders) {
		spin->set_read_only(p_read_only);
	}
}

void EditorPropertySideGroup::_value_changed(double p_val, int p_idx) {
	if (linked->is_pressed()) {
		for (int i = 0; i < spin_sliders.size(); i++) {
			if (i != p_idx) {
				spin_sliders[i]->set_value_no_signal(p_val);
			}
		}
	}

	Vector<String> changed;
	Array values;
	for (int i = 0; i < spin_sliders.size(); i++) {
		changed.push_back(properties[i]);
		if (is_int) {
			values.push_back((int64_t)spin_sliders[i]->get_value());
		} else {
			values.push_back(spin_sliders[i]->get_value());
		}
	}
	emit_signal(SNAME("multiple_properties_changed"), changed, values, false);
}

void EditorPropertySideGroup::_store_link(bool p_linked) {
	if (!get_edited_object() || properties.is_empty()) {
		return;
	}
	const String key = vformat("%s:%s", get_edited_object()->get_class(), properties[0]);
	EditorSettings::get_singleton()->set_project_metadata("linked_properties", key, p_linked);
}

void EditorPropertySideGroup::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (get_edited_object() && !properties.is_empty()) {
				const String key = vformat("%s:%s", get_edited_object()->get_class(), properties[0]);
				linked->set_pressed_no_signal(EditorSettings::get_singleton()->get_project_metadata("linked_properties", key, false));
			}
		} break;

		case NOTIFICATION_THEME_CHANGED: {
			int icon_size = get_theme_constant(SNAME("class_icon_size"), EditorStringName(Editor));
			for (int i = 0; i < spin_icons.size(); i++) {
				if (i < icon_names.size()) {
					spin_icons[i]->set_texture(get_editor_theme_icon(icon_names[i]));
				}
				spin_icons[i]->set_custom_minimum_size(Size2(icon_size, icon_size));
			}
			linked->set_texture_normal(get_editor_theme_icon(SNAME("Unlinked")));
			linked->set_texture_pressed(get_editor_theme_icon(SNAME("Instance")));
			linked->set_custom_minimum_size(Size2(icon_size + 8 * EDSCALE, 0));
		} break;
	}
}

void EditorPropertySideGroup::update_property() {
	Object *obj = get_edited_object();
	if (!obj) {
		return;
	}
	for (int i = 0; i < spin_sliders.size(); i++) {
		spin_sliders[i]->set_value_no_signal((double)obj->get(properties[i]));
	}
}

void EditorPropertySideGroup::setup(const Vector<String> &p_properties, const Vector<String> &p_icons, const Vector<String> &p_labels,
		bool p_is_int, double p_min, double p_max, double p_step,
		bool p_allow_greater, bool p_allow_lesser, const String &p_suffix,
		LayoutType p_layout) {
	properties = p_properties;
	icon_names = p_icons;
	is_int = p_is_int;
	layout_type = p_layout;

	// Create the 4 icon + slider pairs. These are stored by index so that
	// _value_changed() / update_property() / NOTIFICATION_THEME_CHANGED can
	// access them directly, regardless of their position in the grid.
	spin_sliders.resize(4);
	spin_icons.resize(4);
	EditorSpinSlider **spin = spin_sliders.ptrw();
	TextureRect **icon = spin_icons.ptrw();
	for (int i = 0; i < 4; i++) {
		icon[i] = memnew(TextureRect);
		icon[i]->set_stretch_mode(TextureRect::STRETCH_KEEP_CENTERED);
		icon[i]->set_v_size_flags(SIZE_SHRINK_CENTER);

		spin[i] = memnew(EditorSpinSlider);
		spin[i]->set_flat(true);
		spin[i]->set_h_size_flags(SIZE_EXPAND_FILL);
		spin[i]->set_accessibility_name(p_labels[i]);
		spin[i]->set_min(p_min);
		spin[i]->set_max(p_max);
		spin[i]->set_step(p_step);
		spin[i]->set_allow_greater(p_allow_greater);
		spin[i]->set_allow_lesser(p_allow_lesser);
		spin[i]->set_suffix(p_suffix);
		spin[i]->set_editing_integer(p_is_int);
		spin[i]->connect(SceneStringName(value_changed), callable_mp(this, &EditorPropertySideGroup::_value_changed).bind(i));
		add_focusable(spin[i]);
	}

	if (p_layout == LAYOUT_DIAMOND) {
		// Diamond layout (sides):  _ T _  /  L _ R  /  _ B _
		// Properties by index: L=0, T=1, R=2, B=3.
		// Grid slot order (left-to-right, top-to-bottom) with -1 = empty spacer:
		//   slot 0:spacer  slot 1:T  slot 2:spacer
		//   slot 3:L       slot 4:spacer  slot 5:R
		//   slot 6:spacer  slot 7:B  slot 8:spacer
		static const int diamond_order[9] = { -1, 1, -1, 0, -1, 2, -1, 3, -1 };
		grid->set_columns(3);
		for (int slot = 0; slot < 9; slot++) {
			int idx = diamond_order[slot];
			if (idx < 0) {
				Control *spc = memnew(Control);
				spc->set_h_size_flags(SIZE_EXPAND_FILL);
				grid->add_child(spc);
			} else {
				HBoxContainer *cell = memnew(HBoxContainer);
				cell->set_h_size_flags(SIZE_EXPAND_FILL);
				cell->add_child(spin_icons[idx]);
				cell->add_child(spin_sliders[idx]);
				grid->add_child(cell);
			}
		}
	} else {
		// LAYOUT_PAIR: 2×2 grid (corners). Properties TL=0, TR=1, BL=2, BR=3.
		// _update_grid_columns() auto-collapses to 1-per-row when panel is narrow.
		grid->set_columns(2);
		for (int i = 0; i < 4; i++) {
			HBoxContainer *cell = memnew(HBoxContainer);
			cell->set_h_size_flags(SIZE_EXPAND_FILL);
			cell->add_child(spin_icons[i]);
			cell->add_child(spin_sliders[i]);
			grid->add_child(cell);
		}
	}
}

void EditorPropertySideGroup::_update_grid_columns() {
	if (layout_type == LAYOUT_DIAMOND) {
		return; // Diamond layout is always 3 columns.
	}
	// LAYOUT_PAIR: 2 compound cells per row when wide, 1 per row when narrow.
	grid->set_columns(grid->get_size().x >= 200.0f * EDSCALE ? 2 : 1);
}

EditorPropertySideGroup::EditorPropertySideGroup() {
	HBoxContainer *hb = memnew(HBoxContainer);
	hb->set_h_size_flags(SIZE_EXPAND_FILL);

	// Small indent so the first icon doesn't touch the left edge.
	Control *spacer = memnew(Control);
	spacer->set_custom_minimum_size(Size2(8 * EDSCALE, 0));
	hb->add_child(spacer);

	grid = memnew(GridContainer);
	grid->set_h_size_flags(SIZE_EXPAND_FILL);
	grid->connect(SNAME("resized"), callable_mp(this, &EditorPropertySideGroup::_update_grid_columns));
	hb->add_child(grid);

	linked = memnew(TextureButton);
	linked->set_toggle_mode(true);
	linked->set_stretch_mode(TextureButton::STRETCH_KEEP_CENTERED);
	linked->set_tooltip_text(TTR("Lock/Unlock Link"));
	linked->set_v_size_flags(SIZE_SHRINK_CENTER);
	linked->connect(SceneStringName(toggled), callable_mp(this, &EditorPropertySideGroup::_store_link));
	hb->add_child(linked);

	set_bottom_editor(hb);
	add_child(hb);
}

// StyleBoxPreview

bool StyleBoxPreview::grid_preview_enabled = true;

void StyleBoxPreview::_grid_preview_toggled(bool p_active) {
	grid_preview_enabled = p_active;
	queue_redraw();
}

void StyleBoxPreview::edit(const Ref<StyleBox> &p_stylebox) {
	if (stylebox.is_valid()) {
		stylebox->disconnect_changed(callable_mp((CanvasItem *)this, &CanvasItem::queue_redraw));
	}
	stylebox = p_stylebox;
	if (stylebox.is_valid()) {
		stylebox->connect_changed(callable_mp((CanvasItem *)this, &CanvasItem::queue_redraw));
	}
	Ref<StyleBoxTexture> sbt = stylebox;
	grid_preview->set_visible(sbt.is_valid());
	queue_redraw();
}

void StyleBoxPreview::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			set_texture(get_editor_theme_icon(SNAME("Checkerboard")));
			grid_preview->set_button_icon(get_editor_theme_icon(SNAME("StyleBoxGrid")));
		} break;
		case NOTIFICATION_DRAW: {
			_redraw();
		} break;
	}
}

void StyleBoxPreview::_redraw() {
	if (stylebox.is_valid()) {
		float grid_button_width = get_editor_theme_icon(SNAME("StyleBoxGrid"))->get_size().x;
		Rect2 preview_rect = get_rect();
		preview_rect = preview_rect.grow(-grid_button_width);

		// Re-adjust preview panel to fit all drawn content.
		Rect2 drawing_rect = stylebox->get_draw_rect(preview_rect);
		preview_rect.size -= drawing_rect.size - preview_rect.size;
		preview_rect.position -= drawing_rect.position - preview_rect.position;

		draw_style_box(stylebox, preview_rect);

		Ref<StyleBoxTexture> sbt = stylebox;
		// Draw the "grid". Use white lines, as well as subtle black lines to ensure contrast.
		if (sbt.is_valid() && grid_preview->is_pressed()) {
			const Color dark_color = Color(0, 0, 0, 0.4);
			const Color bright_color = Color(1, 1, 1, 0.8);
			int x_left = drawing_rect.position.x + sbt->get_margin(SIDE_LEFT);
			int x_right = drawing_rect.position.x + drawing_rect.size.width - sbt->get_margin(SIDE_RIGHT);
			int y_top = drawing_rect.position.y + sbt->get_margin(SIDE_TOP);
			int y_bottom = drawing_rect.position.y + drawing_rect.size.height - sbt->get_margin(SIDE_BOTTOM);

			draw_line(Point2(x_left + 2, 0), Point2(x_left + 2, get_size().height), dark_color);
			draw_line(Point2(x_right + 1, 0), Point2(x_right + 1, get_size().height), dark_color);
			draw_line(Point2(0, y_top + 2), Point2(get_size().width, y_top + 2), dark_color);
			draw_line(Point2(0, y_bottom + 1), Point2(get_size().width, y_bottom + 1), dark_color);

			draw_line(Point2(x_left + 1, 0), Point2(x_left + 1, get_size().height), bright_color);
			draw_line(Point2(x_right, 0), Point2(x_right, get_size().height), bright_color);
			draw_line(Point2(0, y_top + 1), Point2(get_size().width, y_top + 1), bright_color);
			draw_line(Point2(0, y_bottom), Point2(get_size().width, y_bottom), bright_color);
		}
	}
}

StyleBoxPreview::StyleBoxPreview() {
	set_clip_contents(true);
	set_custom_minimum_size(Size2(0, 150) * EDSCALE);
	set_stretch_mode(TextureRect::STRETCH_TILE);
	set_texture_repeat(CanvasItem::TEXTURE_REPEAT_ENABLED);
	set_anchors_and_offsets_preset(PRESET_FULL_RECT);

	grid_preview = memnew(Button);
	// This theme variation works better than the normal theme because there's no focus highlight.
	grid_preview->set_theme_type_variation("PreviewLightButton");
	grid_preview->set_tooltip_text(TTRC("Toggle margins preview grid."));
	grid_preview->set_toggle_mode(true);
	grid_preview->connect(SceneStringName(toggled), callable_mp(this, &StyleBoxPreview::_grid_preview_toggled));
	grid_preview->set_pressed(grid_preview_enabled);
	add_child(grid_preview);
}

bool EditorInspectorPluginStyleBox::can_handle(Object *p_object) {
	return Object::cast_to<StyleBox>(p_object) != nullptr;
}

void EditorInspectorPluginStyleBox::parse_begin(Object *p_object) {
	Ref<StyleBox> sb = Ref<StyleBox>(Object::cast_to<StyleBox>(p_object));

	StyleBoxPreview *preview = memnew(StyleBoxPreview);
	preview->edit(sb);
	add_custom_control(preview);
}

bool EditorInspectorPluginStyleBox::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
	// Group content_margin_* for all StyleBox types.
	static const Vector<String> content_props = { "content_margin_left", "content_margin_top", "content_margin_right", "content_margin_bottom" };
	static const Vector<String> side_icons = { "ControlAlignCenterLeft", "ControlAlignCenterTop", "ControlAlignCenterRight", "ControlAlignCenterBottom" };
	static const Vector<String> side_labels = { "Left", "Top", "Right", "Bottom" };

	if (p_path == "content_margin_left") {
		EditorPropertySideGroup *ep = memnew(EditorPropertySideGroup);
		ep->setup(content_props, side_icons, side_labels, false, -1.0, 2048.0, 1.0, false, false, "px", EditorPropertySideGroup::LAYOUT_DIAMOND);
		add_property_editor_for_multiple_properties(TTR("Content Margins"), content_props, ep);
		return true;
	}
	if (p_path == "content_margin_top" || p_path == "content_margin_right" || p_path == "content_margin_bottom") {
		return true;
	}

	// StyleBoxFlat-specific groups.
	if (!Object::cast_to<StyleBoxFlat>(p_object)) {
		return false;
	}

	// Border Width group (int, order: left, top, right, bottom).
	static const Vector<String> border_props = { "border_width_left", "border_width_top", "border_width_right", "border_width_bottom" };

	if (p_path == "border_width_left") {
		EditorPropertySideGroup *ep = memnew(EditorPropertySideGroup);
		ep->setup(border_props, side_icons, side_labels, true, 0.0, 100.0, 1.0, true, false, "px", EditorPropertySideGroup::LAYOUT_DIAMOND);
		add_property_editor_for_multiple_properties(TTR("Border Width"), border_props, ep);
		return true;
	}
	if (p_path == "border_width_top" || p_path == "border_width_right" || p_path == "border_width_bottom") {
		return true;
	}

	// Corner Radius group (int). Visual order matches spatial position: TL=0, TR=1, BL=2, BR=3.
	static const Vector<String> corner_props = { "corner_radius_top_left", "corner_radius_top_right", "corner_radius_bottom_left", "corner_radius_bottom_right" };
	static const Vector<String> corner_icons = { "ControlAlignTopLeft", "ControlAlignTopRight", "ControlAlignBottomLeft", "ControlAlignBottomRight" };
	static const Vector<String> corner_labels = { "Top Left", "Top Right", "Bottom Left", "Bottom Right" };

	if (p_path == "corner_radius_top_left") {
		EditorPropertySideGroup *ep = memnew(EditorPropertySideGroup);
		ep->setup(corner_props, corner_icons, corner_labels, true, 0.0, 100.0, 1.0, true, false, "px", EditorPropertySideGroup::LAYOUT_PAIR);
		add_property_editor_for_multiple_properties(TTR("Corner Radius"), corner_props, ep);
		return true;
	}
	if (p_path == "corner_radius_top_right" || p_path == "corner_radius_bottom_right" || p_path == "corner_radius_bottom_left") {
		return true;
	}

	// Expand Margins group (float, order: left, top, right, bottom).
	static const Vector<String> expand_props = { "expand_margin_left", "expand_margin_top", "expand_margin_right", "expand_margin_bottom" };

	if (p_path == "expand_margin_left") {
		EditorPropertySideGroup *ep = memnew(EditorPropertySideGroup);
		ep->setup(expand_props, side_icons, side_labels, false, 0.0, 100.0, 1.0, true, false, "px", EditorPropertySideGroup::LAYOUT_DIAMOND);
		add_property_editor_for_multiple_properties(TTR("Expand Margins"), expand_props, ep);
		return true;
	}
	if (p_path == "expand_margin_top" || p_path == "expand_margin_right" || p_path == "expand_margin_bottom") {
		return true;
	}

	return false;
}

StyleBoxEditorPlugin::StyleBoxEditorPlugin() {
	Ref<EditorInspectorPluginStyleBox> inspector_plugin;
	inspector_plugin.instantiate();
	add_inspector_plugin(inspector_plugin);
}
