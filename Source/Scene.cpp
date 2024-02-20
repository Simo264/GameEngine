#include "Scene.hh"
#include "Shader.hh"
#include "Logger.hh"
#include "Texture2D.hh"
#include "Subsystems/TexturesManager.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Scene::ClearScene() 
{ 
	sceneDLight.reset();

	for (auto& pLight : scenePLights)
		pLight.reset();
	scenePLights.clear();

	for (auto& sMesh: sceneSMeshes)
		sMesh.reset();
	sceneSMeshes.clear();
}

void Scene::LoadScene(Path filepath)
{
	IFStream file(filepath.string());
	if (!file.is_open())
	{
		CONSOLE_WARN("Error on opening file '{}'", filepath.string());
		return;
	}

	String line;
	char attrName[64]{};
	char attrVal[64]{};
	while (std::getline(file, line))
	{
		/* Ignore blank line */
		if (line.empty())
			continue;
		
		/* Ignore comments */
		if (line[0] == '#')
			continue;

		/* Read type of object */
		ParseNameValue(line, attrName, attrVal);
		if (std::strcmp(attrVal, "DirectionalLight") == 0)
		{
			SharedPointer<DirectionalLight> dLight = ParseDirectionalLight(file, line, attrName, attrVal);
			this->AddSceneObject(dLight);
		}
		else if (std::strcmp(attrVal, "PointLight") == 0)
		{
			SharedPointer<PointLight> pLight = ParsePointLight(file, line, attrName, attrVal);
			this->AddSceneObject(pLight);
		}
		else if (std::strcmp(attrVal, "SpotLight") == 0)
		{

		}
		else if (std::strcmp(attrVal, "StaticMesh") == 0)
		{
			SharedPointer<StaticMesh> sMesh = ParseStaticMesh(file, line, attrName, attrVal);
			this->AddSceneObject(sMesh);
		}
	}
}

void Scene::SaveScene(Path outfile)
{

}

void Scene::DrawScene(Shader* shader)
{
	/* Render directional light */
	if (HasDirLight())
		if (sceneDLight->visible)
			sceneDLight->RenderLight(shader);

	/* Render point lights */
	for (auto& scenePLight : scenePLights)
		if (scenePLight->visible)
			scenePLight->RenderLight(shader);

	/* Render static mesh objects (enable cull face to improve performance) */
	for (auto& sceneSMesh : sceneSMeshes)
		if (sceneSMesh->visible)
			sceneSMesh->Draw(shader);
}

void Scene::AddSceneObject(const SharedPointer<DirectionalLight>& obj)
{
	sceneDLight = obj;
}

void Scene::AddSceneObject(const SharedPointer<PointLight>& obj)
{
	scenePLights.push_back(obj);
}

void Scene::AddSceneObject(const SharedPointer<StaticMesh>& obj)
{
	sceneSMeshes.push_back(obj);
}

void Scene::RemoveSceneObject(const SharedPointer<DirectionalLight>& obj)
{
	sceneDLight = nullptr;
}

void Scene::RemoveSceneObject(const SharedPointer<PointLight>& obj)
{
	auto beg = scenePLights.begin();
	auto end = scenePLights.end();
	auto it = std::find_if(beg, end, [&](SharedPointer<PointLight>& o) {
		return o->Compare(*obj);
		});

	if (it != end)
		scenePLights.erase(it);
}

