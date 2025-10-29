#pragma once
#include "msxml2.h"
#include <string>
#include <atlbase.h>

class CNote
{
public:
    CNote();
    virtual ~CNote();

    int Measure() const { return m_measure; }
    double Beat() const { return m_beat; }
    const std::wstring& Instrument() const { return m_instrument; }
    IXMLDOMNode* Node() { return m_node; }
    bool operator<(const CNote& b);
    void XmlLoad(IXMLDOMNode* xml, std::wstring& instrument);

private:
    std::wstring m_instrument;
    int m_measure;
    double m_beat;
    CComPtr<IXMLDOMNode> m_node;
};

