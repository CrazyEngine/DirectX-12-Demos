#include "GPUBake.h"

#include <assert.h>
//#include <optixu/optixu_matrix_namespace.h>
//using namespace optix;

#include "../Baker.h"
#include <Windows.h>


namespace CrazyBakery
{


IMesh::IMesh()
{
}
IMesh::~IMesh()
{
	if (m_pVertexPosition)
	{
		delete[] m_pVertexPosition;
		m_pVertexPosition = NULL;
	}

	if (m_pVertexNormal)
	{
		delete[] m_pVertexNormal;
		m_pVertexNormal = NULL;
	}

	if (m_pVertexUV)
	{
		delete[] m_pVertexUV;
		m_pVertexUV = NULL;
	}

	if (m_pVertexUV2)
	{
		delete[] m_pVertexUV2;
		m_pVertexUV2 = NULL;
	}
	
	if (m_pVertexIndices)
	{
		delete[] m_pVertexIndices;
		m_pVertexIndices = NULL;
	}

#ifdef VertexAttributeUseDifferentIndex
	if (m_pVertexNormalIndex)
	{
		delete[] m_pVertexNormalIndex;
		m_pVertexNormalIndex = NULL;
	}

	/*if (m_pVertexUVIndex)
	{
		delete[] m_pVertexUVIndex;
		m_pVertexUVIndex = NULL;
	}*/

	if (m_pVertexUV2Index)
	{
		delete[] m_pVertexUV2Index;
		m_pVertexUV2Index = NULL;
	}
#endif

	if (_pFaceNormals)
	{
		delete[] _pFaceNormals;
		_pFaceNormals = NULL;
	}

	if (m_pSubMeshes)
	{
		delete[] m_pSubMeshes;
		m_pSubMeshes = NULL;
	}
}

#ifdef VertexAttributeUseDifferentIndex
bool IMesh::ResizeBuffers(size_t numVertices, size_t numNormals, size_t numUVs, size_t numUV2s, size_t numFaces)
#else
bool IMesh::ResizeBuffers(size_t numVertices, size_t numFaces)
#endif
{
	m_numVertex = numVertices;
	m_numFaces = numFaces;

	if (m_pVertexPosition)
		delete m_pVertexPosition;
	m_pVertexPosition = new MyFloat3[numVertices];

#ifdef VertexAttributeUseDifferentIndex
	m_numNormal = numNormals;
	if (m_pVertexNormal)
		delete m_pVertexNormal;
	m_pVertexNormal = new MyFloat3[numNormals];

	m_numUV2 = numUV2s;
	if (m_pVertexUV2)
		delete m_pVertexUV2;
	m_pVertexUV2 = new MyFloat2[numUV2s];
#else
	if (m_pVertexNormal)
		delete m_pVertexNormal;
	m_pVertexNormal = new MyFloat3[numVertices];

	if (m_pVertexUV2)
		delete m_pVertexUV2;
	m_pVertexUV2 = new MyFloat2[numVertices];
#endif
	

	if (m_pVertexIndices)
		delete m_pVertexIndices;
	m_pVertexIndices = new MyInt3[numFaces];

	if (_pFaceNormals)
		delete _pFaceNormals;
	_pFaceNormals = new MyFloat3[numFaces];

#ifdef VertexAttributeUseDifferentIndex
	if (m_pVertexNormalIndex)
		delete m_pVertexNormalIndex;
	m_pVertexNormalIndex = new MyInt3[numNormals];

	/*if (m_pVertexUVIndex)
		delete m_pVertexUVIndex;
	m_pVertexUVIndex = new MyFloat3[numUVs];*/

	if (m_pVertexUV2Index)
		delete m_pVertexUV2Index;
	m_pVertexUV2Index = new MyInt3[numUV2s];
#endif

	return true;
}

bool IMesh::ResizeSubMeshes(size_t numSubMeshes)
{
	if (m_pSubMeshes)
		delete[] m_pSubMeshes;
	m_pSubMeshes = new IMesh::ISubMesh[numSubMeshes];

	m_numSubMesh = numSubMeshes;

	return true;
}



std::map<std::wstring, ITexture*> m_vTextures;

class CMaterialImpl : public IMaterial
{
public:
	CMaterialImpl()
	{

	}
	virtual ~CMaterialImpl()
	{

	}

