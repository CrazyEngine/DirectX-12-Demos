#pragma once

#include <vector>
#include <map>
#include <string>

//需要在项目属性里定义的宏
//VertexAttributeUseDifferentIndex 如果定义了，顶点所有的属性都有自己的索引缓冲区，obj、FBX模型每一个顶点属性都使用自己的一个索引缓冲区，
//而引擎里的模型格式大都共用同一个索引缓冲区
//

#ifdef _CRAZY_BAKERY_EXPORT_
#define CRAZY_BAKERY_API __declspec(dllexport)
#else
#define CRAZY_BAKERY_API __declspec(dllimport)
#endif

namespace CrazyBakery
{
struct CRAZY_BAKERY_API MyFloat2
{
	float x, y;
	MyFloat2& operator += (const MyFloat2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	MyFloat2& operator *= (const MyFloat2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
};

struct CRAZY_BAKERY_API MyFloat3
{
	float x, y, z;
	MyFloat3& operator += (const MyFloat3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	MyFloat3& operator -= (const MyFloat3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	MyFloat3& operator - ()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
};

struct CRAZY_BAKERY_API MyFloat4
{
	float x, y, z, w;
	MyFloat4& operator += (const MyFloat4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	MyFloat4& operator -= (const MyFloat4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	MyFloat4& operator - ()
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
		return *this;
	}
};

struct CRAZY_BAKERY_API MyInt2
{
	int x, y;
};
struct CRAZY_BAKERY_API MyInt3
{
	int x, y, z;
};
struct CRAZY_BAKERY_API MyMatrix3x3
{
	float mat[3][3];
	MyFloat3 operator *(const MyFloat3& v) const
	{
		MyFloat3 Result;
		Result.x = mat[0][0] * v.x + mat[0][1] * v.y + mat[0][2] * v.z;
		Result.y = mat[1][0] * v.x + mat[1][1] * v.y + mat[1][2] * v.z;
		Result.z = mat[2][0] * v.x + mat[2][1] * v.y + mat[2][2] * v.z;
		return Result;
	}
};
struct CRAZY_BAKERY_API MyMatrix4x4
{
	float mat[4][4];
	MyFloat3 operator *(const MyFloat3& v) const
	{
		MyFloat3 Result;
		Result.x = mat[0][0] * v.x + mat[0][1] * v.y + mat[0][2] * v.z + mat[0][3];
		Result.y = mat[1][0] * v.x + mat[1][1] * v.y + mat[1][2] * v.z + mat[1][3];
		Result.z = mat[2][0] * v.x + mat[2][1] * v.y + mat[2][2] * v.z + mat[2][3];
		return Result;
	}
};


/*
一个IMesh对象对应一个模型
**/
class CRAZY_BAKERY_API IMesh
{
public:
	IMesh();
	virtual ~IMesh();

#ifdef VertexAttributeUseDifferentIndex
	bool ResizeBuffers(size_t numVertices, size_t numNormals, size_t numUVs, size_t numUV2s, size_t numFaces);
#else
	bool ResizeBuffers(size_t numVertices, size_t numFaces);
#endif
	bool ResizeSubMeshes(size_t numSubMeshes);

	size_t m_numVertex = 0;
	MyFloat3* m_pVertexPosition = NULL;
	MyFloat3* m_pVertexNormal = NULL;
	MyFloat2* m_pVertexUV = NULL;
	MyFloat2* m_pVertexUV2 = NULL;

	size_t m_numFaces = 0;//下面缓冲区的元素个数都是这个数值
	MyInt3* m_pVertexIndices = NULL;
#ifdef VertexAttributeUseDifferentIndex
	size_t m_numNormal = 0;
//	size_t m_numUV = 0;
	size_t m_numUV2 = 0;

	MyInt3* m_pVertexNormalIndex = NULL;
//	MyInt3* m_pVertexUVIndex = NULL;
	MyInt3* m_pVertexUV2Index = NULL;
#endif
	MyFloat3* _pFaceNormals = NULL;//每个元素是某个面的法线，加载时计算出来的

	class ISubMesh
	{
	public:
		//下面两个变量都是对m_pVertexIndices、m_pVertexNormalIndex、m_pVertexUVIndex
		//m_pVertexUV2Index、_pFaceNormals的索引，索引值完全一样
		size_t m_numOffset = 0;//单位是MyInt3
		size_t m_numFaces = 0;//单位是MyInt3
	};

	size_t m_numSubMesh = 0;
	ISubMesh* m_pSubMeshes = NULL;

private:
};


class CRAZY_BAKERY_API ITexture
{
private:

	//static std::map<std::wstring,CTexture*>* m_vTextures;
};

//暂时没有用到
class CRAZY_BAKERY_API IMaterial
{
public:
	IMaterial() {};
	virtual ~IMaterial() {};

	virtual void SetName(const wchar_t* pName) = 0;
	virtual const wchar_t* GetName() const = 0;
	virtual void SetLightMode(const wchar_t* pName) = 0;
	virtual const wchar_t* GetLightMode() = 0;
	virtual void SetVS(const wchar_t* pName) = 0;
	virtual const wchar_t* GetVS() = 0;
	virtual void SetFS(const wchar_t* pName) = 0;
	virtual const wchar_t* GetFS() = 0;

	virtual void SetAlbedo(const wchar_t* pName) = 0;
	virtual const wchar_t* GetAlbedo() = 0;
	
	ITexture* m_pAlbedo = NULL;
	MyFloat3 m_vAlbedoTint = { 1.0f, 1.0f, 1.0f };	//对Albedo的偏色
	MyFloat2 m_vAlbedoOffset = { 0.0f, 0.0f };		//Albedo的偏移
	MyFloat2 m_vAlbedoScale = { 1.0f, 1.0f };			//Albedo的缩放系数
	
	virtual void SetEmissive(const wchar_t* pName) = 0;
	virtual const wchar_t* GetEmissive() = 0;
	ITexture* m_pEmissive = NULL;
	MyFloat3 m_vEmissiveTint = { 1.0f, 1.0f, 1.0f };	//对Emissive的偏色
	MyFloat2 m_vEmissiveOffset = { 0.0f, 0.0f };		//Emissive的偏移
	MyFloat2 m_vEmissiveScale = { 1.0f, 1.0f };		//Emissive的缩放系数
};

class CRAZY_BAKERY_API IEntity
{
public:
	IEntity() {}
	virtual ~IEntity() {}

	MyFloat3 m_vTranslation = { 0.0f, 0.0f, 0.0f };
	MyFloat3 m_vRotation = { 0.0f, 0.0f, 0.0f };
	MyFloat3 m_vScale = { 1.0f, 1.0f, 1.0f };
	MyMatrix4x4 m_matWorld;
	MyMatrix3x3 m_matWorld3x3;//逆矩阵的转置矩阵，保证法线在非等比缩放下也能正常
};

class CRAZY_BAKERY_API IStaticEntity : public IEntity
{
public:
	IStaticEntity();
	virtual ~IStaticEntity();

	virtual const wchar_t* GetEntityName() const = 0;
	virtual void SetEntityName(const wchar_t* pName) = 0;
	virtual void SetMesh(IMesh* pMesh) = 0;
	virtual const IMesh* GetMesh() const = 0;
	virtual const wchar_t* GetMeshName() const = 0;
	virtual void SetMeshName(const wchar_t* pName) = 0;

	virtual void AddMaterialName(const wchar_t* pName) = 0;
	virtual void AddMaterial(IMaterial* pMaterial) = 0;
	virtual void SetMaterial(size_t iIndex, IMaterial* pMaterial) = 0;
	virtual const IMaterial* GetMaterial(size_t iIndex) const = 0;
	virtual const wchar_t* GetMaterialName(size_t iIndex) const = 0;
//	virtual void SetMaterialName(const wchar_t* pName) = 0;

	int m_idGlobal = 0;//全场景下的序号。TODO 必须和加速结构里的完全一致
	int m_idLocal = 0;//Lightmap里的序号

	size_t m_iLightmap = 0;
	MyFloat2 m_vMapOffset = { 0.0f, 0.0f };	//在Lightmap里的偏移量，值域是0-1
	MyFloat2 m_vMapScale = { 1.0f, 1.0f };	//在Lightmap里的缩放值，值域是0-1

private:
};


class CRAZY_BAKERY_API ILight : public IEntity
{
public:
	ILight() {}
	virtual ~ILight() {}

	enum ELightType
	{
		Point = 0,
		Spot,
		Directional,
		//		Area		//例如天光，夜晚的广告牌灯箱
	};
	enum ELightStage
	{
		Static = 0,	//完全静态，直接光会烘焙进Lightmap
		Dynamic,	//完全动态，不会参与Lightmap的烘焙
		Stationary	//会参与Lightmap的烘焙，但是直接光不会烘焙进Lightmap，而是实时运算
	};

	ELightType m_eType = Directional;
	ELightStage m_eStage = Stationary;
	MyFloat3 m_vColor;
};
class CRAZY_BAKERY_API IDirectionalLight : public ILight
{
public:
	IDirectionalLight()
	{
		m_eType = Directional;
	}
	virtual ~IDirectionalLight() {}

	MyFloat3 m_vDirection;
};
class CRAZY_BAKERY_API IPointLight : public ILight
{
public:
	IPointLight()
	{
		m_eType = Point;
	}
	virtual ~IPointLight() {}
};
class CRAZY_BAKERY_API ISpotLight : public ILight
{
public:
	ISpotLight()
	{
		m_eType = Spot;
	}
	virtual ~ISpotLight() {}

	float m_cosFalloff;
	float m_cosCutoff;
	MyFloat3 m_direction;
};
class CRAZY_BAKERY_API ILineLight : public ILight//暂不考虑实现
{
public:
};
//class CAreaLight : public CLight//暂不考虑实现
//{
//public:
//};


/*
** IScatterer class is used to store baking intermediate in overture and beauty pass.
** It is a global buffer which can be reused between different batches.
*/
class CRAZY_BAKERY_API IScatterer
{
public:
	IScatterer() { }
	virtual ~IScatterer()
	{
	}

	virtual void SetName(const wchar_t* strName) = 0;
	virtual const wchar_t* GetName() const = 0;

	virtual void SetOutputPath(const wchar_t* strName) = 0;
	virtual const wchar_t* GetOutputPath() const = 0;

	virtual void Pushback(IStaticEntity* pEntity) = 0;
	virtual size_t GetEntityNumber() const = 0;
	virtual IStaticEntity* GetEntity(size_t iIndex) const = 0;

	virtual void ResetAlbedoBuffer(size_t numWidth, size_t numHeight) = 0;
	virtual size_t GetAlbedoBufferWidth() const = 0;
	virtual size_t GetAlbedoBufferHeight() const = 0;
	virtual MyFloat3* GetAlbedoBuffer() const = 0;

	size_t GetWidth(bool boBeauty) const { return boBeauty ? m_texWidth : m_fOvertureScaleFactor * m_texWidth; }
	size_t GetHeight(bool boBeauty) const { return boBeauty ? m_texHeight : m_fOvertureScaleFactor * m_texHeight; }

	int m_idGlobal;
	float m_fOvertureScaleFactor = 1.0f;
	int m_texWidth;//Lightmap的分辨率，来自于json文件
	int m_texHeight;//Lightmap的分辨率，来自于json文件
	int m_spreadRadius;//向外“晕染”几个像素，来自于json文件

	//烘焙前需要将场景里的物体分类，每一个Lightmap只能存储某种下面两个参数的组合。例如，对于场景里的普通物体，m_boGenGI和m_boBlockRay都是true
	//而对于环境类型的物体，分别是false和true
	bool m_boGenGI = false;//是否产生Lightmap
	bool m_boBlockRay = false;//烘焙时是否遮挡射线
};

class CRAZY_BAKERY_API IScene
{
public:
	IScene() {}
	virtual ~IScene() {}

	//Scene path是场景文件的绝对路径，用于在ICrazyBakery里管理场景，
	//注意：没有留出外部设置的接口（只允许在CCrazyBakeryImpl::GetOrCreateScene()里设置），因为那样会导致场景管理出错
	virtual const wchar_t* GetPath() const = 0;

	//Scene name暂时没有用到
	virtual void SetName(const wchar_t* strName) = 0;
	virtual const wchar_t* GetName() const = 0;

	virtual IMesh* CreateMesh(const wchar_t* strName) = 0;
	virtual IMesh* GetMesh(const wchar_t* strName) = 0;

	virtual IMaterial* CreateMaterial(const wchar_t* strName) = 0;
	virtual IMaterial* GetMaterial(const wchar_t* strName) = 0;

	virtual IScatterer* AddScatterer() = 0;
	virtual size_t GetScattererNumber() const = 0;
	virtual const IScatterer* GetScatterer(size_t iIndex) const = 0;
	
	virtual IStaticEntity* CreateStaticEntity() = 0;
	virtual void Pushback(IStaticEntity* pEntity) = 0;
	virtual size_t GetEntityNumber(size_t iScatterer) const = 0;
	virtual size_t GetEntityNumber() const { return 0; }
	virtual IStaticEntity* GetEntity(size_t iLightmap, size_t idEntity) const  = 0;//idEntity是Entity在Lightmap内部的索引
	virtual IStaticEntity* GetEntityByGlobalID(size_t idGlobal) const = 0;
	
	virtual ILight* CreateLight(ILight::ELightType eType) = 0;
	virtual size_t GetLightNum() const = 0;
	virtual ILight* GetLight(size_t iIndex) const = 0;

//	wchar_t m_strOutputPath[256] = { 0 };
};

/*
** This is the option you must provide to baker at the very beginning.
*/
class CRAZY_BAKERY_API IBakeOption
{
public:
	IBakeOption() {};
	virtual ~IBakeOption() {};

public:
	virtual void SetSampleNumbersInOverture(size_t numIterators, size_t* pNumbers) = 0;
	//iIterator表示间接光照的索引
	virtual size_t GetSampleNumberInOverture(size_t iIterator) const = 0;
	virtual size_t GetIndirectIterateNumbersInOverture() const = 0;

	virtual void SetSampleNumbersInBeauty(size_t numSamples) = 0;
	virtual size_t GetSampleNumbersInBeauty() const = 0;

public:
//	MyFloat2 Resolution = { 512,512 };//输出的所有Lightmap的分辨率

//	int m_numSamplesPerTexelInOverture = 64;

	/*
	** It determines the number of bounces a ray can do before being terminated.
	** Increasing the number will bring you better global illumination effect,
	** while decreasing it will save you a lot of time. It is your responsiblity
	** to make a balance. Setting bounce number to 0 will close indirect lighting.
	** Range [0, inf)
	*/
	//Overture阶段光的反射次数，第一次是直接光，第二次开始是间接光。目前发现计算3次间接光
	//和2次对最终结果几乎没有差别，当然也有可能是当前测试场景的灯光较暗，或者材质反射不明显导致的。
//	bool m_boDirectLighting = true;
	/*
	** The naively ray traced irradiance values in light map is full of noise,
	** therefore we need to smooth it out using irradiance filter. The radius
	** control the how many neighbor texels are considered. To be exact, filter
	** will interpolate (radius*2+1)*(radius*2+1) texels to get the filtered
	** value for center texel.
	** Range [0, inf)
	*/
	int filterRadius = 5;

//	bool needDirectLight = true;

	bool m_boDumpOvertureDirectLight = false;
	bool m_boDumpOvertureIndirectLight = false;
//	bool m_boDumpObj = false;
//	bool m_boOutputGammaTexture = true;
};

//最终烘焙的结果
class CRAZY_BAKERY_API ILightmap
{
public:
	ILightmap() {};
	virtual ~ILightmap() {}

	virtual void SetOutputPath(const wchar_t* strName) = 0;
	virtual const wchar_t* GetOutputPath() const = 0;

	virtual void ResetBuffer() = 0;
	virtual MyFloat4* GetBuffer() const = 0;

	int m_texWidth;
	int m_texHeight;
};

class CRAZY_BAKERY_API ITimer
{
public:
	virtual void Reset() = 0;//重置计时器（总时间会清空）
	virtual void Pause() = 0;//暂停计时器计时
	virtual void Resume() = 0;//重启计时器计时
	virtual double GetElapsedTime() = 0;//总共计时时间（不含暂停期间的时间）
};

class CRAZY_BAKERY_API ICrazyBakery
{
public:
	ICrazyBakery() {}
	virtual ~ICrazyBakery() {}

	//初始化系统，例如cuda和optix等
	virtual bool Init() { return false; }

	//下面几个创建函数首先查找有没有指定名字的对象，没有就新创建一个
	//下面几个释放函数用于手动释放，也可以不释放，随着ICrazyBakery自动释放
	virtual bool SceneExist(const wchar_t* strPath = L"") = 0;
	virtual IScene* GetOrCreateScene(const wchar_t* strPath = L"") = 0;
	virtual void ReleaseScene(const wchar_t* strPath = L"") = 0;

	virtual IBakeOption* GetBakeOption(const wchar_t* strName = L"") = 0;
	virtual void ReleaseBakeOption(const wchar_t* strName = L"") = 0;

	virtual ILightmap* GetLightmapData(const size_t iIndex = 0) = 0;
	virtual void ReleaseLightmapData(const size_t iIndex = 0) = 0;

	virtual ITimer* GetTimer(const wchar_t* strName = L"") = 0;
	virtual void ReleaseTimer(const wchar_t* strName = L"") = 0;

	//TODO 增加两个函数分别只用于生成场景的碰撞体和光栅化Lightmap，
	//这样，当场景物体个数、缩放系数等不改变时，再次烘焙（例如仅仅灯光和烘焙条件改变了）就不用重新生成加速结构体了。
	//如果灯光改变了，而场景物体和影响分辨率改变的烘焙参数没有发生变化，这样就不用重复光栅化了
	virtual bool Bake(const IScene* pScene, const IBakeOption* pOption, ILightmap** ppLightmapDatas, size_t numLightmapDatas) = 0;
	virtual bool Close() = 0;
};

CRAZY_BAKERY_API ICrazyBakery* CreateCrazyBakery();

}

//Dump函数，所有的实现都不在当前模块里，由外部模块实现，因为希望这个模块尽量精简，不希望搞成大杂脍
namespace CrazyBakery
{
//	void CRAZY_BAKERY_API DumpTriangle(const std::vector<float3>& p, const wchar_t* strPath);
//	void CRAZY_BAKERY_API DumpScatterer(const Scatterer * scatterer, const wchar_t* strPath, const wchar_t* texture = L"");
//	void CRAZY_BAKERY_API DumpScatterer(const std::vector<const Scatterer*>& scatterer, const wchar_t* strPath);
	//用于将整个场景输出成一个通用的模型文件
	typedef void CRAZY_BAKERY_API (*pDumpMergedScatterer)(const IScene* pScene, const wchar_t* strTextureType);
	extern CRAZY_BAKERY_API pDumpMergedScatterer g_pDumpMergedScatterer;
	//void CRAZY_BAKERY_API DumpMergedScatterer(const CScene* pScene, const wchar_t* strPath, const wchar_t* strTextureType);
//	void CRAZY_BAKERY_API DumpRay(const Ray *ray, const Hit *hit, const int n, const wchar_t* strPath);
//	void CRAZY_BAKERY_API DumpOptixRay(const Ray *devRay, const Hit *devHit, const int n, const wchar_t* strPath);
//	void DumpGrid(const Grid<CellInt>& grid, const std::wstring & path);
	//用于将纹理输出成文件
	typedef void CRAZY_BAKERY_API (*pDumpBuffer)(size_t numWidth, size_t numHeight, MyFloat3* pData, const wchar_t* StageName, int iLightmap, const wchar_t* strName, bool boFloat);
	extern CRAZY_BAKERY_API pDumpBuffer g_pDumpBuffer;
//	void CRAZY_BAKERY_API DumpBuffer(size_t numWidth, size_t numHeight, MyFloat3* pData, const wchar_t* strPath, const wchar_t* StageName, int iLightmap, const wchar_t* strName, bool boFloat = false);
//	void CRAZY_BAKERY_API DumpBuffer(const std::vector<Batch::Output>& devBuffer, const wchar_t* strPath, const wchar_t* StageName, bool boFloat = false);
//	void CRAZY_BAKERY_API DumpOptixImage(const Buffer<float3> &devImage, const int w, const int h, const wchar_t* strPath);
//	void CRAZY_BAKERY_API DumpOptixImage(const Buffer<float4> &devImage, const int w, const int h, const wchar_t* strPath);

	//用于输出信息
	enum EMessageType
	{
		MT_Info,
		MT_Debug,
		MT_Warnning,
		MT_Error,
		MT_Fatal
	};
	typedef void CRAZY_BAKERY_API(*pProcessMessage)(EMessageType eType, const wchar_t* strMessage);
	extern CRAZY_BAKERY_API pProcessMessage g_pProcessMessage;
}


//全局函数
namespace CrazyBakery
{
	void CRAZY_BAKERY_API MakeMatrix(MyMatrix3x3* pMat, const MyFloat3* pScale, const MyFloat3* pRot);
	void CRAZY_BAKERY_API MakeMatrix(MyMatrix4x4* pMat, const MyFloat3* pScale, const MyFloat3* pRot, const MyFloat3* pTrans);
}