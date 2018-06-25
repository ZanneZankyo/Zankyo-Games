#include "UITypes.hpp"

ui_metric_t::ui_metric_t(const Vector2 & r, const Vector2 o)
	: ratio(r)
	, offset(o)
{
}

ui_metric_t ui_metric_t::operator+(const ui_metric_t & metric)
{
	return ui_metric_t(ratio + metric.ratio, offset + metric.offset);
}
