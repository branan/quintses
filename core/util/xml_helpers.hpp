#ifndef QNT_CORE_UTIL_XML_HELPERS_HPP
#define QNT_CORE_UTIL_XML_HELPERS_HPP

#ifndef QNT_CORE_UTIL_XML_HPP
#error xml_helpers.hpp requires xml.hpp
#endif

#ifdef QNT_CORE_UTIL_MATH_HPP
// functions that require math to be used correctly

glm::mat4 parseTransform(rapidxml::xml_node<> *node) {
  glm::mat4 trans;
  if(!node)
    return trans;
  rapidxml::xml_node<> *pos = node->first_node("Position");
  rapidxml::xml_node<> *quat = node->first_node("Quaternion");
  if(pos) {
    float x = atof(pos->first_node("X")->value());
    float y = atof(pos->first_node("Y")->value());
    float z = atof(pos->first_node("Z")->value());
    trans = glm::translate(trans, glm::vec3(x, y, z));
  }
  if(quat) {
    float x = atof(quat->first_node("X")->value());
    float y = atof(quat->first_node("Y")->value());
    float z = atof(quat->first_node("Z")->value());
    float w = atof(quat->first_node("W")->value());
    glm::quat quat(x, y, z, w);
    trans = trans * glm::mat4_cast(quat);
  }
  return trans;
}

glm::quat parseQuaternion(rapidxml::xml_node<> *quat) {
  if(!quat)
    return glm::quat();
  float x = atof(quat->first_node("X")->value());
  float y = atof(quat->first_node("Y")->value());
  float z = atof(quat->first_node("Z")->value());
  float w = atof(quat->first_node("W")->value());
  return glm::quat(x, y, z, w);
}
#endif // QNT_CORE_UTIL_MATH_HPP

#endif // QNT_CORE_UTIL_XML_HELPERS_HPP
