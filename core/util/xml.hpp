#ifndef QNT_CORE_UTIL_XML_HPP
#define QNT_CORE_UTIL_XML_HPP

// TODO: adjust RAPIDXML_STATIC_POOL_SIZE and RAPIDXML_DYNAMIC_POOL_SIZE to meet QNT requirements
#include "3rdparty/rapidxml/rapidxml.hpp"
#include "atomic.hpp"

#include <string>
#include <string.h>

class XmlDom {
public:
  XmlDom() : m_count(0), m_document(0) {}
  XmlDom(std::string text) {
    m_count = new uint32_t;
    *m_count = 1;
    m_document = new rapidxml::xml_document<>;
    // this string will be released when the document is destroyed.
    char *string = m_document->allocate_string(text.c_str());
    m_document->parse<0>(string);
  }
  ~XmlDom() {
    if(m_count) {
      fetchAndDecrementP(m_count);
      if((*m_count) == 0) {
        delete m_count;
        delete m_document;
      }
    }
  }
  XmlDom(const XmlDom& rhs) : m_count(rhs.m_count), m_document(rhs.m_document) {
    if(m_count)
      fetchAndIncrementP(m_count);
  }
  XmlDom& operator=(const XmlDom& rhs) {
    if(m_document) {
      fetchAndDecrementP(m_count);
      if((*m_count) == 0) {
        delete m_count;
        delete m_document;
      }
    }
    m_count = rhs.m_count;
    m_document = rhs.m_document;
    if(m_count)
      fetchAndIncrementP(m_count);
    return *this;
  }
  rapidxml::xml_document<> *doc() { return m_document; }
private:
  uint32_t *m_count;
  rapidxml::xml_document<> *m_document;
};

#endif // QNT_CORE_UTIL_XML_HPP
