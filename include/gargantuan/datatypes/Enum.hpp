#pragma once

#include "gargantuan/scripting/Userdata.hpp"

#include <magic_enum/magic_enum.hpp>
#include <string_view>
#include <vector>

namespace gargantuan {

struct Enum : Userdata<Enum> {
    struct EnumItem : Userdata<EnumItem> {
        std::string_view Name;
        int Value;
        const Enum *EnumType;
    };

    std::string_view Name;
    std::vector<EnumItem> Items;

    template <typename EnumType> static typename std::enable_if_t<std::is_enum_v<EnumType>, Enum> fromType() {
        Enum result;
        result.Name = magic_enum::enum_type_name<EnumType>();

        constexpr auto entries = magic_enum::enum_entries<EnumType>();
        result.Items.reserve(entries.size());

        for (const auto &[value, name] : entries) {
            result.Items.push_back({
                name,
                static_cast<int>(value),
                result,
            });
        }

        return result;
    }
};

USERDATA_STACKVALUE(Enum);
USERDATA_STACKVALUE(Enum::EnumItem);

} // namespace gargantuan
