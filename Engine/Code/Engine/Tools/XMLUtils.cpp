#include "XMLUtils.hpp"
#include "..\Core\RGBA.hpp"
#include "..\Core\StringUtils.hpp"
#include "..\Math\Vector2.hpp"
#include <vector>

XMLElement::XMLElement(XMLNode node)
{
	m_element = node;
}

XMLElement::XMLElement(const std::string& filePath)
{
	*this = OpenFile(filePath);
}

int XMLElement::GetAttributeSize() const
{
	return m_element.nAttribute();
}

bool XMLElement::IsEmpty() const
{
	return m_element.isEmpty() == 0 ? false : true;
}

XMLElement XMLElement::OpenFile(const std::string & filePath)
{
	return XMLElement(XMLNode::openFileHelper(filePath.c_str()));
}

XMLElement XMLElement::GetChildNode(const std::string& nodeName, int nodeIndex /*= 0*/) const
{
	return XMLElement(m_element.getChildNode(nodeName.c_str(), nodeIndex));
}

XMLElement XMLElement::GetChildNode(int nodeIndex) const
{
	return m_element.getChildNode(nodeIndex);
}

std::vector<XMLElement> XMLElement::GetChildNodes(const std::string& nodeName /*= ""*/) const
{
	std::vector<XMLElement> childNodes;
	if (nodeName.empty())
	{
		int childNodeSize = GetChildNodeSize();
		for (int childIndex = 0; childIndex < childNodeSize; childIndex++)
			childNodes.push_back(GetChildNode(childIndex));
		return childNodes;
	}
	else
	{
		int childIndex = 0;
		XMLElement childNode = GetChildNode(nodeName, childIndex);
		while (!childNode.IsEmpty())
		{
			childNodes.push_back(childNode);
			childIndex++;
			childNode = GetChildNode(nodeName, childIndex);
		}
		return childNodes;
	}
}

std::string XMLElement::GetNodeName() const
{
	return m_element.getName();
}

std::string XMLElement::GetText() const
{
	const char* text = m_element.getText();
	if(text)
		return text;
	return "";
}

bool XMLElement::HasAttribute(const std::string & attributeName) const
{
	return m_element.isAttributeSet(attributeName.c_str()) == 0 ? false : true;
}

int XMLElement::GetChildNodeSize() const
{
	return m_element.nChildNode();
}

XMLAttributes XMLElement::GetAttributes() const
{
	std::map<std::string, std::string> attrs;

	int numOfAttrs = GetAttributeSize();
	for (int attrIndex = 0; attrIndex < numOfAttrs; attrIndex++)
		attrs[GetAttributeName(attrIndex)] = GetAttributeValue(attrIndex);

	return attrs;
}

std::string XMLElement::GetAttributeName(int attrIndex) const
{
	return m_element.getAttributeName(attrIndex);
}

std::string XMLElement::GetAttributeValue(int attrIndex) const
{
	return m_element.getAttributeValue(attrIndex);
}

std::string XMLElement::GetAttribute(const std::string & attrName) const
{
	const char* value = m_element.getAttribute(attrName.c_str());
	if (value == NULL)
		return "";
	else
		return std::string(value);
}

XMLElement XMLElement::operator=(const XMLElement & copy)
{
	m_element = copy.m_element;
	return *this;
}

template<>
std::string XMLUtils::ParseXMLAttribute(
	const XMLElement& element, const std::string& attrName, const std::string& defaultValue)
{
	if(element.HasAttribute(attrName))
		return element.GetAttribute(attrName);
	return defaultValue;
}

template<>
int XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const int& defaultValue)
{
	if (element.HasAttribute(attrName))
		return stoi(element.GetAttribute(attrName));
	return defaultValue;
}

template<>
float XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const float& defaultValue)
{
	if (element.HasAttribute(attrName))
		return stof(element.GetAttribute(attrName));
	return defaultValue;
}

template<>
Rgba XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const Rgba& defaultValue)
{
	if (element.HasAttribute(attrName))
		return Rgba(element.GetAttribute(attrName));
	return defaultValue;
}

template<>
char XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const char& defaultValue)
{
	if (element.HasAttribute(attrName))
	{
		std::string charStr = element.GetAttribute(attrName);
		if (charStr.empty())
			return defaultValue;
		return charStr[0];
	}
	return defaultValue;
}

template<>
bool XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const bool& defaultValue)
{
	if (element.HasAttribute(attrName))
		return StringUtils::ToBool(element.GetAttribute(attrName));
	return defaultValue;
}

template<>
IntVector2 XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const IntVector2& defaultValue)
{
	if (!element.HasAttribute(attrName))
		return defaultValue;
	std::string vectorString = element.GetAttribute(attrName);
	std::vector <std::string> String2D = StringUtils::Split(vectorString, ',');
	if (String2D.size() < 2)
		return defaultValue;
	int x = stoi(String2D[0]);
	int y = stoi(String2D[1]);
	return IntVector2(x, y);
}

template<>
Vector2 XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const Vector2& defaultValue)
{
	if (!element.HasAttribute(attrName))
		return defaultValue;
	std::string vectorString = element.GetAttribute(attrName);
	std::vector <std::string> String2D = StringUtils::Split(vectorString, ',');
	if (String2D.size() < 2)
		return defaultValue;
	float x = stof(String2D[0]);
	float y = stof(String2D[1]);
	return Vector2(x, y);
}

template< typename ValueType >
ValueType XMLUtils::ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const ValueType& defaultValue)
{
	ASSERT_OR_DIE(false, "Wrong data type when reading XML node!");
}

template<>
bool XMLUtils::ParseXMLAttributeMinMax(
	const XMLElement& element, const std::string& attrName, const char splitToken, 
	int& out_min, int& out_max)
{
	if (!element.HasAttribute(attrName))
		return false;

	std::vector<std::string> roomCountStrings =
		StringUtils::Split(element.GetAttribute(attrName), splitToken);
	if (roomCountStrings.size() <= 1)
		roomCountStrings.push_back(roomCountStrings[0]);
	out_min = stoi(roomCountStrings[0]);
	out_max = stoi(roomCountStrings[1]);

	return true;
}

template< typename ValueType >
bool XMLUtils::ParseXMLAttributeMinMax(
	const XMLElement& element, const std::string& attrName, const char splitToken, 
	ValueType& out_min, ValueType& out_max)
{
	ASSERT_OR_DIE(false, "Wrong data type when reading XML node!");
}

bool XMLUtils::ValidateXmlElement(
	const XMLElement & element, 
	const std::string & commaSeparatedListOfValidChildElementNames, 
	const std::string & commaSeparatedListOfValidAttributeNames)
{
	int numOfChildNode = element.GetChildNodeSize();
	std::vector<std::string> validChildElementNames = StringUtils::Split(commaSeparatedListOfValidChildElementNames, ',');
	std::vector<std::string> validChildAttributeNames = StringUtils::Split(commaSeparatedListOfValidAttributeNames, ',');
	for (int nodeIndex = 0; nodeIndex < numOfChildNode; nodeIndex++)
	{
		XMLElement childNode = element.GetChildNode(nodeIndex);
		if (find(validChildElementNames.begin(), validChildElementNames.end(), childNode.GetNodeName()) == validChildElementNames.end())
			return false;

		XMLAttributes nodeAttrs = element.GetAttributes();
		for (auto& validElementName : validChildAttributeNames)
			if (nodeAttrs.find(validElementName) == nodeAttrs.end())
				return false;

	}
	return true;
}
