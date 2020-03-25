#pragma once

#include <string>

namespace CrazyBakery
{
	class IScene;
}
using namespace CrazyBakery;

class CSceneLoader
{
public:
	bool Load(const std::wstring &strPath, IScene* pScene)
	{
		_LoadSceneFile(strPath, pScene);
		_LoadSceneResource(pScene);
		return true;
	}

	bool PrepareData(IScene* pScene);

private:
	bool _LoadSceneFile(const std::wstring &strPath, IScene* pScene);
	bool _LoadSceneResource(IScene* pScene);
};