/**************************************************************************/
/*  spring_bone_settings.cpp                                              */
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

#include "spring_bone_settings.h"

#include "core/object/callable_mp.h"
#include "core/object/class_db.h"

void SpringBoneSettings::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &SpringBoneSettings::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &SpringBoneSettings::get_radius);
	ClassDB::bind_method(D_METHOD("set_radius_damping_curve", "curve"), &SpringBoneSettings::set_radius_damping_curve);
	ClassDB::bind_method(D_METHOD("get_radius_damping_curve"), &SpringBoneSettings::get_radius_damping_curve);

	ClassDB::bind_method(D_METHOD("set_stiffness", "stiffness"), &SpringBoneSettings::set_stiffness);
	ClassDB::bind_method(D_METHOD("get_stiffness"), &SpringBoneSettings::get_stiffness);
	ClassDB::bind_method(D_METHOD("set_stiffness_damping_curve", "curve"), &SpringBoneSettings::set_stiffness_damping_curve);
	ClassDB::bind_method(D_METHOD("get_stiffness_damping_curve"), &SpringBoneSettings::get_stiffness_damping_curve);

	ClassDB::bind_method(D_METHOD("set_drag", "drag"), &SpringBoneSettings::set_drag);
	ClassDB::bind_method(D_METHOD("get_drag"), &SpringBoneSettings::get_drag);
	ClassDB::bind_method(D_METHOD("set_drag_damping_curve", "curve"), &SpringBoneSettings::set_drag_damping_curve);
	ClassDB::bind_method(D_METHOD("get_drag_damping_curve"), &SpringBoneSettings::get_drag_damping_curve);

	ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &SpringBoneSettings::set_gravity);
	ClassDB::bind_method(D_METHOD("get_gravity"), &SpringBoneSettings::get_gravity);
	ClassDB::bind_method(D_METHOD("set_gravity_damping_curve", "curve"), &SpringBoneSettings::set_gravity_damping_curve);
	ClassDB::bind_method(D_METHOD("get_gravity_damping_curve"), &SpringBoneSettings::get_gravity_damping_curve);
	ClassDB::bind_method(D_METHOD("set_gravity_direction", "gravity_direction"), &SpringBoneSettings::set_gravity_direction);
	ClassDB::bind_method(D_METHOD("get_gravity_direction"), &SpringBoneSettings::get_gravity_direction);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius/value", PROPERTY_HINT_RANGE, "0,1,0.001,or_greater,suffix:m"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "radius/damping_curve", PROPERTY_HINT_RESOURCE_TYPE, Curve::get_class_static()), "set_radius_damping_curve", "get_radius_damping_curve");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "stiffness/value", PROPERTY_HINT_RANGE, "0,4,0.01,or_greater"), "set_stiffness", "get_stiffness");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stiffness/damping_curve", PROPERTY_HINT_RESOURCE_TYPE, Curve::get_class_static()), "set_stiffness_damping_curve", "get_stiffness_damping_curve");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "drag/value", PROPERTY_HINT_RANGE, "0,1,0.01,or_greater"), "set_drag", "get_drag");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "drag/damping_curve", PROPERTY_HINT_RESOURCE_TYPE, Curve::get_class_static()), "set_drag_damping_curve", "get_drag_damping_curve");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity/value", PROPERTY_HINT_RANGE, "0,1,0.01,or_greater,or_less,suffix:m/s"), "set_gravity", "get_gravity");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "gravity/damping_curve", PROPERTY_HINT_RESOURCE_TYPE, Curve::get_class_static()), "set_gravity_damping_curve", "get_gravity_damping_curve");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "gravity/direction"), "set_gravity_direction", "get_gravity_direction");
}

void SpringBoneSettings::_curve_changed() {
	emit_changed();
}

void SpringBoneSettings::_set_damping_curve(Ref<Curve> &r_curve, const Ref<Curve> &p_curve) {
	if (r_curve == p_curve) {
		return;
	}
	if (r_curve.is_valid()) {
		r_curve->disconnect_changed(callable_mp(this, &SpringBoneSettings::_curve_changed));
	}
	r_curve = p_curve;
	if (r_curve.is_valid()) {
		r_curve->connect_changed(callable_mp(this, &SpringBoneSettings::_curve_changed), CONNECT_REFERENCE_COUNTED);
	}
	emit_changed();
}

void SpringBoneSettings::set_radius(float p_radius) {
	if (Math::is_equal_approx(radius, p_radius)) {
		return;
	}
	radius = p_radius;
	emit_changed();
}

void SpringBoneSettings::set_radius_damping_curve(const Ref<Curve> &p_damping_curve) {
	_set_damping_curve(radius_damping_curve, p_damping_curve);
}

