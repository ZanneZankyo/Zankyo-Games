#include "UIElement.hpp"
#include "..\Core\Hsv.hpp"
#include "..\Math\MathUtils.hpp"

UIElement::UIElement()
	: m_position()
	, m_size()
	, m_pivot()
	, m_margin()
	, m_paddins()
	, m_children()
	, m_parent(nullptr)
	, m_fillColor(255,255,255,0)
	, m_unit(UI_UNIT_ABSOLUTE)
	, m_absoluteBound()
	, m_align(UI_ALIGN_LEFT_TOP)
{
	m_fillColor = Hsv(GetRandomFloatZeroToOne() * 360.f, 1.f, 1.f).ToRgba();
}

UIElement::~UIElement()
{
	RemoveSelf();
	for (UIElement* child : m_children)
		delete child;
}

void UIElement::SetAlign(UIAlign align)
{

}

void UIElement::SetPivot(const Vector2 & pivot)
{
	m_pivot = pivot;
}

void UIElement::SetSize(const ui_metric_t& size)
{
	m_size = size;
}

void UIElement::SetPosition(const ui_metric_t& position)
{
	m_position = position;
}

Vector2 UIElement::GetSize() const
{
	return m_size.offset;
}

Vector2 UIElement::GetRelativeSize() const
{
	if (!m_parent)
		return m_size.offset;
	return m_parent->GetRelativeSize() * m_size.ratio + m_size.offset;
}

Vector2 UIElement::GetParentSize() const
{
	if(!m_parent)
		return Vector2();
	if (m_parent && !m_parent->m_parent)
		return m_parent->m_size.offset;
	return m_parent->GetParentSize() * m_parent->m_size.ratio + m_parent->m_size.offset;
}

AABB2 UIElement::GetLocalBounds() const
{
	Vector2 center;// = GetRelativePosition();
	center -= (m_pivot - Vector2(0.5f, 0.5f)) * m_size.offset;
	AABB2 bound = AABB2::CreateFromCenterAndSize(center, GetRelativeSize());
	return bound;
}

AABB2 UIElement::GetRelativeBounds() const
{
	return GetLocalBounds() + GetParentPosition();
}

Vector2 UIElement::GetLocalPosition() const
{
	if (!m_parent)
		return m_position.offset;
	return m_position.offset + GetParentSize() * m_position.ratio;
}

Vector2 UIElement::GetRelativePosition() const
{
	if (!m_parent)
		return m_position.offset;
	Vector2 relativePosition = m_parent->GetRelativePosition() + m_position.ratio * GetParentSize() + m_position.offset;
	return relativePosition;
}

Vector2 UIElement::GetParentPosition() const
{
	if(!m_parent)
		return Vector2();
	return m_parent->GetParentPosition();
}

Matrix4 UIElement::GetLocalTransform() const
{
	Vector2 localPosition = GetLocalPosition();
	return Matrix4::CreateTranslation(localPosition);
}

Matrix4 UIElement::GetWorldTransform() const
{
	Matrix4 mat = GetLocalTransform();
	if (m_parent)
		mat = mat * m_parent->GetWorldTransform();
	return mat;
}

void UIElement::AddChildElement(UIElement * childElement)
{
	if (childElement->GetParent())
		childElement->RemoveSelf();
	childElement->m_parent = this;
	m_children.push_back(childElement);
}

void UIElement::RemoveChildElement(UIElement * childElement)
{

}

void UIElement::RemoveSelf()
{
	UIElement* parent = m_parent;
	if (!parent)
		return;
	for (size_t index = 0; index < parent->m_children.size(); index++)
	{
		if (parent->m_children[index] == this)
		{
			parent->m_children.erase(parent->m_children.begin() + index);
			return;
		}
	}
}

UIElement * UIElement::GetParent()
{
	return m_parent;
}
