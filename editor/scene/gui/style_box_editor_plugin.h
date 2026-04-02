/**************************************************************************/
/*  style_box_editor_plugin.h                                             */
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

#pragma once

#include "editor/inspector/editor_inspector.h"
#include "editor/plugins/editor_plugin.h"
#include "scene/gui/texture_rect.h"

class Button;
class EditorSpinSlider;
class GridContainer;
class StyleBox;
class StyleBoxFlat;
class TextureButton;

class EditorPropertySideGroup : public EditorProperty {
	GDCLASS(EditorPropertySideGroup, EditorProperty);

	Vector<EditorSpinSlider *> spin_sliders;
	Vector<TextureRect *> spin_icons;
	Vector<String> icon_names;
	GridContainer *grid = nullptr;
	TextureButton *linked = nullptr;
	Vector<String> properties;
	bool is_int = false;

	void _value_changed(double p_val, int p_idx);
	void _store_link(bool p_linked);
	void _update_grid_columns();

protected:
	virtual void _set_read_only(bool p_read_only) override;
	void _notification(int p_what);

public:
	virtual void update_property() override;
	void setup(const Vector<String> &p_properties, const Vector<String> &p_icons, const Vector<String> &p_labels,
			bool p_is_int, double p_min, double p_max, double p_step,
			bool p_allow_greater, bool p_allow_lesser, const String &p_suffix);
	EditorPropertySideGroup();
};

class StyleBoxPreview : public TextureRect {
	GDCLASS(StyleBoxPreview, TextureRect);

	Button *grid_preview = nullptr;
	Ref<StyleBox> stylebox;

	void _sb_changed();
	void _redraw();
	static bool grid_preview_enabled;
	void _grid_preview_toggled(bool p_active);

protected:
	void _notification(int p_what);

public:
	void edit(const Ref<StyleBox> &p_stylebox);

	StyleBoxPreview();
};

class EditorInspectorPluginStyleBox : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginStyleBox, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_begin(Object *p_object) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;
};

class StyleBoxEditorPlugin : public EditorPlugin {
	GDCLASS(StyleBoxEditorPlugin, EditorPlugin);

public:
	virtual String get_plugin_name() const override { return "StyleBox"; }

	StyleBoxEditorPlugin();
};
