#include "UICanvas.hpp"

void UICanvas::SetRenderer(D3D11Renderer* renderer)
{
	m_renderer = renderer;
}

void UICanvas::SetHeightResolution(float height)
{
	m_size.offset.y = height;
	IntVector2 windowSize = m_renderer->GetWindowSize();
	float screenRatio = windowSize.x / (float)windowSize.y;
	float width = height * screenRatio;
	m_size.offset.x = width;
}

void UICanvas::Render() const
{
	IntVector2 windowSize = m_renderer->GetWindowSize();
	float screenRatio = windowSize.x / (float)windowSize.y;
	float height = m_size.offset.y;
	float width = height * screenRatio;
	m_renderer->SetOrthoProjection(Vector2(width, height));
	m_renderer->SetMaterial("Blank");
	RenderElement((UIElement*)this);
}

void UICanvas::RenderElement(UIElement * element) const
{
	std::vector<Vertex3> vertices = MeshBuilder::Squard(element->GetLocalBounds(), element->m_fillColor);
	m_renderer->DrawVertices(vertices);
	for (UIElement* childElem : element->m_children)
	{
		m_renderer->PushMatrix();
		m_renderer->MultMatrix(childElem->GetLocalTransform());
		RenderElement(childElem);
		m_renderer->PopMatrix();
	}
}

