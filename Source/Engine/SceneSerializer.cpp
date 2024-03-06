#include "SceneSerializer.hpp"

#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/GameObjectType.hpp"
#include "Engine/ECS/LabelComponent.hpp"
#include "Engine/ECS/TypeComponent.hpp"
#include "Engine/ECS/TransformComponent.hpp"
#include "Engine/ECS/StaticMeshComponent.hpp"
#include "Engine/ECS/Lighting/DirLightComponent.hpp"
#include "Engine/ECS/Lighting/PointLightComponent.hpp"
#include "Engine/ECS/Lighting/SpotLightComponent.hpp"

#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Core/FileParser/INIFileParser.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeScene(Scene& scene, const Path& filepath)
{
	INIFileParser conf(filepath);

	for (auto entity : scene.Reg().view<LabelComponent>())
	{
		GameObject obj{ entity, &scene.Reg() };
		SerializeObject(conf, obj);
	}
	conf.Write();
}

void SceneSerializer::DeserializeScene(Scene& scene, const Path& filepath)
{
	INIFileParser conf(filepath);
	conf.ReadData();

	String buff;
	buff.reserve(128);

	int nPointLights = 0; /* used for setting up shader uniforms */

	GameObject obj;
	for (auto const& it : conf.GetData())
	{
		const String& section = it.first;

		/* Parse object */
		if (section.find(':') == String::npos)
		{
			buff = conf.GetValue(section.c_str(), "label");
			uint32_t type	= std::stoi(conf.GetValue(section.c_str(), "type"));
			obj	= scene.CreateObject(buff.c_str(), type);
		}
		/* Parse object component */
		else
		{
			const char* componentName = (std::strchr(section.c_str(), ':') + 1);
			if (std::strcmp(componentName, DirLightComponent::GetComponentName(true)) == 0)
			{
				/* Parse DirLightComponent: 
					Vec3f color
					float ambient
					float diffuse
					float specular
					Vec3f direction
				*/
				auto& dlightComp = obj.AddComponent<DirLightComponent>(SHADER_UNIFORM_DIRLIGHT);
				buff = conf.GetValue(section.c_str(), "color");

				dlightComp.color = INIFileParser::StringToVec3f(buff);
				dlightComp.ambient = std::stof(conf.GetValue(section.c_str(), "ambient"));
				dlightComp.diffuse = std::stof(conf.GetValue(section.c_str(), "ambient"));
				dlightComp.specular = std::stof(conf.GetValue(section.c_str(), "ambient"));
				
				buff = conf.GetValue(section.c_str(), "direction");
				dlightComp.direction = INIFileParser::StringToVec3f(buff);
			}
			else if (std::strcmp(componentName, PointLightComponent::GetComponentName(true)) == 0)
			{
				/* Parse PointLightComponent: 
					Vec3f color
					float ambient
					float diffuse
					float specular
					Vec3f position 
				*/
				auto& plightComp = obj.AddComponent<PointLightComponent>(SHADER_UNIFORM_POINTLIGHT(nPointLights++));
				buff = conf.GetValue(section.c_str(), "color");
				plightComp.color = INIFileParser::StringToVec3f(buff);
				plightComp.ambient = std::stof(conf.GetValue(section.c_str(), "ambient"));
				plightComp.diffuse = std::stof(conf.GetValue(section.c_str(), "ambient"));
				plightComp.specular = std::stof(conf.GetValue(section.c_str(), "ambient"));
				buff = conf.GetValue(section.c_str(), "position");
				plightComp.position = INIFileParser::StringToVec3f(buff);
			}
			else if (std::strcmp(componentName, SpotLightComponent::GetComponentName(true)) == 0)
			{
				/* Parse SpotLightComponent: 
					Vec3f color
					float ambient
					float diffuse
					float specular
					Vec3f direction
					float cutoff
				*/

				/* TODO */
			}
			else if (std::strcmp(componentName, TransformComponent::GetComponentName(true)) == 0)
			{
				/* Parse TransformComponent:
					color
					Vec3f position;
					Vec3f scale;
					Vec3f rotation;	
				*/
				auto& transComp = obj.AddComponent<TransformComponent>();
				buff = conf.GetValue(section.c_str(), "position");
				transComp.position	= INIFileParser::StringToVec3f(buff);
				buff = conf.GetValue(section.c_str(), "scale");
				transComp.scale			= INIFileParser::StringToVec3f(buff);
				buff = conf.GetValue(section.c_str(), "rotation");
				transComp.rotation		= INIFileParser::StringToVec3f(buff);
			}
			else if (std::strcmp(componentName, StaticMeshComponent::GetComponentName(true)) == 0)
			{
				/* Parse StaticMeshComponent:
					model-path					(if exists)
					material-diffuse		(if exists)
					material-specular		(if exists)
				*/
				auto& smeshComp = obj.AddComponent<StaticMeshComponent>();
				buff = conf.GetValue(section.c_str(), "model-path");
				if (!buff.empty())
				{
					ObjectLoader loader(buff);
					loader.LoadMesh(&smeshComp);
				}

				buff = conf.GetValue(section.c_str(), "material-diffuse");
				if (!buff.empty())
					smeshComp.material.diffuse = TextureManager::Instance().GetTextureByPath(buff);

				buff = conf.GetValue(section.c_str(), "material-specular");
				if (!buff.empty())
					smeshComp.material.specular = TextureManager::Instance().GetTextureByPath(buff);
			}
		}
	}
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeObject(INIFileParser& conf, GameObject& object)
{
	/* Every object has both LabelComponent and TypeComponent: 
		[Entity_<id>]
		label=<label>
		type=<type>

		for each components:
		[Entity_<id>:<component-name>]
		<component-attributes>
		...

		[Entity_<id>:<component-name>]
		<component-attributes>
		...
	*/
	String buffer;				
	buffer.reserve(512);

	/* Set current section */
	char section[32]{};
	sprintf_s(section, "Entity_%d", static_cast<uint32_t>(object.GetObjectID()));

	/* Print label */
	auto labelComp = object.GetComponent<LabelComponent>();
	conf.Update(section, "label", labelComp->label.c_str());

	/* Print type */
	auto tyeComp = object.GetComponent<TypeComponent>();
	sprintf_s(buffer.data(), 32, "%d", tyeComp->type);
	conf.Update(section, "type", buffer.c_str());
	
	/* Serialize components here */
	if (auto dLightComp = object.GetComponent<DirLightComponent>())
	{
		buffer.clear();
		dLightComp->ToString(buffer);
		SerializeComponent(conf, section, DirLightComponent::GetComponentName(), buffer);
	}
	else if (auto pLightComp = object.GetComponent<PointLightComponent>())
	{
		buffer.clear();
		pLightComp->ToString(buffer);
		SerializeComponent(conf, section, PointLightComponent::GetComponentName(), buffer);
	}

	if (auto transComp = object.GetComponent<TransformComponent>())
	{
		buffer.clear();
		transComp->ToString(buffer);
		SerializeComponent(conf, section, TransformComponent::GetComponentName(), buffer);
	}
	if (auto meshComp = object.GetComponent<StaticMeshComponent>())
	{
		buffer.clear();
		meshComp->ToString(buffer);
		SerializeComponent(conf, section, StaticMeshComponent::GetComponentName(), buffer);
	}
}

void SceneSerializer::SerializeComponent(
	INIFileParser& conf, const char* section, const char* componentName, String& strComponent)
{
	char childSection[64]{};
	std::strcat(childSection, section);
	std::strcat(childSection, ":");
	std::strcat(childSection, componentName);

	/* Read line by line from string in C-Style to avoid slow memory allocations */
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
		conf.Update(childSection, name, value);

		if (nextLine)
			*nextLine = '\n';  /* then restore newline-char, just to be tidy */ 

		currentLine = nextLine ? (nextLine + 1) : nullptr;
	}
}
