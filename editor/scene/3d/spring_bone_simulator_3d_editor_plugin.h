/**************************************************************************/
/*  spring_bone_simulator_3d_editor_plugin.h                              */
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
#include "scene/gui/box_container.h"

class CheckBox;
class ConfirmationDialog;
class ItemList;
class OptionButton;
class SpringBoneSimulator3D;

class SpringBoneSimulator3DEditor : public VBoxContainer {
	GDCLASS(SpringBoneSimulator3DEditor, VBoxContainer);

	SpringBoneSimulator3D *simulator = nullptr;

	ConfirmationDialog *copy_dialog = nullptr;
	OptionButton *source_option = nullptr;
	OptionButton *mode_option = nullptr;
	ItemList *target_list = nullptr;
	CheckBox *copy_rotation_axis = nullptr;

	void _open_copy_dialog();
	void _populate_copy_dialog();
	void _select_all_targets();
	void _select_none_targets();
	void _copy_dialog_confirmed();

	String _get_chain_label(int p_index) const;

public:
	SpringBoneSimulator3DEditor(SpringBoneSimulator3D *p_simulator);
};

class EditorInspectorPluginSpringBoneSimulator3D : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginSpringBoneSimulator3D, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_begin(Object *p_object) override;
};

class SpringBoneSimulator3DEditorPlugin : public EditorPlugin {
	GDCLASS(SpringBoneSimulator3DEditorPlugin, EditorPlugin);

public:
	SpringBoneSimulator3DEditorPlugin();
};
