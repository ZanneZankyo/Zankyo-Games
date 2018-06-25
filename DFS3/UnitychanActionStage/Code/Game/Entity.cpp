#include "Entity.hpp"
#include "Engine\Core\Time.hpp"

void Entity::Update()
{
	m_position += m_velocity * Time::deltaSeconds;
	if (m_pxActor)
		m_position = m_pxActor->GetTransform().GetPosition();
}

void Entity::Render() const
{

}

void Entity::SetPosition(const Vector3& position)
{
	m_position = position;
	if (m_pxActor)
	{
		Matrix4 transform = m_pxActor->GetTransform();
		transform.SetTranslate(position);
		m_pxActor->SetTransform(transform);
	}
}
