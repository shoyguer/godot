/**************************************************************************/
/*  spring_bone_simulator_3d_editor_plugin.cpp                            */
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

#include "spring_bone_simulator_3d_editor_plugin.h"

#include "core/object/callable_mp.h"
#include "editor/editor_string_names.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/themes/editor_scale.h"
#include "scene/3d/spring_bone_settings.h"
#include "scene/3d/spring_bone_simulator_3d.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/control.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/item_list.h"
#include "scene/gui/label.h"
#include "scene/gui/option_button.h"
#include "scene/scene_string_names.h"

String SpringBoneSimulator3DEditor::_get_chain_label(int p_index) const {
	const String root_name = simulator->get_root_bone_name(p_index);
	const String end_name = simulator->get_end_bone_name(p_index);
	if (root_name.is_empty() && end_name.is_empty()) {
		return vformat(TTR("Chain %d"), p_index);
	}
	if (end_name.is_empty()) {
		return vformat("%d: %s", p_index, root_name);
	}
	return vformat("%d: %s -> %s", p_index, root_name, end_name);
}

void SpringBoneSimulator3DEditor::_open_copy_dialog() {
	_populate_copy_dialog();
	copy_dialog->popup_centered_clamped(Size2(480, 520) * EDSCALE);
}

void SpringBoneSimulator3DEditor::_populate_copy_dialog() {
	source_option->clear();
	target_list->clear();

	const int count = simulator->get_setting_count();
	for (int i = 0; i < count; i++) {
		const String label = _get_chain_label(i);
		source_option->add_item(label);
		source_option->set_item_metadata(i, i);
		target_list->add_item(label);
		target_list->set_item_metadata(i, i);
		if (i != 0) {
			target_list->select(i, false);
		}
	}

	if (count > 0) {
		source_option->select(0);
	}
}

void SpringBoneSimulator3DEditor::_select_all_targets() {
	target_list->deselect_all();
	for (int i = 0; i < target_list->get_item_count(); i++) {
		target_list->select(i, false);
	}
}

void SpringBoneSimulator3DEditor::_select_none_targets() {
	target_list->deselect_all();
}

void SpringBoneSimulator3DEditor::_copy_dialog_confirmed() {
	if (!simulator || simulator->get_setting_count() == 0) {
		return;
	}

	const int source = source_option->get_selected_metadata();
	ERR_FAIL_INDEX(source, simulator->get_setting_count());

	Vector<int> targets;
	for (int i = 0; i < target_list->get_item_count(); i++) {
		if (!target_list->is_selected(i)) {
			continue;
		}
		const int target = target_list->get_item_metadata(i);
		if (target == source) {
			continue;
		}
		targets.push_back(target);
	}

	if (targets.is_empty()) {
		return;
	}

	Ref<SpringBoneSettings> source_res = simulator->get_spring_bone_settings(source);
	if (source_res.is_null()) {
		simulator->set_spring_bone_settings(source, Ref<SpringBoneSettings>());
		source_res = simulator->get_spring_bone_settings(source);
	}
	ERR_FAIL_COND(source_res.is_null());

	const bool assign_resource = mode_option->get_selected() == 1;
	const bool copy_axis = copy_rotation_axis->is_pressed();
	const SkeletonModifier3D::RotationAxis rotation_axis = simulator->get_rotation_axis(source);
	const Vector3 rotation_axis_vector = simulator->get_rotation_axis_vector(source);

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(assign_resource ? TTR("Assign Spring Bone Settings Resource") : TTR("Copy Spring Bone Chain Settings"));

	for (const int target : targets) {
		const Ref<SpringBoneSettings> old_settings = simulator->get_spring_bone_settings(target);
		Ref<SpringBoneSettings> new_settings;
		if (assign_resource) {
			new_settings = source_res;
		} else {
			new_settings = source_res->duplicate(true);
		}
		ur->add_do_method(simulator, "set_spring_bone_settings", target, new_settings);
		ur->add_undo_method(simulator, "set_spring_bone_settings", target, old_settings);

		if (copy_axis) {
			ur->add_do_method(simulator, "set_rotation_axis", target, rotation_axis);
			ur->add_do_method(simulator, "set_rotation_axis_vector", target, rotation_axis_vector);
			ur->add_undo_method(simulator, "set_rotation_axis", target, simulator->get_rotation_axis(target));
			ur->add_undo_method(simulator, "set_rotation_axis_vector", target, simulator->get_rotation_axis_vector(target));
		}
	}

	ur->commit_action();
}

