#include "Core.hpp"

const fs::path& GetRootPath()
{
	static const fs::path ROOT_PATH = fs::current_path().parent_path().lexically_normal();
	return ROOT_PATH;
}

const fs::path& GetResourcePath()
{
	static const fs::path RESOURCE_PATH = (GetRootPath() / "Resources").lexically_normal();
	return RESOURCE_PATH;
}

const fs::path& GetShadersPath()
{
	static const fs::path SHADERS_PATH = (GetResourcePath() / "Shaders").lexically_normal();
	return SHADERS_PATH;
}

const fs::path& GetTexturesPath()
{
	static const fs::path TEXTURES_PATH = (GetResourcePath() / "Textures").lexically_normal();
	return TEXTURES_PATH;
}

const fs::path& GetIconsPath()
{
	static const fs::path ICONS_PATH = (GetResourcePath() / "Icons").lexically_normal();
	return ICONS_PATH;
}

const fs::path& GetFontsPath()
{
	static const fs::path FONTS_PATH = (GetResourcePath() / "Fonts").lexically_normal();
	return FONTS_PATH;
}

const fs::path& GetAssetsPath()
{
	static const fs::path ASSETS_PATH = (GetResourcePath() / "Assets").lexically_normal();
	return ASSETS_PATH;
}