#pragma once

#include "UITypes.hpp"
#include <vector>

class UIElement
{
private:
	AABB2 m_absoluteBound;
public:
	ui_metric_t m_position;
	ui_metric_t m_size;
	Vector2 m_pivot;

	UIUnit m_unit;
	UIAlign m_align;

	Vector2 m_margin;
	Vector2 m_paddins;

	UIElement* m_parent;
	std::vector<UIElement*> m_children;

	Rgba m_fillColor;

public:
	UIElement();
	~UIElement();

	void SetAlign(UIAlign align);
	void SetPivot(const Vector2& pivot);
	void SetSize(const ui_metric_t& size);
	void SetPosition(const ui_metric_t& position);
	void SetRelativePosition(const Vector2& relativePosition);

	Vector2 GetSize() const;
	Vector2 GetRelativeSize() const;
	Vector2 GetParentSize() const;
	AABB2 GetLocalBounds() const;
	AABB2 GetRelativeBounds() const;
	Vector2 GetLocalPosition() const;
	Vector2 GetRelativePosition() const;
	Vector2 GetParentPosition() const;
	Matrix4 GetLocalTransform() const;
	Matrix4 GetWorldTransform() const;

	void AddChildElement(UIElement* childElement);
	void RemoveChildElement(UIElement* childElement);
	
	void RemoveSelf();
	UIElement* GetParent();
};