SpringBoneSimulator3DEditor::SpringBoneSimulator3DEditor(SpringBoneSimulator3D *p_simulator) {
	simulator = p_simulator;

	EditorInspectorActionButton *copy_button = memnew(EditorInspectorActionButton(TTRC("Copy Chain Settings..."), SNAME("ActionCopy")));
	copy_button->set_disabled(simulator->get_setting_count() == 0);
	copy_button->connect(SceneStringName(pressed), callable_mp(this, &SpringBoneSimulator3DEditor::_open_copy_dialog));
	add_child(copy_button);

	add_child(memnew(Control)); // Padding before the regular properties.

	copy_dialog = memnew(ConfirmationDialog);
	copy_dialog->set_title(TTRC("Copy Chain Settings"));
	copy_dialog->set_ok_button_text(TTRC("Apply"));
	copy_dialog->connect(SceneStringName(confirmed), callable_mp(this, &SpringBoneSimulator3DEditor::_copy_dialog_confirmed));
	add_child(copy_dialog);

	VBoxContainer *vb = memnew(VBoxContainer);
	copy_dialog->add_child(vb);

	vb->add_child(memnew(Label(TTRC("Source Chain"))));
	source_option = memnew(OptionButton);
	source_option->set_h_size_flags(SIZE_EXPAND_FILL);
	vb->add_child(source_option);

	vb->add_child(memnew(Label(TTRC("Mode"))));
	mode_option = memnew(OptionButton);
	mode_option->set_h_size_flags(SIZE_EXPAND_FILL);
	mode_option->add_item(TTRC("Copy Values"));
	mode_option->add_item(TTRC("Assign Same Resource"));
	vb->add_child(mode_option);

	vb->add_child(memnew(Label(TTRC("Target Chains"))));

	HBoxContainer *select_hb = memnew(HBoxContainer);
	vb->add_child(select_hb);
	Button *select_all = memnew(Button);
	select_all->set_text(TTRC("Select All"));
	select_all->connect(SceneStringName(pressed), callable_mp(this, &SpringBoneSimulator3DEditor::_select_all_targets));
	select_hb->add_child(select_all);
	Button *select_none = memnew(Button);
	select_none->set_text(TTRC("Select None"));
	select_none->connect(SceneStringName(pressed), callable_mp(this, &SpringBoneSimulator3DEditor::_select_none_targets));
	select_hb->add_child(select_none);

	target_list = memnew(ItemList);
	target_list->set_select_mode(ItemList::SELECT_MULTI);
	target_list->set_v_size_flags(SIZE_EXPAND_FILL);
	target_list->set_custom_minimum_size(Size2(0, 180) * EDSCALE);
	vb->add_child(target_list);

	copy_rotation_axis = memnew(CheckBox);
	copy_rotation_axis->set_text(TTRC("Also Copy Rotation Axis"));
	vb->add_child(copy_rotation_axis);
}

bool EditorInspectorPluginSpringBoneSimulator3D::can_handle(Object *p_object) {
	return Object::cast_to<SpringBoneSimulator3D>(p_object) != nullptr;
}

void EditorInspectorPluginSpringBoneSimulator3D::parse_begin(Object *p_object) {
	SpringBoneSimulator3D *simulator = Object::cast_to<SpringBoneSimulator3D>(p_object);
	ERR_FAIL_NULL(simulator);
	SpringBoneSimulator3DEditor *editor = memnew(SpringBoneSimulator3DEditor(simulator));
	add_custom_control(editor);
}

SpringBoneSimulator3DEditorPlugin::SpringBoneSimulator3DEditorPlugin() {
	Ref<EditorInspectorPluginSpringBoneSimulator3D> plugin;
	plugin.instantiate();
	add_inspector_plugin(plugin);
}