	virtual void SetName(const wchar_t* pName)
	{
		m_strName = pName;
	}
	virtual const wchar_t* GetName() const
	{
		return m_strName.c_str();
	}
	virtual void SetLightMode(const wchar_t* pName)
	{
		m_strLightMode = pName;
	}
	virtual const wchar_t* GetLightMode()
	{
		return m_strLightMode.c_str();
	}
	virtual void SetVS(const wchar_t* pName)
	{
		m_strVS = pName;
	}
	virtual const wchar_t* GetVS()
	{
		return m_strVS.c_str();
	}
	virtual void SetFS(const wchar_t* pName)
	{
		m_strFS = pName;
	}
	virtual const wchar_t* GetFS()
	{
		return m_strFS.c_str();
	}

	virtual void SetAlbedo(const wchar_t* pName)
	{
		m_strAlbedo = pName;
	}
	virtual const wchar_t* GetAlbedo()
	{
		return m_strAlbedo.c_str();
	}

	virtual void SetEmissive(const wchar_t* pName)
	{
		m_strEmissive = pName;
	}
	virtual const wchar_t* GetEmissive()
	{
		return m_strEmissive.c_str();
	}

private:
	std::wstring m_strName;
	std::wstring m_strLightMode;
	std::wstring m_strVS;
	std::wstring m_strFS;
	std::wstring m_strAlbedo;
	std::wstring m_strEmissive;
};



IStaticEntity::IStaticEntity()
{
	//m_vMaterials = new std::vector<CMaterial*>;
}
IStaticEntity::~IStaticEntity()
{
	//delete m_vMaterials;
}


class CStaticEntityImpl : public IStaticEntity
{
public:
	CStaticEntityImpl()
	{}
	virtual ~CStaticEntityImpl()
	{
	}

	virtual const wchar_t* GetEntityName() const
	{
		return m_strEntityName.c_str();
	}
	virtual void SetEntityName(const wchar_t* pName)
	{
		m_strEntityName = pName;
	}

	virtual const wchar_t* GetMeshName() const
	{
		return m_strMeshName.c_str();
	}
	virtual void SetMeshName(const wchar_t* pName)
	{
		m_strMeshName = pName;
	}

	virtual void SetMesh(IMesh* pMesh)
	{
		m_pMesh = pMesh;
	}
	virtual const IMesh* GetMesh() const
	{
		return m_pMesh;
	}

	virtual void AddMaterialName(const wchar_t* pName)
	{
//		assert(m_vMaterialNames.size() < m_pMesh->m_numSubMesh);
		m_vMaterialNames.push_back(pName);
	}
	virtual void AddMaterial(IMaterial* pMaterial)
	{
		assert(m_vMaterials.size() < m_pMesh->m_numSubMesh);
		m_vMaterials.push_back(pMaterial);
	}
	virtual void SetMaterial(size_t iIndex, IMaterial* pMaterial)
	{
		assert(iIndex< m_vMaterials.size());
		m_vMaterials[iIndex] = pMaterial;
	}
	virtual const IMaterial* GetMaterial(size_t iIndex) const
	{
		assert(iIndex < m_vMaterials.size());
		return m_vMaterials[iIndex];
	}
	virtual const wchar_t* GetMaterialName(size_t iIndex) const
	{
		assert(iIndex < m_vMaterialNames.size());
		return m_vMaterialNames[iIndex].c_str();
	}

private:
	std::wstring m_strEntityName;
	std::wstring m_strMeshName;
	IMesh* m_pMesh = NULL;
//	std::wstring m_strMaterialName;			//由于现在使用的是obj，加载obj时会自动加载材质，所以这个变量暂时没用
//	CMaterial* m_pMaterial = NULL;
	std::vector<std::wstring> m_vMaterialNames;
	std::vector<IMaterial*> m_vMaterials;	//一般来说，元素的个数和m_pMesh里的m_vSubMeshes里面的元素个数一样
};


class CLightmapImpl : public IScatterer
{
public:
	CLightmapImpl()
	{
		m_fOvertureScaleFactor = 1.0f;
	}
	virtual ~CLightmapImpl()
	{
		if (m_pAlbedo)
		{
			delete[] m_pAlbedo;
			m_pAlbedo = NULL;
		}

		if (m_pLightmap)
		{
			delete[] m_pLightmap;
			m_pLightmap = NULL;
		}

		for (size_t i = 0; i < m_vEntities.size(); ++i)
		{
			IStaticEntity* pEntity = m_vEntities[i];
			if (pEntity)
				delete pEntity;
		}
		m_vEntities.clear();
	}

