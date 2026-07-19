#include "gargantuan/instances/ClassDefinition.hpp"

namespace gargantuan::instances {

// template <typename ClassType, typename MemberType>
// PropertyDefinition
// PropertyDefinition::fromSimple(std::string_view name, scripting::AnyScriptType type, MemberType ClassType::*member) {
//     return PropertyDefinition{
//         .Name = name,
//         .Type = type,
//         .Get =
//             [member](const Instance *instance) {
//                 const auto *concrete = static_cast<const ClassType *>(instance);
//                 return std::any(concrete->*member);
//             },
//         .Set =
//             [member](const Instance *instance, const std::any &value) {
//                 auto *concrete = static_cast<ClassType *>(instance);
//                 concrete->*member = std::any_cast<MemberType>(value);
//             },
//     };
// }

} // namespace gargantuan::instances
