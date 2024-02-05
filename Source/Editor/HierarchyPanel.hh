#pragma once

#include "../Core.hh"

class HierarchyPanel
{
public:
	HierarchyPanel(const char* name, Vec2i size);
	~HierarchyPanel() = default;

	bool isOpen;
	bool isPropertiesOpen = true;

	Vec2i panelSize;
	String panelName;

	void RenderPanel(class Scene* scene);

private:
	Vector<class Texture2D*> _textures;

	bool _dirLightSelected;
	int _pointLightSelected;
	int _staticMeshSelected;

	String _treeName;
	String _treeNode;

	void ShowPropertiesPanel(class StaticMesh* meshTarget);
	void ShowPropertiesPanel(class DirectionalLight* dirLight);
	void ShowPropertiesPanel(class PointLight* pointLight);

	void SetTreeName(const char* name, int count = -1);

	void SetTreeNodeName(const char* name, int count = -1);

	/* Show directional light node of the tree */
	void DirLightNode();

	/* List the point light nodes of the tree */
	void PointLightNodes(Vector<PointLight*> pointLights);

	/* List the static mesh nodes of the tree */
	void StaticMeshNodes(Vector<StaticMesh*> meshList);

};