	virtual void SetName(const wchar_t* strName)
	{
		m_strName = strName;
	}
	virtual const wchar_t* GetName() const
	{
		return m_strName.c_str();
	}

	virtual void SetOutputPath(const wchar_t* strName)
	{
		m_strOutputPath = strName;
	}
	virtual const wchar_t* GetOutputPath() const
	{
		return m_strOutputPath.c_str();
	}

	virtual void Pushback(IStaticEntity* pEntity)
	{
		m_vEntities.push_back(pEntity);
	}
	virtual size_t GetEntityNumber() const { return m_vEntities.size(); }
	virtual IStaticEntity* GetEntity(size_t iIndex) const
	{
		assert( iIndex < m_vEntities.size() );
		return m_vEntities[iIndex];
	}

	virtual void ResetAlbedoBuffer(size_t numWidth, size_t numHeight)
	{
		_numAlbedoWidth = numWidth;
		_numAlbedoHeight = numHeight;

		if (m_pAlbedo)
		{
			delete[] m_pAlbedo;
			m_pAlbedo = NULL;
		}
		m_pAlbedo = new MyFloat3[numWidth * numHeight];//在exe工程里创建buffer的话，在析构函数里释放会崩溃。Why？
	}
	virtual size_t GetAlbedoBufferWidth() const { return _numAlbedoWidth; }
	virtual size_t GetAlbedoBufferHeight() const { return _numAlbedoHeight; }
	virtual MyFloat3* GetAlbedoBuffer() const
	{
		return m_pAlbedo;
	}

	virtual void ResetLightmapBuffer()
	{
		if (m_pLightmap)
		{
			delete[] m_pLightmap;
			m_pLightmap = NULL;
		}
		size_t numFragments = m_texWidth * m_texHeight;
		m_pLightmap = new MyFloat4[numFragments];//在exe工程里创建buffer的话，在析构函数里释放会崩溃。Why？
	}
	virtual MyFloat4* GetLightmapBuffer() const
	{
		return m_pLightmap;
	}

private:
	std::wstring m_strName;
	std::wstring m_strOutputPath;//Lightmap的输出路径，例如"..\\Data\\CornellBoxMerged.Unity\\Output\\CornellBox.png"，来自于json文件
	std::vector<IStaticEntity*> m_vEntities;
	MyFloat3* m_pAlbedo = NULL;
	size_t _numAlbedoWidth;
	size_t _numAlbedoHeight;

	MyFloat4* m_pLightmap = NULL;
};


class CBakeOptionImpl : public IBakeOption
{
public:
	CBakeOptionImpl()
	{
		_vSampleNumberInOverture.clear();
		_vSampleNumberInOverture.push_back(64);
		_vSampleNumberInOverture.push_back(16);
		_vSampleNumberInOverture.push_back(4);
	}
	virtual ~CBakeOptionImpl(){}

