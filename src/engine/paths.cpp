#include "Paths.hpp"

const fs::path& GetRootPath()
{
	static auto lambda = []() {
		auto currentPath = fs::current_path();
		if (currentPath.filename() == "Build")
			return currentPath.parent_path().lexically_normal();

		return currentPath.lexically_normal();
	};

	static const auto ROOT_PATH = lambda();
	return ROOT_PATH;
}
const fs::path& GetShadersPath()
{
	static const auto SHADERS_PATH = (GetRootPath() / "Shaders").lexically_normal();
	return SHADERS_PATH;
}
const fs::path& GetAssetsPath()
{
	static const fs::path ASSETS_PATH = (GetRootPath() / "Assets").lexically_normal();
	return ASSETS_PATH;
}
const fs::path& GetTexturesPath()
{
	static const auto TEXTURES_PATH = (GetAssetsPath() / "Textures").lexically_normal();
	return TEXTURES_PATH;
}
const fs::path& GetIconsPath()
{
	static const auto ICONS_PATH = (GetAssetsPath() / "Icons").lexically_normal();
	return ICONS_PATH;
}
const fs::path& GetFontsPath()
{
	static const auto FONTS_PATH = (GetAssetsPath() / "Fonts").lexically_normal();
	return FONTS_PATH;
}
const fs::path& GetModelsPath()
{
	static const auto MODELS_PATH = (GetAssetsPath() / "Models").lexically_normal();
	return MODELS_PATH;
}