#pragma once
#include "UITypes.hpp"
#include "UIElement.hpp"
#include "..\RHI\Texture2D.hpp"

class UISprite : public UIElement
{
public:
	enum FillMode
	{
		FILL_STRETCH
	};
	Texture2D* m_texture;

	void SetTexture(Texture2D* texture);
};