	virtual void SetSampleNumbersInOverture(size_t numIterators, size_t* pNumbers)
	{
		_vSampleNumberInOverture.clear();
		for (size_t i=0 ; i < numIterators; ++i)
		{
			_vSampleNumberInOverture.push_back(pNumbers[i]);
		}
	}
	virtual size_t GetSampleNumberInOverture(size_t iIterator) const
	{
		return _vSampleNumberInOverture[iIterator];
	}

	virtual size_t GetIndirectIterateNumbersInOverture() const
	{
		return _vSampleNumberInOverture.size();
	}

	virtual void SetSampleNumbersInBeauty(size_t numSamples)
	{
		m_numSamplesPerTexelInBeauty = numSamples;
	}
	virtual size_t GetSampleNumbersInBeauty() const
	{
		return m_numSamplesPerTexelInBeauty;
	}

private:
	std::vector<size_t> _vSampleNumberInOverture;
	/*
	** It determines the number of rays that will be shot from each
	** texel to estimate irradiance.
	** The more rays you shoot, the better result you will get.
	** Range [1, inf)
	*/
	int m_numSamplesPerTexelInBeauty = 1024;

};

class CLightmapDataImpl : public ILightmap
{
public:
	CLightmapDataImpl()
	{

	}
	virtual ~CLightmapDataImpl()
	{
		if (m_pLightmap)
		{
			delete[] m_pLightmap;
			m_pLightmap = NULL;
		}
	}

	virtual void SetOutputPath(const wchar_t* strName)
	{
		m_strOutputPath = strName;
	}
	virtual const wchar_t* GetOutputPath() const
	{
		return m_strOutputPath.c_str();
	}

	virtual void ResetBuffer()
	{
		if (m_pLightmap)
		{
			delete[] m_pLightmap;
			m_pLightmap = NULL;
		}
		size_t numFragments = m_texWidth * m_texHeight;
		m_pLightmap = new MyFloat4[numFragments];//在exe工程里创建buffer的话，在析构函数里释放会崩溃。Why？
	}
	virtual MyFloat4* GetBuffer() const
	{
		return m_pLightmap;
	}

private:
	std::wstring m_strOutputPath;//Lightmap的输出路径，例如"..\\Data\\CornellBoxMerged.Unity\\Output\\CornellBox.png"，来自于json文件

	MyFloat4* m_pLightmap = NULL;
};

class CSceneImpl : public IScene
{
public:
	CSceneImpl()
	{

	}
	virtual ~CSceneImpl()
	{
		for (size_t i=0;i< m_vLightmaps.size();++i)
		{
			IScatterer* pLightmap = m_vLightmaps[i];
			if (pLightmap)
				delete pLightmap;
		}
		m_vLightmaps.clear();

		//不用在这里析构，在Lightmap类里释放的
		m_vAllStaticEntities.clear();

		std::map<std::wstring, IMesh*>::iterator it = _vMeshes.begin();
		for (; it != _vMeshes.end(); ++it)
		{
			if (it->second)
			{
				delete it->second;
			}
		}
		_vMeshes.clear();

		std::map<std::wstring, IMaterial*>::iterator itMaterial = _vMaterials.begin();
		for (; itMaterial != _vMaterials.end(); ++itMaterial)
		{
			if (itMaterial->second)
			{
				delete itMaterial->second;
			}
		}
		_vMaterials.clear();

		//IMesh::DestroyAll();
		//IMaterial::DestroyAll();
	}

	virtual void SetFileName(const wchar_t* strName)
	{
		_strFileName = strName;
	}
	virtual const wchar_t* GetFileName() const
	{
		return _strFileName.c_str();
	}

	virtual void SetName(const wchar_t* strName)
	{
		_strName = strName;
	}
	virtual const wchar_t* GetName() const
	{
		return _strName.c_str();
	}


