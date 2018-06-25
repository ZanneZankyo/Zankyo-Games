#pragma once
#include "Engine\Renderer\SpriteSheet.hpp"
#include "Engine\Math\AABB2.hpp"

enum SpriteAnimePlayMode
{
	SPRITE_ANIME_SINGLE,
	SPRITE_ANIME_LOOP,
	SPRITE_ANIME_PINGPONG,
};

class SpriteAnimation
{
public:

	const SpriteSheet	m_spriteSheet;
	int					m_startSpriteIndex;
	int					m_endSpriteIndex;
	float				m_durationSeconds;
	float				m_ageInSeconds;
	bool				m_isPlaying;
	SpriteAnimePlayMode m_playMode;

public:
	SpriteAnimation();
	SpriteAnimation(const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimePlayMode playMode = SPRITE_ANIME_LOOP);

	void Update(float deltaSeconds);
	
	void	Pause();
	void	Resume();
	void	Reset();
	AABB2	GetTextureCoordsAtFrationCompelete(float percent) const;
	AABB2	GetCurrectTextureCoords() const;
	const Texture* GetTexture() const;
	float	GetDurationSeconds() const;
	float	GetSecondsElapsed() const;
	float	GetFractionElapsed() const;
	float	GetFractionRemaining() const;
	SpriteAnimePlayMode GetPlayMode() const;
	bool	IsFinished() const;
	bool	IsPlaying() const;
	void	SetPlayMode(SpriteAnimePlayMode playmode);
	void	SetSecondsElapsed(float secondsElapsed);
	void	SetFractionElapsed(float fractionElapsed);

};