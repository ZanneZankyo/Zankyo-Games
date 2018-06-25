#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

SpriteAnimation::SpriteAnimation()
	: m_spriteSheet()
	, m_startSpriteIndex(0)
	, m_endSpriteIndex(0)
	, m_durationSeconds(0.f)
	, m_ageInSeconds(0.f)
	, m_playMode(SPRITE_ANIME_SINGLE)
	, m_isPlaying(false)
{}

SpriteAnimation::SpriteAnimation(const SpriteSheet & sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimePlayMode playMode)
	:m_spriteSheet(sheet)
	,m_startSpriteIndex(startSpriteIndex)
	,m_endSpriteIndex(endSpriteIndex)
	,m_durationSeconds(durationSeconds)
	,m_ageInSeconds(0.f)
	,m_playMode(playMode)
	,m_isPlaying(true)
{
}

void SpriteAnimation::Update(float deltaSeconds)
{
	if (m_playMode == SPRITE_ANIME_LOOP && m_ageInSeconds >= m_durationSeconds)
	{
		m_ageInSeconds = 0.f;
	}
	
	if(m_isPlaying && m_ageInSeconds < m_durationSeconds)
	{
		m_ageInSeconds += deltaSeconds;
		if (m_ageInSeconds > m_durationSeconds)
			m_ageInSeconds = m_durationSeconds;
	}
}

void SpriteAnimation::Pause()
{
	m_isPlaying = false;
}

void SpriteAnimation::Resume()
{
	m_isPlaying = true;
}

void SpriteAnimation::Reset()
{
	m_ageInSeconds = 0.f;
}

AABB2 SpriteAnimation::GetTextureCoordsAtFrationCompelete(float percent) const
{
	int index = m_startSpriteIndex + static_cast<int>((m_endSpriteIndex - m_startSpriteIndex) * percent);
	return m_spriteSheet.GetTextCoordsForSpriteIndex(index);
}

AABB2 SpriteAnimation::GetCurrectTextureCoords() const
{
	return GetTextureCoordsAtFrationCompelete(m_ageInSeconds / m_durationSeconds);
}

const Texture* SpriteAnimation::GetTexture() const
{
	return m_spriteSheet.m_texture;
}

float SpriteAnimation::GetDurationSeconds() const
{
	return m_durationSeconds;
}

float SpriteAnimation::GetSecondsElapsed() const
{
	return m_ageInSeconds;
}

float SpriteAnimation::GetFractionElapsed() const
{
	return m_ageInSeconds / m_durationSeconds;
}

float SpriteAnimation::GetFractionRemaining() const
{
	return 1.f- GetFractionElapsed();
}

SpriteAnimePlayMode SpriteAnimation::GetPlayMode() const
{
	return m_playMode;
}

bool SpriteAnimation::IsFinished() const
{
	return m_playMode == SPRITE_ANIME_SINGLE && m_ageInSeconds >= m_durationSeconds;
}

bool SpriteAnimation::IsPlaying() const
{
	return m_isPlaying;
}

void SpriteAnimation::SetPlayMode(SpriteAnimePlayMode playmode)
{
	m_playMode = playmode;
}

void SpriteAnimation::SetSecondsElapsed(float secondsElapsed)
{
	m_ageInSeconds = secondsElapsed;
}

void SpriteAnimation::SetFractionElapsed(float fractionElapsed)
{
	m_ageInSeconds = m_durationSeconds * fractionElapsed;
}