	virtual IMesh* CreateMesh(const wchar_t* strName)
	{
		assert(NULL == GetMesh(strName));
		IMesh* pMesh = new IMesh;
		_vMeshes[strName] = pMesh;
		return pMesh;
	}
	virtual IMesh* GetMesh(const wchar_t* strName)
	{
		std::map<std::wstring, IMesh*>::iterator it = _vMeshes.find(strName);
		if (it == _vMeshes.end())
		{
			return NULL;
		}
		else
		{
			return it->second;
		}
	}


	virtual IMaterial* CreateMaterial(const wchar_t* strName)
	{
		std::map<std::wstring, IMaterial*>::iterator it = _vMaterials.find(strName);
		if (it == _vMaterials.end())
		{
			IMaterial* pMaterial = new CMaterialImpl;
			_vMaterials[strName] = pMaterial;
			return pMaterial;
		}
		else
		{
			return it->second;
		}
	}
	virtual IMaterial* GetMaterial(const wchar_t* strName)
	{
		std::map<std::wstring, IMaterial*>::iterator it = _vMaterials.find(strName);
		if (it == _vMaterials.end())
		{
			return NULL;
		}
		else
		{
			return it->second;
		}
	}
	

	virtual IScatterer* AddScatterer()
	{
		IScatterer* pLightmap = new CLightmapImpl;
		m_vLightmaps.push_back(pLightmap);

//		m_vAllStaticEntities.push_back(new CEntityGroup);

		return pLightmap;
	}
	virtual size_t GetScattererNumber() const
	{
		return m_vLightmaps.size();
	}
	virtual const IScatterer* GetScatterer(size_t iIndex) const
	{
		assert(iIndex<m_vLightmaps.size());
		return m_vLightmaps[iIndex];
	}

	virtual IStaticEntity* CreateStaticEntity()
	{
		IStaticEntity* pEntity = new CStaticEntityImpl;
		return pEntity;
	}
	virtual void Pushback(IStaticEntity* pEntity)
	{
		size_t iLM = pEntity->m_iLightmap;
		IScatterer* pLightmap = m_vLightmaps[iLM];
		pLightmap->Pushback(pEntity);

		m_vAllStaticEntities.push_back(pEntity);
	}
	virtual size_t GetEntityNumber(size_t iLightmap) const
	{
		assert(iLightmap < m_vLightmaps.size());
		return m_vLightmaps[iLightmap]->GetEntityNumber();
	}
	virtual size_t GetEntityNumber() const
	{
		/*size_t num = 0;
		for (size_t i = 0; i < m_vLightmaps.size(); ++i)
		{
			num += m_vLightmaps[i]->GetEntityNumber();
		}
		return num;*/
		return m_vAllStaticEntities.size();
	}
	virtual IStaticEntity* GetEntity(size_t iLightmap,size_t iEntity) const
	{
		assert(iLightmap < m_vLightmaps.size());
		IScatterer* pScatter = m_vLightmaps[iLightmap];
		return pScatter->GetEntity(iEntity);
	}
	virtual IStaticEntity* GetEntityByGlobalID(size_t idGlobal) const
	{
		/*size_t num = idGlobal;
		for (size_t i = 0; i < m_vLightmaps.size(); ++i)
		{
			IScatterer* pLightmap = m_vLightmaps[i];
			if (num < pLightmap->GetEntityNumber())
			{
				return pLightmap->GetEntity(num);
			}
			num -= pLightmap->GetEntityNumber();
		}
		return NULL;*/
		return m_vAllStaticEntities[idGlobal];
	}

