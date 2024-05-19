#include "SceneSerializer.hpp"

#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"


#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Core/FileParser/INIFileParser.hpp"

#if 0

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeScene(Scene& scene, const fspath& filepath)
{
	INIFileParser conf(filepath);

	for (auto entity : scene.Reg().view<LabelComponent>())
	{
		GameObject obj{ entity, &scene.Reg() };
		SerializeObject(conf, obj);
	}
	conf.Write();
}

void SceneSerializer::DeserializeScene(Scene& scene, const fspath& filepath)
{
	INIFileParser conf(filepath);
	conf.ReadData();

	string buff;
	buff.reserve(512);

	GameObject obj;
	for (auto const& it : conf.GetData())
	{
		const string& section = it.first;

		/* Parse object */
		if (section.find(':') == string::npos)
		{
			buff = conf.GetValue(section.c_str(), "label");
			uint32_t type	= std::stoi(conf.GetValue(section.c_str(), "type"));
			obj	= scene.CreateObject(buff.c_str(), type);
		}
		/* Parse components */
		else
		{
			int component = std::stoi(std::strchr(section.c_str(), ':') + 1);
			if (component == static_cast<int>(Components::DirLightComponent))
			{
				/* 
					Parse DirLightComponent: 
						.Vec3f color
						.float ambient
						.float diffuse
						.float specular
						.Vec3f direction
				*/
				auto& dlightComp = obj.AddComponent<DirLightComponent>("");
				buff = conf.GetValue(section.c_str(), "color");

				dlightComp.color		= INIFileParser::StringToVec3f(buff);
				dlightComp.ambient	= std::stof(conf.GetValue(section.c_str(), "ambient"));
				dlightComp.diffuse	= std::stof(conf.GetValue(section.c_str(), "diffuse"));
				dlightComp.specular = std::stof(conf.GetValue(section.c_str(), "specular"));
				
				buff = conf.GetValue(section.c_str(), "direction");
				dlightComp.direction = INIFileParser::StringToVec3f(buff);
			}
			//else if (std::strcmp(componentName, PointLightComponent::GetComponentName(true)) == 0)
			//{
			//	/* 
			//		Parse PointLightComponent: 
			//			.Vec3f color
			//			.float ambient
			//			.float diffuse
			//			.float specular
			//			.Vec3f position 
			//	*/

			//	/* TODO */
			//}
			//else if (std::strcmp(componentName, SpotLightComponent::GetComponentName(true)) == 0)
			//{
			//	/* 
			//		Parse SpotLightComponent: 
			//			.Vec3f color
			//			.float ambient
			//			.float diffuse
			//			.float specular
			//			.Vec3f direction
			//			.float cutoff
			//	*/

			//	/* TODO */
			//}
			else if (component == static_cast<int>(Components::TransformComponent))
			{
				/* 
					Parse TransformComponent:
						.Vec3f position;
						.Vec3f scale;
						.Vec3f rotation;	
				*/
				auto& transComp = obj.AddComponent<TransformComponent>();
				buff = conf.GetValue(section.c_str(), "position");
				transComp.position = INIFileParser::StringToVec3f(buff);
				buff = conf.GetValue(section.c_str(), "scale");
				transComp.scale	= INIFileParser::StringToVec3f(buff);
				buff = conf.GetValue(section.c_str(), "rotation");
				transComp.rotation = INIFileParser::StringToVec3f(buff);
				transComp.UpdateTransformation();
			}
			else if (component == static_cast<int>(Components::StaticMeshComponent))
			{
				/* 
					Parse StaticMeshComponent:
						.string model-path			
						.string material-diffuse
						.string material-specular
				*/
				
				buff = conf.GetValue(section.c_str(), "model-path");
				if (buff.empty())
					continue;

				auto& smeshComp = obj.AddComponent<StaticMeshComponent>(buff);
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
	/* 
		Every object has both LabelComponent and TypeComponent: 
			[Entity_<id>]
			label=<label>
			type=<type>

		for each components:
			[Entity_<id>:<component-name>]
			<component-attributes>
			...
	*/
	
	char section[32]{};
	sprintf_s(section, "Entity_%d", static_cast<uint32_t>(object.GetObjectID()));

	LabelComponent* labelComp = object.GetComponent<LabelComponent>();
	conf.Update(section, "label", labelComp->label.c_str());
	
	TypeComponent* typeComp = object.GetComponent<TypeComponent>();
	char digits[8];
	sprintf_s(digits, "%d", typeComp->type);
	conf.Update(section, "type", digits);

	string buffer;
	buffer.reserve(512);

	char childSection[64]{ '\0' };

	/* Serialize components here */
	if (auto dLightComp = object.GetComponent<DirLightComponent>())
	{
		buffer.clear();
		dLightComp->Format(buffer);
		sprintf_s(childSection, "%s:%d", section, static_cast<int>(Components::DirLightComponent));
		SerializeComponent(conf, childSection, buffer);
	}
	//else if (auto pLightComp = object.GetComponent<PointLightComponent>())
	//{
	//	buffer.clear();
	//	pLightComp->ToString(buffer);
	//	SerializeComponent(conf, section, PointLightComponent::GetComponentName(), buffer);
	//}
	//else if (auto sLightComp = object.GetComponent<SpotLightComponent>())
	//{
	//	buffer.clear();
	//	sLightComp->ToString(buffer);
	//	SerializeComponent(conf, section, SpotLightComponent::GetComponentName(), buffer);
	//}
	if (auto transComp = object.GetComponent<TransformComponent>())
	{
		buffer.clear();
		transComp->Format(buffer);
		sprintf_s(childSection, "%s:%d", section, static_cast<int>(Components::TransformComponent));
		SerializeComponent(conf, childSection, buffer);
	}
	if (auto meshComp = object.GetComponent<StaticMeshComponent>())
	{
		buffer.clear();
		meshComp->Format(buffer);
		sprintf_s(childSection, "%s:%d", section, static_cast<int>(Components::StaticMeshComponent));
		SerializeComponent(conf, childSection, buffer);
	}
}

void SceneSerializer::SerializeComponent(INIFileParser& conf, const char* childSection, string& formatted)
{
	/* Read line by line from string in C-Style to avoid slow memory allocations */
	char* currentLine = formatted.data();
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

#endif