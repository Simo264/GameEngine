#include "Filesystem.hpp"

namespace Filesystem
{
	const fs::path& GetRootPath()
	{
		static const fs::path ROOT_PATH = fs::current_path().parent_path().lexically_normal();
		return ROOT_PATH;
	}

	const fs::path& GetShadersPath()
	{
		static const fs::path SHADERS_PATH = (GetRootPath() / "Shaders").lexically_normal();
		return SHADERS_PATH;
	}

	const fs::path& GetAssetsPath()
	{
		static const fs::path ASSETS_PATH = (GetRootPath() / "Assets").lexically_normal();
		return ASSETS_PATH;
	}

	const fs::path& GetTexturesPath()
	{
		static const fs::path TEXTURES_PATH = (GetAssetsPath() / "Textures").lexically_normal();
		return TEXTURES_PATH;
	}

	const fs::path& GetIconsPath()
	{
		static const fs::path ICONS_PATH = (GetAssetsPath() / "Icons").lexically_normal();
		return ICONS_PATH;
	}

	const fs::path& GetFontsPath()
	{
		static const fs::path FONTS_PATH = (GetAssetsPath() / "Fonts").lexically_normal();
		return FONTS_PATH;
	}

	const fs::path& GetModelsPath()
	{
		static const fs::path MODELS_PATH = (GetAssetsPath() / "Models").lexically_normal();
		return MODELS_PATH;
	}

	const fs::path& GetStaticModelsPath()
	{
		static const fs::path STATIC_MODELS_PATH = (GetModelsPath() / "Static").lexically_normal();
		return STATIC_MODELS_PATH;
	}

	const fs::path& GetSkeletalModelsPath()
	{
		static const fs::path SKELETAL_MODELS_PATH = (GetModelsPath() / "Skeletal").lexically_normal();
		return SKELETAL_MODELS_PATH;
	}
}