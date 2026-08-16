#pragma once

#include "gargantuan/reflection/Enums.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include <string>

namespace gargantuan {
	G_ENUM(
		FontWeight,
		Thin = 100,
		ExtraLight = 200,
		Light = 300,
		Regular = 400,
		Medium = 500,
		SemiBold = 600,
		Bold = 700,
		ExtraBold = 800,
		Heavy = 900
	);

	G_ENUM(FontStyle, Normal, Italic);

	typedef std::string Content;

	G_USERDATA_DECL(
		Font,

		Font(
			Content fontContent,
			Enums::FontWeight weight = Enums::FontWeight::Regular,
			Enums::FontStyle style = Enums::FontStyle::Normal
		);
		static Font fromName(std::string name);

		Content Family = "";
		Enums::FontWeight Weight;
		Enums::FontStyle Style;
	);
}
