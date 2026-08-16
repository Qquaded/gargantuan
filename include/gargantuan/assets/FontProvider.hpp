#pragma once

#include "gargantuan/filesystem/BaseFilesystem.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>

#include <exception>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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

	struct FontVariant {
		SDL_GPUTexture *AtlasTexture = nullptr;
		std::unordered_map<char, GlyphInfo> Glyphs;
		float LineHeight = 0.0f;
	};

	struct FontManifestVariant {
		std::optional<std::string> Normal;
		std::optional<std::string> Italic;
	};

	struct FontManifest {
		std::string Name;
		std::string Family;
		std::unordered_map<std::string, FontManifestVariant> Variants;
	};

	struct Glyph {
		glm::vec2 UVMin;
		glm::vec2 UVMax;
		glm::vec2 Size;
		glm::vec2 Bearing;
		float Advance;
	};

	class FontAtlas {
	  public:
		SDL_GPUTexture *Texture = nullptr;
		TTF_Font *Font = nullptr;

		int Width = 2048, Height = 2048;
		int CurrentX = 1, CurrentY = 1;
		int RowHeight = 0.0f;

		float LineHeight = 0.0f;
		float PointSize = 0.0f;

		std::unordered_map<char32_t, Glyph> Glyphs;
	};

	class FontProvider {
	  public:
		FontProvider(SDL_GPUDevice *gpu, BaseFilesystem *filesystem);
		~FontProvider();

		void RegisterManifest(const std::filesystem::path &path);

		const FontAtlas *GetAtlas(
			const std::string &name, const Enums::FontWeight &weight, const Enums::FontStyle &style, float pointSize
		);

	  private:
		SDL_GPUDevice *Gpu;
		BaseFilesystem *Filesystem;

		using SourceKey = std::tuple<std::string, Enums::FontWeight, Enums::FontStyle>;
		std::map<SourceKey, std::filesystem::path> SourcePaths;

		using AtlasKey = std::tuple<std::string, Enums::FontWeight, Enums::FontStyle, uint32_t>;
		std::map<AtlasKey, FontAtlas> Atlases;

		void RegisterSource(
			std::string name,
			const Enums::FontWeight &weight,
			const Enums::FontStyle &style,
			const std::filesystem::path &manifestPath,
			const std::filesystem::path &variantPath
		);

		[[nodiscard]] FontAtlas CreateFontAtlas(const std::filesystem::path &source, float pointSize);
	};
}
