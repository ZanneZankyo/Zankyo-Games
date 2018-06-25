#include "ParticleEmitter.hpp"
#include "Engine\Core\Console.hpp"

ParticleEmitter::ParticleEmitter(const ParticleEmitterDefinition* definition)
	: m_definition(definition)
	, m_particles()
	, m_isPlaying(true)
	, m_isFirstParticleSpawned(false)
	, m_spawnCountThisFrame(0.f)
	, m_lifeSpam(0.f)
{

}

ParticleEmitter::~ParticleEmitter()
{
	DestroyImmediate();
}

void ParticleEmitter::Stop()
{
	m_isPlaying = false;
}

void ParticleEmitter::Play()
{
	m_isPlaying = true;
}

void ParticleEmitter::Reset()
{
	DestroyImmediate();
	m_isFirstParticleSpawned = false;
}

void ParticleEmitter::Prewarm()
{
	float steppingTime = 1.f / 60.f;
	for (float time = 0; time < m_definition->m_lifeTime; time += steppingTime)
		Update(steppingTime);
}

bool ParticleEmitter::DestroyOnFinished()
{
	if (IsFinished())
	{
		DestroyImmediate();
		return true;
	}
	return false;
}

void ParticleEmitter::DestroyImmediate()
{
	for (auto particle : m_particles)
		delete particle;
	m_particles.clear();
}

bool ParticleEmitter::IsFinished() const
{
	return m_isFirstParticleSpawned && m_particles.empty();
}

void ParticleEmitter::Update(float deltaSeconds)
{
	//Burst Spawning
	if (!m_isFirstParticleSpawned)
	{
		m_particles.reserve(m_definition->m_burst);
		for (int count = 0; count < m_definition->m_burst; count++)
		{
			m_isFirstParticleSpawned = true;
			m_particles.push_back(new Particle(m_definition, this));
		}
	}
	float spawn = (float)m_definition->m_spawn;
	m_lifeSpam += deltaSeconds;

	if (m_lifeSpam < m_definition->m_spawnTime || m_definition->m_spawnTime < 0)
	{
		m_spawnCountThisFrame += spawn * deltaSeconds;
		if (m_spawnCountThisFrame >= 1.f)
		{
			int numOfParticle = (int)m_spawnCountThisFrame;
			if (m_definition->m_particleType == ParticleEmitterDefinition::RIBBON)
				numOfParticle = 1;
			for (int count = 0; count < numOfParticle; count++)
			{
				m_isFirstParticleSpawned = true;
				m_particles.push_back(new Particle(m_definition, this));
			}
			m_spawnCountThisFrame = fmod(m_spawnCountThisFrame, 1.f);
		}
	}
	

	for (auto particle : m_particles)
		particle->UpdateStatus(deltaSeconds);
	for(uint index = 0; index < m_particles.size(); index++)
		if (m_particles[index]->m_lifeSpam < 0.f)
		{
			//Particle* particle = m_particles[index];
			//delete particle;
			delete m_particles[index];
			m_particles.erase(m_particles.begin() + index);
			index--;
			//Console::Log("ParticleDeleted, " + std::to_string(m_particles.size()) + " remains.");
		}
}

void ParticleEmitter::SortByDistance(const Vector3& centerPosition)
{
	for(int i = 0; i < (int)(m_particles.size() - 1); i++)
	{
		uint j = i + 1;
			
		Particle* a = m_particles[i];
		Particle* b = m_particles[j];
		float distanceSquareA = (a->m_status.m_position - centerPosition).CalcLengthSquared();
		float distanceSquareB = (b->m_status.m_position - centerPosition).CalcLengthSquared();
		if (distanceSquareB > distanceSquareA)
		{
			m_particles[i] = b;
			m_particles[j] = a;
			i -= 2;
			if (i < 0)
				i = 0;
		}
	}
}

bool ParticleEmitter::GetVertices(std::vector<Vertex3>& vertices, const Vector3& cameraPosition) const
{
	switch (m_definition->m_particleType)
	{
	case ParticleEmitterDefinition::RIBBON:
		return GetRibbonVertices(vertices, cameraPosition);
	default:
		return GetDefaultVertices(vertices, cameraPosition);
	}
}

