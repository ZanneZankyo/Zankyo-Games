#include "Material.hpp"
#include "Engine\Tools\XMLUtils.hpp"
#include "Engine\File\FileSystem.hpp"

std::map<std::string, Material*> Material::s_material;

void Material::LoadAllMaterial(const std::string & materialFolderPath)
{
	std::vector<std::string> filePaths = FileSystem::GetAllFileNamesInPath(materialFolderPath + "*.xml");
	for (std::string& filePath : filePaths)
		CreateOrGetMaterial(materialFolderPath + filePath);
}

Material* Material::CreateOrGetMaterial(const std::string& materialFilePath)
{
	auto found = s_material.find(materialFilePath);
	if (found != s_material.end())
		return found->second;
	return CreateMaterial(materialFilePath);
}

Material* Material::CreateMaterial(const std::string& materialFilePath)
{
	XMLElement element = XMLElement::OpenFile(materialFilePath);
	if (element.IsEmpty())
		return nullptr;
	std::string name = element.GetAttribute("name");
	std::string shaderName = element.GetChildNode("Shader").GetAttribute("name");
	std::string diffuseTextureFilePath = element.GetChildNode("DiffuseTexture").GetAttribute("image");
	std::string normalTextureFilePath = element.GetChildNode("NormalTexture").GetAttribute("image");
	std::string specularTextureFilePath = element.GetChildNode("SpecularTexture").GetAttribute("image");
	XMLElement blendNode = element.GetChildNode("BlendMode");
	BlendFactor srcBlend = BlendState::ToFactor(blendNode.GetAttribute("src"));
	BlendFactor destBlend = BlendState::ToFactor(blendNode.GetAttribute("dest"));

	Material* newMaterial = new Material(name, shaderName, diffuseTextureFilePath, normalTextureFilePath, specularTextureFilePath, srcBlend, destBlend);

	s_material[materialFilePath] = newMaterial;

	return newMaterial;
}

Material* Material::GetMaterial(const std::string& materialFilePath)
{
	auto found = s_material.find(materialFilePath);
	if (found != s_material.end())
		return found->second;
	return nullptr;
}