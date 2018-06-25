#pragma once
#include "UIElement.hpp"
#include "Engine\Renderer\D3D11Renderer.hpp"

class UICanvas : public UIElement
{
public:
	D3D11Renderer* m_renderer;
public:
	void SetRenderer(D3D11Renderer* renderer);
	void SetHeightResolution(float height);
	void Render() const;
	void RenderElement(UIElement* element) const;
};