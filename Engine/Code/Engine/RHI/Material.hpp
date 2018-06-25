#pragma once
#include <string>
#include <map>
#include "BlendState.hpp"
#include "RasterState.hpp"

class Material
{
public:
	std::string m_name;
	std::string m_shaderName;
	std::string m_diffuseTextureFilePath;
	std::string m_normalTextureFilePath;
	std::string m_specularTextureFilePath;
	BlendFactor m_srcBlend;
	BlendFactor m_destBlend;
	CullMode	m_cullMode;
	FillMode	m_fillMode;

	Material(
		const std::string& name = "",
		const std::string& shaderName = "",
		const std::string& diffuse = "",
		const std::string& normal = "",
		const std::string& spec = "",
		BlendFactor srcBlend = BLEND_SRC_ALPHA,
		BlendFactor destBlend = BLEND_INV_SRC_ALPHA,
		CullMode cullMode = CULLMODE_BACK,
		FillMode fillMode = FILLMODE_SOLID
	);
	Material(const std::string& materialFilePath);

public:
	static void LoadAllMaterial(const std::string& materialFolderPath = "Data/Material/");
	static Material* CreateOrGetMaterial(const std::string& materialFilePath);
	static Material* CreateMaterial(const std::string& materialFilePath);
	static Material* GetMaterial(const std::string& materialFilePath);

	static std::map<std::string, Material*> s_material;
	static Material* s_defaultMaterial;
};

inline Material::Material
(
	const std::string& name /*= ""*/, 
	const std::string& shaderName /*= ""*/, 
	const std::string& diffuse /*= ""*/, 
	const std::string& normal /*= ""*/, 
	const std::string& spec /*= ""*/, 
	BlendFactor srcBlend /*= BLEND_SRC_ALPHA*/, 
	BlendFactor destBlend /*= BLEND_INV_SRC_ALPHA */,
	CullMode cullMode /*= CULLMODE_BACK*/,
	FillMode fillMode /*= FILLMODE_SOLID*/
)
	: m_name(name)
	, m_shaderName(shaderName)
	, m_diffuseTextureFilePath(diffuse)
	, m_normalTextureFilePath(normal)
	, m_specularTextureFilePath(spec)
	, m_srcBlend(srcBlend)
	, m_destBlend(destBlend)
	, m_cullMode(cullMode)
	, m_fillMode(fillMode)
{
	s_material[m_name] = this;
}