void SpringBoneSettings::set_stiffness(float p_stiffness) {
	if (Math::is_equal_approx(stiffness, p_stiffness)) {
		return;
	}
	stiffness = p_stiffness;
	emit_changed();
}

void SpringBoneSettings::set_stiffness_damping_curve(const Ref<Curve> &p_damping_curve) {
	_set_damping_curve(stiffness_damping_curve, p_damping_curve);
}

void SpringBoneSettings::set_drag(float p_drag) {
	if (Math::is_equal_approx(drag, p_drag)) {
		return;
	}
	drag = p_drag;
	emit_changed();
}

void SpringBoneSettings::set_drag_damping_curve(const Ref<Curve> &p_damping_curve) {
	_set_damping_curve(drag_damping_curve, p_damping_curve);
}

void SpringBoneSettings::set_gravity(float p_gravity) {
	if (Math::is_equal_approx(gravity, p_gravity)) {
		return;
	}
	gravity = p_gravity;
	emit_changed();
}

void SpringBoneSettings::set_gravity_damping_curve(const Ref<Curve> &p_damping_curve) {
	_set_damping_curve(gravity_damping_curve, p_damping_curve);
}

void SpringBoneSettings::set_gravity_direction(const Vector3 &p_gravity_direction) {
	ERR_FAIL_COND(p_gravity_direction.is_zero_approx());
	if (gravity_direction.is_equal_approx(p_gravity_direction)) {
		return;
	}
	gravity_direction = p_gravity_direction;
	emit_changed();
}

static bool _curves_are_equal(const Ref<Curve> &p_a, const Ref<Curve> &p_b) {
	if (p_a == p_b) {
		return true;
	}
	if (p_a.is_null() || p_b.is_null()) {
		return false;
	}
	if (p_a->get_point_count() != p_b->get_point_count() || p_a->get_bake_resolution() != p_b->get_bake_resolution()) {
		return false;
	}
	if (!Math::is_equal_approx(p_a->get_min_value(), p_b->get_min_value()) ||
			!Math::is_equal_approx(p_a->get_max_value(), p_b->get_max_value()) ||
			!Math::is_equal_approx(p_a->get_min_domain(), p_b->get_min_domain()) ||
			!Math::is_equal_approx(p_a->get_max_domain(), p_b->get_max_domain())) {
		return false;
	}
	for (int i = 0; i < p_a->get_point_count(); i++) {
		const Curve::Point pa = p_a->get_point(i);
		const Curve::Point pb = p_b->get_point(i);
		if (!pa.position.is_equal_approx(pb.position) ||
				!Math::is_equal_approx(pa.left_tangent, pb.left_tangent) ||
				!Math::is_equal_approx(pa.right_tangent, pb.right_tangent) ||
				pa.left_mode != pb.left_mode ||
				pa.right_mode != pb.right_mode) {
			return false;
		}
	}
	return true;
}

bool SpringBoneSettings::is_equal_to(const Ref<SpringBoneSettings> &p_other) const {
	if (p_other.is_null()) {
		return false;
	}
	if (p_other.ptr() == this) {
		return true;
	}
	return Math::is_equal_approx(radius, p_other->radius) &&
			Math::is_equal_approx(stiffness, p_other->stiffness) &&
			Math::is_equal_approx(drag, p_other->drag) &&
			Math::is_equal_approx(gravity, p_other->gravity) &&
			gravity_direction.is_equal_approx(p_other->gravity_direction) &&
			_curves_are_equal(radius_damping_curve, p_other->radius_damping_curve) &&
			_curves_are_equal(stiffness_damping_curve, p_other->stiffness_damping_curve) &&
			_curves_are_equal(drag_damping_curve, p_other->drag_damping_curve) &&
			_curves_are_equal(gravity_damping_curve, p_other->gravity_damping_curve);
}

SpringBoneSettings::~SpringBoneSettings() {
	if (radius_damping_curve.is_valid()) {
		radius_damping_curve->disconnect_changed(callable_mp(this, &SpringBoneSettings::_curve_changed));
	}
	if (stiffness_damping_curve.is_valid()) {
		stiffness_damping_curve->disconnect_changed(callable_mp(this, &SpringBoneSettings::_curve_changed));
	}
	if (drag_damping_curve.is_valid()) {
		drag_damping_curve->disconnect_changed(callable_mp(this, &SpringBoneSettings::_curve_changed));
	}
	if (gravity_damping_curve.is_valid()) {
		gravity_damping_curve->disconnect_changed(callable_mp(this, &SpringBoneSettings::_curve_changed));
	}
}