bool ParticleEmitter::GetRibbonVertices(std::vector<Vertex3>& vertices, const Vector3& cameraPosition) const
{
	if (m_particles.empty())
		return false;
	vertices.reserve((uint)(m_particles.size() * 12));

	Vertex3 prevTop;
	Vertex3 prevBottom;
	Vector3 prevPos;

	for (size_t particleIndex = 0; particleIndex < m_particles.size(); particleIndex++)
	{

		if (particleIndex == 0)
		{
			Particle* prev = m_particles[0];
			prevPos = prev->m_status.m_position;
			Rgba prevColor = m_definition->GetColorByTime(m_definition->m_lifeTime);
			prevTop = Vertex3(prevPos, prevColor);
			prevBottom = Vertex3(prevPos, prevColor);
			continue;
		}

		Particle* current = m_particles[particleIndex];
		Vector3 currentPos = current->m_status.m_position;

		/*Vector3 horizontal = currentPos - prevPos;
		Vector3 toCamera = Vector3::Y_AXIS; // cameraPosition - currentPos;
		Vector3 vertical = CrossProduct(horizontal, toCamera);
		float currentSize = Interpolate(m_definition->m_endSize, m_definition->m_startSize, current->m_lifeSpam / m_definition->m_lifeTime);
		Rgba currentColor = Interpolate(m_definition->m_endColor, m_definition->m_startColor, current->m_lifeSpam / m_definition->m_lifeTime);
		
		Vertex3 currentTop(currentPos/ * + horizontal*currentSize*0.5f* / + vertical*currentSize*0.5f, currentColor);
		Vertex3 currentBottom(currentPos/ * + horizontal*currentSize*0.5f* / - vertical*currentSize*0.5f, currentColor);*/

		//create basis
		Vector3 toCamera = cameraPosition - currentPos;
		Vector3 toPrev = prevPos - currentPos;
		Vector3 planeVertical = CrossProduct(toCamera, toPrev);
		planeVertical.Normalize();
		
		float currentSize = m_definition->GetSizeByTime(current->m_lifeSpam);//Interpolate(m_definition->m_endSize, m_definition->m_startSize, current->m_lifeSpam / m_definition->m_lifeTime);
		Rgba currentColor = m_definition->GetColorByTime(current->m_lifeSpam);//Interpolate(m_definition->m_endColor, m_definition->m_startColor, current->m_lifeSpam / m_definition->m_lifeTime);

		Vertex3 currentTop(currentPos + planeVertical*currentSize*0.5f, currentColor, Vector2(1.f, 0.f));
		Vertex3 currentBottom(currentPos - planeVertical*currentSize*0.5f, currentColor, Vector2(1.f, 1.f));
		
		//triangulate
		vertices.emplace_back(prevBottom);
		vertices.emplace_back(currentBottom);
		vertices.emplace_back(currentTop);
		vertices.emplace_back(prevBottom);
		vertices.emplace_back(currentTop);
		vertices.emplace_back(prevTop);

		vertices.emplace_back(prevBottom);
		vertices.emplace_back(currentTop);
		vertices.emplace_back(currentBottom);
		vertices.emplace_back(prevBottom);
		vertices.emplace_back(prevTop);
		vertices.emplace_back(currentTop);

		currentTop.texCoords = Vector2(0.f, 0.f);
		currentBottom.texCoords = Vector2(0.f, 1.f);

		prevPos = currentPos;
		prevTop = currentTop;
		prevBottom = currentBottom;
	}
	return true;
}

bool ParticleEmitter::GetDefaultVertices(std::vector<Vertex3>& vertices, const Vector3& cameraPosition) const
{
	if (m_particles.empty())
		return false;
	vertices.reserve((uint)(m_particles.size() * 6));
	for (auto particle : m_particles)
	{
		float size = m_definition->GetSizeByTime(particle->m_lifeSpam);//Interpolate(m_definition->m_endSize, m_definition->m_startSize, particle->m_lifeSpam / m_definition->m_lifeTime);
		Rgba color = m_definition->GetColorByTime(particle->m_lifeSpam);//Interpolate(m_definition->m_endColor, m_definition->m_startColor, particle->m_lifeSpam / m_definition->m_lifeTime);
		Vector3 particlePos(particle->m_status.m_position);
		//particlePos = m_transform.TransformPosition(particlePos);
		Vector3 cameraToParticle(particle->m_status.m_position - cameraPosition);
		cameraToParticle.Normalize();
		Vector3 right;
		if (right != Vector3::Y_AXIS)
			right = CrossProduct(Vector3::Y_AXIS, cameraToParticle);
		else
			right = Vector3::X_AXIS;
		Vector3 up = CrossProduct(cameraToParticle, right);

		Vector3 leftTopPos(particlePos + up*size*0.5f - right*size*0.5f);
		Vector3 leftBottomPos(particlePos - up*size*0.5f - right*size*0.5f);
		Vector3 rightTopPos(particlePos + up*size*0.5f + right*size*0.5f);
		Vector3 rightBottomPos(particlePos - up*size*0.5f + right*size*0.5f);

		Vertex3 leftTop(leftTopPos, color, Vector2(0.f, 0.f), -cameraToParticle, right);
		Vertex3 leftBottom(leftBottomPos, color, Vector2(0.f, 1.f), -cameraToParticle, right);
		Vertex3 rightTop(rightTopPos, color, Vector2(1.f, 0.f), -cameraToParticle, right);
		Vertex3 rightBottom(rightBottomPos, color, Vector2(1.f, 1.f), -cameraToParticle, right);

		vertices.emplace_back(leftBottom);
		vertices.emplace_back(rightTop);
		vertices.emplace_back(leftTop);

		vertices.emplace_back(leftBottom);
		vertices.emplace_back(rightBottom);
		vertices.emplace_back(rightTop);
	}
	return true;
}
