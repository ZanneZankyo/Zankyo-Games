#pragma once
#include <string>
#include "ThirdParty\xml\XMLParser.hpp"
#include <map>
#include <vector>

typedef std::map<std::string, std::string> XMLAttributes;

class XMLElement
{
public:
	XMLNode m_element;

public:

	XMLElement(XMLNode node);
	XMLElement(const std::string& filePath);

	static XMLElement OpenFile(const std::string& filePath);

	bool IsEmpty() const;

	int GetChildNodeSize() const;
	XMLElement GetChildNode(const std::string& nodeName, int nodeIndex = 0) const;
	XMLElement GetChildNode(int nodeIndex = 0) const;
	std::vector<XMLElement> GetChildNodes(const std::string& nodeName = "") const;
	std::string GetNodeName() const;
	std::string GetText() const;
	bool HasAttribute(const std::string& attributeName) const;

	int GetAttributeSize() const;
	XMLAttributes GetAttributes() const;
	std::string GetAttributeName(int attrIndex) const;
	std::string GetAttributeValue(int attrIndex) const;
	std::string GetAttribute(const std::string& attrName) const;

	XMLElement operator = (const XMLElement& copy);
};

class XMLUtils
{
public:
	template< typename ValueType >
	static ValueType ParseXMLAttribute(const XMLElement& element, const std::string& attrName, const ValueType& defaultValue);
	template< typename ValueType >
	static bool ParseXMLAttributeMinMax(const XMLElement& element, const std::string& attrName, const char splitToken, ValueType& out_min, ValueType& out_max);
	bool ValidateXmlElement(const XMLElement& element,		// ASSERTS that element has no
		const std::string& commaSeparatedListOfValidChildElementNames,	// child elements or attributes with 
		const std::string& commaSeparatedListOfValidAttributeNames);

};
