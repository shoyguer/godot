/**************************************************************************/
/*  spring_bone_settings.h                                                */
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

#include "core/io/resource.h"
#include "core/math/vector3.h"
#include "scene/resources/curve.h"

class SpringBoneSettings : public Resource {
	GDCLASS(SpringBoneSettings, Resource);
	OBJ_SAVE_TYPE(SpringBoneSettings);
	RES_BASE_EXTENSION("springbonesettings");

	float radius = 0.02;
	Ref<Curve> radius_damping_curve;
	float stiffness = 1.0;
	Ref<Curve> stiffness_damping_curve;
	float drag = 0.4;
	Ref<Curve> drag_damping_curve;
	float gravity = 0.0;
	Ref<Curve> gravity_damping_curve;
	Vector3 gravity_direction = Vector3(0, -1, 0);

	void _curve_changed();
	void _set_damping_curve(Ref<Curve> &r_curve, const Ref<Curve> &p_curve);

protected:
	static void _bind_methods();

public:
	void set_radius(float p_radius);
	float get_radius() const { return radius; }
	void set_radius_damping_curve(const Ref<Curve> &p_damping_curve);
	Ref<Curve> get_radius_damping_curve() const { return radius_damping_curve; }

	void set_stiffness(float p_stiffness);
	float get_stiffness() const { return stiffness; }
	void set_stiffness_damping_curve(const Ref<Curve> &p_damping_curve);
	Ref<Curve> get_stiffness_damping_curve() const { return stiffness_damping_curve; }

	void set_drag(float p_drag);
	float get_drag() const { return drag; }
	void set_drag_damping_curve(const Ref<Curve> &p_damping_curve);
	Ref<Curve> get_drag_damping_curve() const { return drag_damping_curve; }

	void set_gravity(float p_gravity);
	float get_gravity() const { return gravity; }
	void set_gravity_damping_curve(const Ref<Curve> &p_damping_curve);
	Ref<Curve> get_gravity_damping_curve() const { return gravity_damping_curve; }
	void set_gravity_direction(const Vector3 &p_gravity_direction);
	Vector3 get_gravity_direction() const { return gravity_direction; }

	bool is_equal_to(const Ref<SpringBoneSettings> &p_other) const;

	~SpringBoneSettings();
};