	virtual void Pushback(ILight* pLight)
	{
		m_vLights.push_back(pLight);
	}
	virtual size_t GetLightNum() const
	{
		return m_vLights.size();
	}
	virtual ILight* GetLight(size_t iIndex) const
	{
		assert(iIndex<m_vLights.size());
		return m_vLights[iIndex];
	}

private:
	std::wstring _strFileName;
	std::wstring _strName;
	std::map<std::wstring, IMesh*> _vMeshes;
	std::map<std::wstring, IMaterial*> _vMaterials;//TODO 加载完毕场景应该清除
	std::vector<IScatterer*> m_vLightmaps;
	std::vector<IStaticEntity*> m_vAllStaticEntities;	//场景里所有的静态实体，用于运行时根据ID快速索引
//	typedef std::vector<CStaticEntity*> CEntityGroup;
//	std::vector<CEntityGroup*> m_vAllStaticEntities;	//场景里所有的静态实体，每一个元素都和m_vClassScatterers里的相对应
	std::vector<ILight*> m_vLights;			//场景里所有的光源
};



class CTimerImpl : public ITimer
{
public:
	CTimerImpl()
	{
		m_LastCounter.HighPart = 0;
		_TotalCounter.HighPart = 0;
		QueryPerformanceFrequency(&m_Frequency);
	}

	virtual void Reset()
	{
		_TotalCounter.QuadPart = 0;
		QueryPerformanceCounter(&m_LastCounter);
	}
	virtual void Pause()
	{
		LARGE_INTEGER CurrentCounter;
		QueryPerformanceCounter(&CurrentCounter);
		_TotalCounter.QuadPart += CurrentCounter.QuadPart - m_LastCounter.QuadPart;
	}
	virtual void Resume()
	{
		QueryPerformanceCounter(&m_LastCounter);
	}
	virtual double GetElapsedTime()
	{
		LARGE_INTEGER CurrentCounter;
		QueryPerformanceCounter(&CurrentCounter);
		_TotalCounter.QuadPart += CurrentCounter.QuadPart - m_LastCounter.QuadPart;
		return (double)_TotalCounter.QuadPart / (double)m_Frequency.QuadPart;
	}

private:
	LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_LastCounter;//最近一次重置或者重启计时时的计数
	LARGE_INTEGER _TotalCounter;
};


class CCrazyBakeryImpl : public ICrazyBakery
{
public:
	CCrazyBakeryImpl() {}
	virtual ~CCrazyBakeryImpl() {}

	virtual bool Init()
	{
		return _cBaker.Init();
	}

	virtual IScene* GetScene(const wchar_t* strName)
	{
		std::map<std::wstring, IScene*>::iterator it = _vScenes.find(strName);
		if (it == _vScenes.end())
		{
			IScene* pScene = new CSceneImpl;
			pScene->SetFileName(strName);
			_vScenes[strName] = pScene;
			return pScene;
		}
		else
		{
			return it->second;
		}
	}
	virtual void ReleaseScene(const wchar_t* strName)
	{
		std::map<std::wstring, IScene*>::iterator it = _vScenes.find(strName);
		if (it != _vScenes.end())
		{
			IScene* pScene = it->second;
			delete pScene;
			_vScenes.erase(it);
		}
	}

	virtual IBakeOption* GetBakeOption(const wchar_t* strName)
	{
		std::map<std::wstring, IBakeOption*>::iterator it = _vBakeOptions.find(strName);
		if (it == _vBakeOptions.end())
		{
			IBakeOption* pOption = new CBakeOptionImpl;
			_vBakeOptions[strName] = pOption;
			return pOption;
		}
		else
		{
			return it->second;
		}
	}
	virtual void ReleaseBakeOption(const wchar_t* strName)
	{
		std::map<std::wstring, IBakeOption*>::iterator it = _vBakeOptions.find(strName);
		if (it != _vBakeOptions.end())
		{
			IBakeOption* pOption = it->second;
			delete pOption;
			_vBakeOptions.erase(it);
		}
	}

