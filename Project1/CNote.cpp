#include "pch.h"
#include "CNote.h"

CNote::CNote() {}
CNote::~CNote() {}

void CNote::XmlLoad(IXMLDOMNode* xml, std::wstring& instrument)
{
    m_node = xml;
    m_instrument = instrument;

    CComPtr<IXMLDOMNamedNodeMap> attributes;
    xml->get_attributes(&attributes);
    long len;
    attributes->get_length(&len);

    for (int i = 0; i < len; i++)
    {
        CComPtr<IXMLDOMNode> attrib;
        attributes->get_item(i, &attrib);

        CComBSTR name;
        attrib->get_nodeName(&name);

        CComVariant value;
        attrib->get_nodeValue(&value);

        if (name == L"measure")
        {
            value.ChangeType(VT_I4);
            m_measure = value.intVal - 1;
        }
        else if (name == L"beat")
        {
            value.ChangeType(VT_R8);
            m_beat = value.dblVal - 1;
        }
    }
}

bool CNote::operator<(const CNote& b)
{
    if (m_measure < b.m_measure)
        return true;
    if (m_measure > b.m_measure)
        return false;
    if (m_beat < b.m_beat)
        return true;

    return false;
}