void Scene::RemoveSceneObject(const SharedPointer<StaticMesh>& obj)
{
	auto beg = sceneSMeshes.begin();
	auto end = sceneSMeshes.end();
	auto it = std::find_if(beg, end, [&](SharedPointer<StaticMesh>& o) {
		return o->Compare(*obj);
		});

	if (it != end)
		sceneSMeshes.erase(it);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Scene::ParseNameValue(const String& line, char name[64], char value[64])
{
	auto sep = line.find('=');
	std::copy(line.begin(), line.begin() + sep, name);
	std::copy(line.begin() + sep + 1, line.end(), value);
}

void Scene::ClearNameValue(char name[64], char value[64])
{
	std::fill_n(name, 64, 0);
	std::fill_n(value, 64, 0);
}

Vec3f Scene::ParseVec3f(const String& str)
{
	Vec3f vec;
	
	IStringStream iss(str);
	String seg;
	std::getline(iss, seg, ',');
	vec.x = std::stof(seg);
	std::getline(iss, seg, ',');
	vec.y = std::stof(seg);
	std::getline(iss, seg, ',');
	vec.z = std::stof(seg);
	return vec;
}

SharedPointer<DirectionalLight> Scene::ParseDirectionalLight(
	IFStream& file, String& line, char name[64], char value[64])
{
	String tag;
	Vec3f color;
	float ambient;
	float diffuse;
	float specular;
	Vec3f direction;
	for (int i = 0; i < 6; i++)
	{
		ClearNameValue(name, value);
		std::getline(file, line);
		ParseNameValue(line, name, value);

		if (std::strcmp(name, "tag") == 0)
			tag = value;
		
		else if (std::strcmp(name, "color") == 0)
			color = ParseVec3f(value);
		
		else if (std::strcmp(name, "ambient") == 0)
			ambient = std::stof(value);
		
		else if (std::strcmp(name, "diffuse") == 0)
			diffuse = std::stof(value);
		
		else if (std::strcmp(name, "specular") == 0)
			specular = std::stof(value);
		
		else if (std::strcmp(name, "direction") == 0)
			direction = ParseVec3f(value);
	}

	SharedPointer<DirectionalLight> dLight = std::make_shared<DirectionalLight>("UDirLight");
	dLight->color = color;
	dLight->ambient = ambient;
	dLight->diffuse = diffuse;
	dLight->specular = specular;
	dLight->direction = direction;
	dLight->tagName = tag;
	return dLight;
}

SharedPointer<PointLight> Scene::ParsePointLight(
	IFStream& file, String& line, char name[64], char value[64])
{
	String tag;
	Vec3f color;
	float ambient;
	float diffuse;
	float specular;
	Vec3f position;
	for (int i = 0; i < 6; i++)
	{
		ClearNameValue(name, value);
		std::getline(file, line);
		ParseNameValue(line, name, value);

		if (std::strcmp(name, "tag") == 0)
			tag = value;

		else if (std::strcmp(name, "color") == 0)
			color = ParseVec3f(value);

		else if (std::strcmp(name, "ambient") == 0)
			ambient = std::stof(value);

		else if (std::strcmp(name, "diffuse") == 0)
			diffuse = std::stof(value);

		else if (std::strcmp(name, "specular") == 0)
			specular = std::stof(value);

		else if (std::strcmp(name, "position") == 0)
			position = ParseVec3f(value);
	}

	char uname[16]{};
	sprintf_s(uname, "UPointLight[%d]", (int) this->scenePLights.size());

	SharedPointer<PointLight> pLight = std::make_shared<PointLight>(uname);
	pLight->color = color;
	pLight->ambient = ambient;
	pLight->diffuse = diffuse;
	pLight->specular = specular;
	pLight->position = position;
	pLight->tagName = tag;
	return pLight;
}

SharedPointer<StaticMesh> Scene::ParseStaticMesh(
	IFStream& file, String& line, char name[64], char value[64])
{
	String tag;
	Path modelPath;
	Texture2D* textDiffuse = nullptr;
	Vec3f position;
	Vec3f scale;
	Vec3f degrees;
	for (int i = 0; i < 6; i++)
	{
		ClearNameValue(name, value);
		std::getline(file, line);
		ParseNameValue(line, name, value);

		if (std::strcmp(name, "tag") == 0)
			tag = value;

		else if (std::strcmp(name, "modelpath") == 0)
			modelPath = ROOT_PATH / value;

		else if (std::strcmp(name, "texture-diffuse") == 0)
			textDiffuse = TexturesManager::Instance().GetTextureByPath(ROOT_PATH / value);

		else if (std::strcmp(name, "scale") == 0)
			scale = ParseVec3f(value);

		else if (std::strcmp(name, "position") == 0)
			position = ParseVec3f(value);

		else if (std::strcmp(name, "degrees") == 0)
			degrees = ParseVec3f(value);
	}


	SharedPointer<StaticMesh> sMesh = std::make_shared<StaticMesh>(modelPath);
	sMesh->diffuse = textDiffuse;
	sMesh->specular = nullptr;
	sMesh->transform.position = position;
	sMesh->transform.scale = scale;
	sMesh->transform.degrees = degrees;
	sMesh->tagName = tag;
	return sMesh;
}