	virtual ILightmap* GetLightmapData(const size_t iIndex)
	{
		std::map<size_t, ILightmap*>::iterator it = _vLightmapDatas.find(iIndex);
		if (it == _vLightmapDatas.end())
		{
			ILightmap* pLightmapData = new CLightmapDataImpl;
			_vLightmapDatas[iIndex] = pLightmapData;
			return pLightmapData;
		}
		else
		{
			return it->second;
		}
	}
	virtual void ReleaseLightmapData(const size_t iIndex)
	{
		std::map<size_t, ILightmap*>::iterator it = _vLightmapDatas.find(iIndex);
		if (it != _vLightmapDatas.end())
		{
			ILightmap* pLightmapData = it->second;
			delete pLightmapData;
			_vLightmapDatas.erase(it);
		}
	}

	virtual ITimer* GetTimer(const wchar_t* strName)
	{
		std::map<std::wstring, ITimer*>::iterator it = _vTimers.find(strName);
		if (it == _vTimers.end())
		{
			ITimer* pTimer = new CTimerImpl;
			_vTimers[strName] = pTimer;
			return pTimer;
		}
		else
		{
			return it->second;
		}
	}
	virtual void ReleaseTimer(const wchar_t* strName)
	{
		std::map<std::wstring, ITimer*>::iterator it = _vTimers.find(strName);
		if (it != _vTimers.end())
		{
			ITimer* pTimer = it->second;
			delete pTimer;
			_vTimers.erase(it);
		}
	}


	virtual bool Bake(const IScene* pScene, const IBakeOption* pOption, ILightmap** ppLightmapDatas, size_t numLightmapDatas)
	{
		std::vector<ILightmap*> vLightmapDatas;
		for (size_t i = 0; i < numLightmapDatas; ++i)
		{
			vLightmapDatas.push_back(ppLightmapDatas[i]);
		}

		if (_cBaker.Start(pScene, pOption, vLightmapDatas) == 0)
			return true;

		return false;
	}
	virtual bool Close()
	{
		{
			std::map<size_t, ILightmap*>::iterator it = _vLightmapDatas.begin();
			for (; it != _vLightmapDatas.end(); ++it)
			{
				if (it->second)
				{
					delete it->second;
				}
			}
			_vLightmapDatas.clear();
		}

		{
			std::map<std::wstring, IBakeOption*>::iterator it = _vBakeOptions.begin();
			for (; it != _vBakeOptions.end(); ++it)
			{
				if (it->second)
				{
					delete it->second;
				}
			}
			_vBakeOptions.clear();
		}

		{
			std::map<std::wstring, IScene*>::iterator it = _vScenes.begin();
			for (; it != _vScenes.end(); ++it)
			{
				if (it->second)
				{
					delete it->second;
				}
			}
			_vScenes.clear();
		}
		
		{
			std::map<std::wstring, ITimer*>::iterator it = _vTimers.begin();
			for (; it != _vTimers.end(); ++it)
			{
				if (it->second)
				{
					delete it->second;
				}
			}
			_vTimers.clear();
		}

		return _cBaker.Close();
	}

private:
	Baker _cBaker;
	std::map<std::wstring, IScene*> _vScenes;
	std::map<std::wstring, IBakeOption*> _vBakeOptions;
	std::map<size_t, ILightmap*> _vLightmapDatas;
	std::map<std::wstring, ITimer*> _vTimers;
};

ICrazyBakery* CreateCrazyBakery()
{
	return new CCrazyBakeryImpl;
}


void DumpMergedScatterer(const IScene* pScene, const wchar_t* strPath, const wchar_t* strTextureType)
{
}
void DumpBuffer(size_t numWidth, size_t numHeight, MyFloat3* pData, const wchar_t* strPath, const wchar_t* StageName, int iLightmap, const wchar_t* strName, bool boFloat)
{
}
pDumpMergedScatterer g_pDumpMergedScatterer = DumpMergedScatterer;
pDumpBuffer g_pDumpBuffer = DumpBuffer;

void ProcessMessage(EMessageType eType, const wchar_t* strMessage)
{
}
pProcessMessage g_pProcessMessage = ProcessMessage;


}