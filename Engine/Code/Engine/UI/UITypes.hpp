#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Core\RGBA.hpp"

struct ui_metric_t
{
	Vector2 ratio;
	Vector2 offset;
	ui_metric_t(const Vector2& r = Vector2(), const Vector2 o = Vector2());
	ui_metric_t operator + (const ui_metric_t& metric);
};

enum UIAlign
{
	UI_ALIGN_LEFT_TOP,
	UI_ALIGN_CENTER_TOP,
	UI_ALIGN_RIGHT_TOP,
	UI_ALIGN_LEFT_CENTER,
	UI_ALIGN_CENTER,
	UI_ALIGN_RIGHT_CENTER,
	UI_ALIGN_LEFT_BOTTOM,
	UI_ALIGN_CENTER_BOTTOM,
	UI_ALIGN_RIGHT_BOTTOM,
	UI_ALIGN_RELATIVE,
	UI_ALIGN_ABSOLUTE,
	NUM_OF_UI_ALIGNS
};

enum UIUnit
{
	UI_UNIT_ABSOLUTE,
	UI_UNIT_PERCENTAGE,
};

enum UIPivot
{

};

namespace UI
{
	Vector2 AlignToPosition();
}

