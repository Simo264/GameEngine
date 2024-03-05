#include "SceneSerializer.hpp"

#include "Engine/Scene.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/GameObjectType.hpp"
#include "Engine/ECS/LabelComponent.hpp"
#include "Engine/ECS/TypeComponent.hpp"
#include "Engine/ECS/TransformComponent.hpp"
#include "Engine/ECS/StaticMeshComponent.hpp"
#include "Engine/ECS/Lighting/DirLightComponent.hpp"
#include "Engine/ECS/Lighting/PointLightComponent.hpp"
#include "Engine/ECS/Lighting/SpotLightComponent.hpp"

#include "Core/FileParser/INIFileParser.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeScene(Scene& scene, const Path& filepath)
{
	INIFileParser conf(filepath);

	for (auto [entity, labelComp, typeComp] : scene.Reg().view<LabelComponent, TypeComponent>().each())
	{
		char section[32]{};
		sprintf_s(section, "Entity_%d", static_cast<uint32_t>(entity));

		/* object label */
		conf.Update(section, "label", labelComp.label.c_str());

		/* object type */
		char buff[32]{};
		sprintf_s(buff, "%d", typeComp.type);
		conf.Update(section, "type", buff);

		GameObject obj{ entity, &scene.Reg() };
		if (auto dLightComp = obj.GetComponent<DirLightComponent>())
		{
			String strDirLight; 
			dLightComp->ToString(strDirLight);

			SerializeComponent(conf, section, strDirLight);
		}
		else if (auto pLightComp = obj.GetComponent<PointLightComponent>())
		{
			String strPointLight;
			pLightComp->ToString(strPointLight);

			SerializeComponent(conf, section, strPointLight);
		}
		if (auto transComp = obj.GetComponent<TransformComponent>())
		{
			String strTrans;
			transComp->ToString(strTrans);

			SerializeComponent(conf, section, strTrans);
		}
		if (auto meshComp = obj.GetComponent<StaticMeshComponent>())
		{
			String strMesh;
			meshComp->ToString(strMesh);

			SerializeComponent(conf, section, strMesh);
		}
	}

	conf.Write();
}

void SceneSerializer::DeserializeScene(Scene& scene, const Path& filepath)
{

}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeComponent(INIFileParser& conf, const char* section, String& strComponent)
{
	/* Read line by line from string in C-Style to avoid unnecessary memory allocations */
	char* currentLine = strComponent.data();
	while (currentLine && std::strlen(currentLine) > 0)
	{
		char* nextLine = strchr(currentLine, '\n');
		if (nextLine)
			*nextLine = '\0';  /* temporarily terminate the current line */ 

		char* value = std::strchr(currentLine, '=') + 1;
		int sep = (value - 1) - currentLine;
		char name[32]{};
		std::copy(currentLine, currentLine + sep, name);
		conf.Update(section, name, value);

		if (nextLine)
			*nextLine = '\n';  /* then restore newline - char, just to be tidy*/ 

		currentLine = nextLine ? (nextLine + 1) : nullptr;
	}
}
