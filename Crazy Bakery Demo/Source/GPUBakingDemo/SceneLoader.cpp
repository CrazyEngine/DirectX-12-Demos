#include "stdafx.h"
#include "SceneLoader.h"

#include <vector>
#include <fstream>

#include "json/json.h"

#include "GPUBake.h"
#include "String.h"
using namespace CrazyBakery;

inline void ProcessScatterer(const Json::Value &jsonVal, IScene* pScene)
{
	int idEntity = 0;
	for (int iScatterer = 0; iScatterer < jsonVal.size(); iScatterer++)//All scatterer
	{
		IScatterer* pScatterer = pScene->AddScatterer();
		pScatterer->m_idGlobal = iScatterer;

		const Json::Value jsonScatterer = jsonVal[iScatterer];

		const Json::Value jsonScattererName = jsonScatterer["ScattererName"];
		pScatterer->SetName(StringToWString(jsonScattererName.asString()).c_str());

		std::wstring parent = pScene->m_strOutputPath; // GetParent(path);
		std::wstring path = parent + pScatterer->GetName() + L".png";
		pScatterer->SetOutputPath(path.c_str());

		const Json::Value jsonTextureWidth = jsonScatterer["TextureWidth"];
		pScatterer->m_texWidth = jsonTextureWidth.asInt();
		
		const Json::Value jsonTextureHeight = jsonScatterer["TextureHeight"];
		pScatterer->m_texHeight = jsonTextureHeight.asInt();

		const Json::Value jsonSpreadRadius = jsonScatterer["SpreadRadius"];
		pScatterer->m_spreadRadius = jsonSpreadRadius.asInt();

		const Json::Value jsonGenGI = jsonScatterer["GenGI"];
		if (jsonGenGI != Json::Value::nullSingleton())
			pScatterer->m_boGenGI = jsonGenGI.asBool();

		const Json::Value jsonBlockRay = jsonScatterer["BlockRay"];
		if (jsonBlockRay != Json::Value::nullSingleton())
			pScatterer->m_boBlockRay = jsonBlockRay.asBool();

		const Json::Value jsonStaticMeshs = jsonScatterer["StaticMeshs"];
		for (int i = 0; i < jsonStaticMeshs.size(); i++)
		{
			const Json::Value jsonStaticMesh = jsonStaticMeshs[i];
			IStaticEntity* pEntity = pScene->CreateStaticEntity();
			pEntity->m_idGlobal = idEntity;
			pEntity->m_idLocal = i;
			++idEntity;

			const Json::Value jEntityNameVal = jsonStaticMesh["EntityName"];
			if (jEntityNameVal != Json::Value::nullSingleton())
			{
				std::wstring strjEntityNameVal = /*bake::*/StringToWString(jEntityNameVal.asString());
				pEntity->SetEntityName(strjEntityNameVal.c_str());
			}
			else
			{
				MessageBoxW(NULL,L"找不到EntityName",L"",MB_OK);
				continue;
			}

			const Json::Value jMeshVal = jsonStaticMesh["MeshName"];
			if (jMeshVal != Json::Value::nullSingleton())
			{
				std::wstring strMeshName = /*bake::*/StringToWString(jMeshVal.asString());
				pEntity->SetMeshName(strMeshName.c_str());
			}
			else
			{
				MessageBoxW(NULL, L"找不到MeshName", L"", MB_OK);
				continue;
			}

			const Json::Value jMaterialsVal = jsonStaticMesh["Materials"];
			if (jMaterialsVal != Json::Value::nullSingleton())
			{
				for (int iMaterial = 0; iMaterial < jMaterialsVal.size(); iMaterial++)//All Material
				{
					const Json::Value jMaterialVal = jMaterialsVal[iMaterial];
					std::wstring strMaterialName = StringToWString(jMaterialVal["MaterialName"].asString());
					pEntity->AddMaterialName(strMaterialName.c_str());
				}
			}
			else
			{
				MessageBoxW(NULL, L"找不到Materials", L"", MB_OK);
				continue;
			}

			const Json::Value jRotationVal = jsonStaticMesh["Rotation"];
			if (jRotationVal != Json::Value::nullSingleton())
			{
				pEntity->m_vRotation = MyFloat3
				{
					jRotationVal[0].asFloat(),
					jRotationVal[1].asFloat(),
					jRotationVal[2].asFloat()
				};
			}
			else
			{
				pEntity->m_vRotation = MyFloat3{ 0.0f,0.0f,0.0f };
			}


			const Json::Value jTranslationVal = jsonStaticMesh["Translation"];
			if (jTranslationVal != Json::Value::nullSingleton())
			{
				pEntity->m_vTranslation = MyFloat3
				{
					jTranslationVal[0].asFloat(),
					jTranslationVal[1].asFloat(),
					jTranslationVal[2].asFloat()
				};
			}
			else
			{
				pEntity->m_vTranslation = MyFloat3{ 0.0f,0.0f,0.0f };
			}

			const Json::Value jScaleVal = jsonStaticMesh["Scale"];
			if (jScaleVal != Json::Value::nullSingleton())
			{
				pEntity->m_vScale = MyFloat3
				{
					jScaleVal[0].asFloat(),
					jScaleVal[1].asFloat(),
					jScaleVal[2].asFloat()
				};
			}
			else
			{
				pEntity->m_vScale = MyFloat3{ 1.0f,1.0f,1.0f };
			}
			
			MakeMatrix(&pEntity->m_matWorld, &pEntity->m_vScale, &pEntity->m_vRotation, &pEntity->m_vTranslation);
			MakeMatrix(&pEntity->m_matWorld3x3, &pEntity->m_vScale, &pEntity->m_vRotation);

			pEntity->m_iLightmap = iScatterer;

			const Json::Value jLightmapOffsetVal = jsonStaticMesh["LightmapOffset"];
			if (jLightmapOffsetVal != Json::Value::nullSingleton())
			{
				pEntity->m_vMapOffset = MyFloat2
				{
					jLightmapOffsetVal[0].asFloat(),
					jLightmapOffsetVal[1].asFloat()
				};
			}
			else
			{
				pEntity->m_vMapOffset = MyFloat2{ 0.0f,0.0f };
			}

			const Json::Value jLightmapScaleVal = jsonStaticMesh["LightmapScale"];
			if (jLightmapScaleVal != Json::Value::nullSingleton())
			{
				pEntity->m_vMapScale = MyFloat2
				{
					jLightmapScaleVal[0].asFloat(),
					jLightmapScaleVal[1].asFloat()
				};
			}
			else
			{
				pEntity->m_vMapScale = MyFloat2{ 1.0f,1.0f };
			}

			pScene->Pushback(pEntity);
		}
	}

	//DumpScatterer(&scatterer[0], L"e:\\scatterer.obj");
}

MyFloat3 normalize(const MyFloat3& v)
{
	MyFloat3 vRes;
	float fLength = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	vRes.x = v.x/fLength;
	vRes.y = v.y/fLength;
	vRes.z = v.z/fLength;
	return vRes;
}
#define M_PI       3.14159265358979323846   // pi
inline void ProcessEmitter(const Json::Value &jsonVal, IScene* pScene)
{
	for (int i = 0; i < jsonVal.size(); i++)
	{
		const Json::Value type = jsonVal[i]["Type"];
		if (type == Json::Value::nullSingleton())
			continue;

		ILight::ELightStage eLightStage;
		const Json::Value Stage = jsonVal[i]["Stage"];
		if (Stage.asString() == "Static" )
		{
			eLightStage = ILight::ELightStage::Static;
		} 
		else if (Stage.asString() == "Dynamic")
		{
			eLightStage = ILight::ELightStage::Dynamic;
		}
		else if (Stage.asString() == "Stationary")
		{
			eLightStage = ILight::ELightStage::Stationary;
		}
		else
		{
			MessageBoxW(NULL, L"Undefined Stage", L"", MB_OK);
		}

		if (type.asString() == "Directional")
		{
			const Json::Value jsonDirection = jsonVal[i]["Direction"];
			const Json::Value jsonColor = jsonVal[i]["Color"];

			if (jsonDirection == Json::Value::nullSingleton())
				continue;
			if (jsonColor == Json::Value::nullSingleton())
				continue;

			IDirectionalLight* pLight = new IDirectionalLight;
			pLight->m_eStage = eLightStage;
			pLight->m_vDirection = MyFloat3
			{
				jsonDirection[0].asFloat(),
				jsonDirection[1].asFloat(),
				jsonDirection[2].asFloat()
			};
			pLight->m_vDirection = normalize(pLight->m_vDirection);//Gitee上这里对应的代码有bug：应该改成e.m_direction = normalize(e.m_direction);
			pLight->m_vColor = MyFloat3
			{
				jsonColor[0].asFloat(),
				jsonColor[1].asFloat(),
				jsonColor[2].asFloat()
			};

			pScene->Pushback(pLight);
		}
		else if (type.asString() == "Point")
		{
			const Json::Value jsonPosition = jsonVal[i]["Position"];
			const Json::Value jsonColor = jsonVal[i]["Color"];

			if (jsonPosition == Json::Value::nullSingleton())
				continue;
			if (jsonColor == Json::Value::nullSingleton())
				continue;

			IPointLight* pLight = new IPointLight;
			pLight->m_eStage = eLightStage;
			pLight->m_vTranslation = MyFloat3
			{
				jsonPosition[0].asFloat(),
				jsonPosition[1].asFloat(),
				jsonPosition[2].asFloat()
			};
			pLight->m_vColor = MyFloat3
			{
				jsonColor[0].asFloat(),
				jsonColor[1].asFloat(),
				jsonColor[2].asFloat()
			};

			pScene->Pushback(pLight);
		}
		else if (type.asString() == "Spot")
		{
			const Json::Value jsonPosition = jsonVal[i]["Position"];
			const Json::Value jsonColor = jsonVal[i]["Color"];
			const Json::Value jsonCutoff = jsonVal[i]["Cutoff"];
			const Json::Value jsonFalloff = jsonVal[i]["Falloff"];
			const Json::Value jsonDirection = jsonVal[i]["Direction"];

			if (jsonPosition == Json::Value::nullSingleton())
				continue;
			if (jsonColor == Json::Value::nullSingleton())
				continue;
			if (jsonCutoff == Json::Value::nullSingleton())
				continue;
			if (jsonFalloff == Json::Value::nullSingleton())
				continue;
			if (jsonDirection == Json::Value::nullSingleton())
				continue;

			ISpotLight* pLight = new ISpotLight;
			pLight->m_eStage = eLightStage;
			pLight->m_vTranslation = MyFloat3
			{
				jsonPosition[0].asFloat(),
				jsonPosition[1].asFloat(),
				jsonPosition[2].asFloat()
			};
			pLight->m_vColor = MyFloat3
			{
				jsonColor[0].asFloat(),
				jsonColor[1].asFloat(),
				jsonColor[2].asFloat()
			};
			pLight->m_cosCutoff = cos(jsonCutoff.asFloat() * M_PI / 180.f);
			pLight->m_cosFalloff = cos(jsonFalloff.asFloat() * M_PI / 180.f);
			pLight->m_direction = MyFloat3
			{
				jsonDirection[0].asFloat(),
				jsonDirection[1].asFloat(),
				jsonDirection[2].asFloat()
			};
			pLight->m_direction = normalize(pLight->m_direction);

			pScene->Pushback(pLight);
		}
	}
}

/** cross product */
MyFloat3 cross(const MyFloat3& a, const MyFloat3& b)
{
	return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
}
float dot(MyFloat3 a, MyFloat3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline MyFloat3 ComputeFaceNormal(
	const MyFloat3 &p0,
	const MyFloat3 &p1,
	const MyFloat3 &p2,
	const MyFloat3 &n0,
	const MyFloat3 &n1,
	const MyFloat3 &n2
)
{
	//求三个顶点法线的平均值
	MyFloat3 n = n0;
	n += n1;
	n += n2;
	MyFloat3 s = normalize(n);

	//计算面法线
	MyFloat3 pv1 = p0;
	pv1 -= p2;
	MyFloat3 pv2 = p1;
	pv2 -= p2;
	MyFloat3 f = normalize(cross(pv1, pv2));

	//如果两个法线方向相反，翻转面法线。什么情况下会发生呢？
	if (dot(s, f) > 0.f)
		return f;
	else
		return -f;
}
extern std::wstring g_strAssetPath;

#ifdef VertexAttributeUseDifferentIndex

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
using namespace tinyobj;
bool LoadMeshUnity(const std::wstring& strPath, IMesh* pMesh)
{
	std::wstring strObjPath = g_strAssetPath + strPath;
	std::string pathOBJ = /*bake::*/WStringToString(strObjPath);
	pathOBJ += ".obj";
	const std::string pathMTL = /*bake::*/WStringToString(/*bake::*/GetParent(strObjPath));
	attrib_t attrib;//所有的顶点属性数据
	std::vector<shape_t> shapes;//obj里所有的g
	std::vector<material_t> materials;//obj里所有的材质
	
	std::string warn, err;
	//调用下面的函数时，必须提供obj所在的文件夹的路径，否则加载材质会失败，这样所有的面的材质ID都会是-1
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathOBJ.c_str(), pathMTL.c_str());

	if (!ret)
		return -1;

	/*pMesh->m_numVertex = attrib.vertices.size()/3;
	pMesh->m_pVertexPosition = new MyFloat3[pMesh->m_numVertex];
	memcpy(pMesh->m_pVertexPosition, &attrib.vertices[0], pMesh->m_numVertex *sizeof(MyFloat3));

	pMesh->m_numNormal = attrib.normals.size()/3;
	pMesh->m_pVertexNormal = new MyFloat3[pMesh->m_numNormal];
	memcpy(pMesh->m_pVertexNormal, &attrib.normals[0], pMesh->m_numNormal * sizeof(MyFloat3));

	pMesh->m_numUV = attrib.texcoords.size()/2;
	pMesh->m_pVertexUV2 = new MyFloat2[pMesh->m_numUV];
	memcpy(pMesh->m_pVertexUV2, &attrib.texcoords[0], pMesh->m_numUV * sizeof(MyFloat2));*/

	//David: TODO 加UV2

	//根据材质分组顶点
	std::vector< std::map<int, std::vector<int>> > GeometryGroup;//first保存的是材质索引号，second是使用某种材质的所有的面的三个索引
	GeometryGroup.resize(shapes.size());
	size_t numMaterial = 0;
	size_t numFaces = 0;
	for (size_t iGeometry = 0; iGeometry < shapes.size(); iGeometry++)//遍历obj里所有的g
	{
		std::map<int, std::vector<int>>& Geometry = GeometryGroup[iGeometry];
		numFaces += shapes[iGeometry].mesh.material_ids.size();//mesh_t::material_ids里存储的每个元素是某个面的材质索引

		//将某个g里的所有面根据材质进行分组
		std::map<int, std::vector<int>> mg;
		for (int mi = 0; mi < shapes[iGeometry].mesh.material_ids.size(); mi++)
		{
			const int mid = shapes[iGeometry].mesh.material_ids[mi];
			Geometry[mid].push_back(mi * 3);
			Geometry[mid].push_back(mi * 3 + 1);
			Geometry[mid].push_back(mi * 3 + 2);
		}
		//为了保证obj里的submesh顺序和场景文件里材质顺序完全一致（因为
		//map里的顺序是材质索引号，很可能跟场景文件里设置的材质顺序是冲突的），
		//规定一个g里只允许有一个材质，每一个g对应一个submesh
		if (1 != Geometry.size())
		{
			MessageBoxW(NULL, L"In obj file, one g only allows for one material!", L"", MB_OK);
		}

		numMaterial += Geometry.size();
	}
	
	//拷贝顶点缓冲区
	size_t numVertex = attrib.vertices.size() / 3;
	size_t numNormal = attrib.normals.size() / 3;
	size_t numUVs = attrib.texcoords.size() / 2;
	pMesh->ResizeBuffers(numVertex, numNormal, numUVs, numUVs, numFaces);
	memcpy(pMesh->m_pVertexPosition, &attrib.vertices[0], numVertex * sizeof(MyFloat3));
	memcpy(pMesh->m_pVertexNormal, &attrib.normals[0], numNormal * sizeof(MyFloat3));
	memcpy(pMesh->m_pVertexUV2, &attrib.texcoords[0], numUVs * sizeof(MyFloat2));

	//调整SubMesh的个数
	pMesh->ResizeSubMeshes(numMaterial);

	/*int numFaces = pMesh->m_numFaces;
	pMesh->m_pVertexIndices = new MyInt3[numFaces];
	pMesh->m_pVertexNormalIndex = new MyInt3[numFaces];
	pMesh->m_pVertexUVIndex = new MyInt3[numFaces];
	pMesh->_pFaceNormals = new MyFloat3[numFaces];
	pMesh->m_numSubMesh = numMaterial;
	pMesh->m_pSubMeshes = new IMesh::ISubMesh[numMaterial];*/

	int numOffset = 0;
	for (size_t iGeometry = 0; iGeometry < shapes.size(); iGeometry++)//遍历obj里所有的g
	{
		std::map<int, std::vector<int>>& Geometry = GeometryGroup[iGeometry];
		std::map<int, std::vector<int>>::iterator it = Geometry.begin();
		for (size_t i=0; it != Geometry.end(); it++, ++i)//遍历g里所有的材质，目前只允许有一个
		{
			IMesh::ISubMesh& cSubMesh = pMesh->m_pSubMeshes[iGeometry];
			cSubMesh.m_numOffset = numOffset;
			cSubMesh.m_numFaces = it->second.size()/3;

			const std::vector<index_t>& Index = shapes[iGeometry].mesh.indices;
			const std::vector<int>& iVerts = it->second;
			for (int vi = 0; vi < iVerts.size(); vi += 3)//遍历使用某材质的所有的面
			{
				//mesh_t::indices包含了所有面的所有顶点的索引
				const MyInt3 pid = MyInt3{
					Index[iVerts[vi + 0]].vertex_index,
					Index[iVerts[vi + 1]].vertex_index,
					Index[iVerts[vi + 2]].vertex_index };
				const MyInt3 nid = MyInt3{
					Index[iVerts[vi + 0]].normal_index,
					Index[iVerts[vi + 1]].normal_index,
					Index[iVerts[vi + 2]].normal_index };
				const MyInt3 uid = MyInt3{
					Index[iVerts[vi + 0]].texcoord_index,
					Index[iVerts[vi + 1]].texcoord_index,
					Index[iVerts[vi + 2]].texcoord_index };
				memcpy(pMesh->m_pVertexIndices + numOffset,&pid,sizeof(MyInt3));
				memcpy(pMesh->m_pVertexNormalIndex + numOffset, &nid, sizeof(MyInt3));
				memcpy(pMesh->m_pVertexUV2Index + numOffset, &uid, sizeof(MyInt3));

				//David: TODO 加UV2

				//计算模型空间的面法线
				MyFloat3 vFaceNormal = ComputeFaceNormal(
					pMesh->m_pVertexPosition[pid.x],
					pMesh->m_pVertexPosition[pid.y],
					pMesh->m_pVertexPosition[pid.z],
					pMesh->m_pVertexNormal[nid.x],
					pMesh->m_pVertexNormal[nid.y],
					pMesh->m_pVertexNormal[nid.z]
				);
				memcpy(pMesh->_pFaceNormals + numOffset, &vFaceNormal, sizeof(MyFloat3));

				++numOffset;
			}//end //遍历使用某材质的所有的面
		}//遍历所有的材质结束
	}

	return true;
}

#else

#include "LoadM3d.h"
bool LoadM3D(const std::wstring& strPath, IMesh* pMesh)
{
	std::vector<M3DLoader::Subset> mSubsets;//David：只在LoadSkinnedModel()里面加载模型时使用，对应的是m3d模型里的SubsetTable
	std::vector<M3DLoader::M3dMaterial> mMats;//David：在LoadSkinnedModel()里面加载模型时会使用，对应的是m3d模型里的Materials
	std::vector<M3DLoader::Vertex> vertices;
	std::vector<std::uint16_t> indices;//所有三角形的顶点索引

	//Load model data from .m3d file
	M3DLoader m3dLoader;
	std::wstring strModelFilenameW = g_strAssetPath + strPath;
	std::string strModelFilename = WStringToString(strModelFilenameW);
	strModelFilename += ".m3d";
	m3dLoader.LoadM3d(strModelFilename, vertices, indices, mSubsets, mMats);

	//初始化各种缓冲区
	size_t numVertex = vertices.size();
	size_t numFaces = indices.size() / 3;
	size_t numSubMeshes = mSubsets.size();
	pMesh->ResizeBuffers(numVertex, numFaces);

	//拷贝顶点缓冲区
	for (size_t i = 0; i < numVertex; ++i)
	{
		memcpy(pMesh->m_pVertexPosition + i, &vertices[i].Pos, sizeof(MyFloat3));
		memcpy(pMesh->m_pVertexNormal + i, &vertices[i].Normal, sizeof(MyFloat3));
		memcpy(pMesh->m_pVertexUV2 + i, &vertices[i].TexC, sizeof(MyFloat2));
	}

	//拷贝索引缓冲区。注意：不能用memcpy直接拷贝，因为M3DLoader返回的索引indices是unsigned short类型的，而optix要使用int
	for (size_t i = 0; i < numFaces; ++i)
	{
		pMesh->m_pVertexIndices[i].x = indices[3 * i + 0];
		pMesh->m_pVertexIndices[i].y = indices[3 * i + 1];
		pMesh->m_pVertexIndices[i].z = indices[3 * i + 2];
	}

	//计算面法线
	for (size_t iFace = 0; iFace < numFaces; ++iFace)
	{
		std::uint16_t iFirst = indices[3 * iFace + 0];
		std::uint16_t iSecond = indices[3 * iFace + 1];
		std::uint16_t iThird = indices[3 * iFace + 2];
		MyFloat3 vFaceNormal = ComputeFaceNormal(
			pMesh->m_pVertexPosition[iFirst],
			pMesh->m_pVertexPosition[iSecond],
			pMesh->m_pVertexPosition[iThird],
			pMesh->m_pVertexNormal[iFirst],
			pMesh->m_pVertexNormal[iSecond],
			pMesh->m_pVertexNormal[iThird]
		);
		memcpy(pMesh->_pFaceNormals + iFace, &vFaceNormal, sizeof(MyFloat3));
	}

	//调整SubMesh的个数
	pMesh->ResizeSubMeshes(numSubMeshes);
	//设置每一个SubMesh
	for (size_t iSubMesh = 0; iSubMesh < numSubMeshes; ++iSubMesh)
	{
		IMesh::ISubMesh& cSubMesh = pMesh->m_pSubMeshes[iSubMesh];
		cSubMesh.m_numOffset = mSubsets[iSubMesh].FaceStart;
		cSubMesh.m_numFaces = (UINT)mSubsets[iSubMesh].FaceCount;
	}

	return true;
}
#endif
bool CheckFolderExist(const std::wstring &strPath)
{
	bool rValue = false;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = ::FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	::FindClose(hFind);
	return rValue;
}
bool CSceneLoader::_LoadSceneFile(const std::wstring& strPath, IScene* pScene)
{
	pScene->SetName(strPath.c_str());//场景文件的绝对路径

	//设置输出路径，并保证文件夹确实创建了
	std::wstring strOutputPath = strPath;
	size_t iPos = strOutputPath.find_last_of(L"\\");
	strOutputPath = strOutputPath.substr(0, iPos + 1);
	strOutputPath += L"Output";
	//保证输出文件夹确实创建了
	bool boRootExist = CheckFolderExist(strOutputPath.c_str());
	if (!boRootExist)
	{
		::CreateDirectory(strOutputPath.c_str(), NULL);
	}
	strOutputPath += L"\\";
	wcscpy_s(pScene->m_strOutputPath, strOutputPath.c_str());//为m_strOutputPath赋值必须在ProcessScatterer()之前执行

	std::ifstream sceneFile;
	sceneFile.open(strPath.c_str(), std::ios_base::in);

	Json::CharReaderBuilder jsonReader;
	Json::Value jsonRoot;
	std::string jsonErrs;
	const bool jsonResult = parseFromStream(jsonReader, sceneFile, &jsonRoot, &jsonErrs);
	if (!jsonResult)
	{
		return false;
	}

	const Json::Value jsonScatterer = jsonRoot["Scatterers"];
	const Json::Value jsonLightSource = jsonRoot["LightSources"];

	if (jsonScatterer != Json::Value::nullSingleton())
	{
		ProcessScatterer(jsonScatterer, pScene);
	}
	
	if (jsonLightSource != Json::Value::nullSingleton())
	{
		ProcessEmitter(jsonLightSource, pScene);
	}

	sceneFile.close();

	return true;
}

bool LoadMaterial(const std::wstring& strPath, IMaterial* pMaterial)
{
	std::wstring strAbsPath = g_strAssetPath;
	strAbsPath += strPath;
	strAbsPath += L".json";//TODO 临时
	std::ifstream sceneFile;
	sceneFile.open(strAbsPath.c_str(), std::ios_base::in);
	if (!sceneFile)
	{
		return false;
	}

	Json::CharReaderBuilder jsonReader;
	Json::Value jsonRoot;
	std::string jsonErrs;
	const bool jsonResult = parseFromStream(jsonReader, sceneFile, &jsonRoot, &jsonErrs);
	if (!jsonResult)
	{
		return false;
	}

	const Json::Value jsonPasses = jsonRoot["Passes"];
	if (jsonPasses != Json::Value::nullSingleton())
	{
		for (int iPass = 0; iPass < jsonPasses.size(); ++iPass)//All scatterer
		{
			const Json::Value jsonPass = jsonPasses[iPass];

			const Json::Value jsonPassName = jsonPass["PassName"];
			pMaterial->SetName( StringToWString(jsonPassName.asString()).c_str() );

			const Json::Value jsonLightMode = jsonPass["LightMode"];
			pMaterial->SetLightMode( StringToWString(jsonLightMode.asString()).c_str() );

			const Json::Value jsonVS = jsonPass["VS"];
			pMaterial->SetVS( StringToWString(jsonVS.asString()).c_str() );

			const Json::Value jsonFS = jsonPass["FS"];
			pMaterial->SetFS( StringToWString(jsonFS.asString()).c_str() );

			const Json::Value jsonAlbedo = jsonPass["Albedo"];
			pMaterial->SetAlbedo( StringToWString(jsonAlbedo.asString()).c_str() );

			const Json::Value jsonAlbedoTint = jsonPass["AlbedoTint"];
			pMaterial->m_vAlbedoTint = MyFloat3
			{
				jsonAlbedoTint[0].asFloat(),
				jsonAlbedoTint[1].asFloat(),
				jsonAlbedoTint[2].asFloat()
			};

			const Json::Value jsonAlbedoOffset = jsonPass["AlbedoOffset"];
			pMaterial->m_vAlbedoOffset = MyFloat2
			{
				jsonAlbedoOffset[0].asFloat(),
				jsonAlbedoOffset[1].asFloat()
			};

			const Json::Value jsonAlbedoScale = jsonPass["AlbedoScale"];
			pMaterial->m_vAlbedoScale = MyFloat2
			{
				jsonAlbedoScale[0].asFloat(),
				jsonAlbedoScale[1].asFloat()
			};


			const Json::Value jsonEmissive = jsonPass["Emissive"];
			pMaterial->SetEmissive( StringToWString(jsonEmissive.asString()).c_str() );

			const Json::Value jsonEmissiveTint = jsonPass["EmissiveTint"];
			pMaterial->m_vEmissiveTint = MyFloat3
			{
				jsonEmissiveTint[0].asFloat(),
				jsonEmissiveTint[1].asFloat(),
				jsonEmissiveTint[2].asFloat()
			};

			const Json::Value jsonEmissiveOffset = jsonPass["EmissiveOffset"];
			pMaterial->m_vEmissiveOffset = MyFloat2
			{
				jsonEmissiveOffset[0].asFloat(),
				jsonEmissiveOffset[1].asFloat()
			};

			const Json::Value jsonEmissiveScale = jsonPass["EmissiveScale"];
			pMaterial->m_vEmissiveScale = MyFloat2
			{
				jsonEmissiveScale[0].asFloat(),
				jsonEmissiveScale[1].asFloat()
			};
		}
	}


	sceneFile.close();

	return true;
}

bool CSceneLoader::_LoadSceneResource(IScene* pScene)
{
	size_t numStaticEntities = pScene->GetEntityNumber();
	for (size_t i=0; i<numStaticEntities; ++i)
	{
		IStaticEntity* pEntity = pScene->GetEntityByGlobalID(i);
		if (NULL==pEntity->GetMesh())
		{
			IMesh* pMesh = pScene->GetMesh(pEntity->GetMeshName());
			if (!pMesh)
			{
				pMesh = pScene->CreateMesh(pEntity->GetMeshName());
#ifdef VertexAttributeUseDifferentIndex
				LoadMeshUnity(pEntity->GetMeshName(), pMesh);
#else
				LoadM3D(pEntity->GetMeshName(), pMesh);
#endif
			}
			pEntity->SetMesh(pMesh);

			for (size_t i=0;i< pMesh->m_numSubMesh;++i)
			{
				std::wstring strMaterialName = pEntity->GetMaterialName(i);
				IMaterial* pMaterial = pScene->GetMaterial(strMaterialName.c_str());
				if (!pMaterial)
				{
					pMaterial = pScene->CreateMaterial(pEntity->GetMeshName());
					LoadMaterial(strMaterialName, pMaterial);
				}
				pEntity->AddMaterial(pMaterial);
			}
			
			
			/*IMesh* pMesh = new IMesh;
			LoadMeshUnity(*, pMesh);
			(*IMesh::s_vMeshes)[*pEntity->m_strMeshName] = pMesh;*/
			/*std::map<std::wstring, IMesh*>::iterator it = IMesh::s_vMeshes->find(*(pEntity->m_strMeshName));
			if (it== IMesh::s_vMeshes->end())
			{
				IMesh* pMesh = new IMesh;
				LoadMeshUnity(*pEntity->m_strMeshName, pMesh);
				(*IMesh::s_vMeshes)[*pEntity->m_strMeshName] = pMesh;
			}
			else
			{
				pEntity->m_pMesh = it->second;
			}*/
		}
	}
	
	return true;
}

bool CSceneLoader::PrepareData(IScene* pScene )
{
	//下面是临时代码，以后应该改成使用GPU渲染场景物体到Albedo上
	if (pScene->GetScatterer(0)->m_texWidth==256)
	{
		for (size_t iScatterer = 0; iScatterer < pScene->GetScattererNumber(); ++iScatterer)
		{
			IScatterer* pScatterer = const_cast<IScatterer*>(pScene->GetScatterer(iScatterer));
			pScatterer->ResetAlbedoBuffer(pScatterer->m_texWidth, pScatterer->m_texHeight);
			MyFloat3* pAlbedoBuffer = pScatterer->GetAlbedoBuffer();

			MyFloat3 AlbedoColor;
			switch (iScatterer)
			{
			case 0:
			case 1:
			case 2:
				AlbedoColor = { 0.8f, 0.8f, 0.8f };
				break;
			case 3:
				AlbedoColor = { 0.0f, 0.5f, 0.25f };
				break;
			case 4:
				AlbedoColor = { 0.0f, 0.5f, 0.99f };
				break;
			case 5:
				AlbedoColor = { 0.5f, 0.5f, 0.5f };
				break;
			case 6:
				AlbedoColor = { 0.3f, 0.3f, 0.0f };
				break;
			default:
				break;
			}
			
			for (int y = 0; y < pScatterer->m_texHeight; y++)
				for (int x = 0; x < pScatterer->m_texWidth; x++)
				{
					pAlbedoBuffer[y*pScatterer->m_texWidth + x] = AlbedoColor;
				}
		}
	}
	else if (pScene->GetScatterer(0)->m_texWidth == 512)
	{
		{
			IScatterer* pScatterer = const_cast<IScatterer*>(pScene->GetScatterer(0));
			pScatterer->ResetAlbedoBuffer(pScatterer->m_texWidth, pScatterer->m_texHeight);
			MyFloat3* pAlbedoBuffer = pScatterer->GetAlbedoBuffer();

			for (int y = 0; y < pScatterer->m_texHeight; y++)
				for (int x = 0; x < pScatterer->m_texWidth; x++)
				{
					MyFloat3 AlbedoColor = { 0.8f, 0.8f, 0.8f };
					pAlbedoBuffer[y*pScatterer->m_texWidth + x] = AlbedoColor;
				}
		}

		//	for (size_t iScatterer=0;iScatterer<pScene->GetScattererNumber();++iScatterer)
		{
			IScatterer* pScatterer = const_cast<IScatterer*>(pScene->GetScatterer(1));
			pScatterer->ResetAlbedoBuffer(pScatterer->m_texWidth, pScatterer->m_texHeight);
			MyFloat3* pAlbedoBuffer = pScatterer->GetAlbedoBuffer();

			for (int y = 0; y < pScatterer->m_texHeight; y++)
				for (int x = 0; x < pScatterer->m_texWidth; x++)
				{
					MyFloat3 AlbedoColor;
					if (x < pScatterer->m_texWidth / 2 && y < pScatterer->m_texHeight / 2)
					{
						AlbedoColor = { 0.0f, 0.5f, 0.25f };
					}
					if (x >= pScatterer->m_texWidth / 2 && y < pScatterer->m_texHeight / 2)
					{
						AlbedoColor = { 0.0f, 0.5f, 0.99f };
					}
					if (x < pScatterer->m_texWidth / 2 && y >= pScatterer->m_texHeight / 2)
					{
						AlbedoColor = { 0.5f, 0.5f, 0.5f };
					}
					if (x >= pScatterer->m_texWidth / 2 && y >= pScatterer->m_texHeight / 2)
					{
						AlbedoColor = { 0.3f, 0.3f, 0.0f };
					}

					pAlbedoBuffer[y*pScatterer->m_texWidth + x] = AlbedoColor;
				}
		}
	}
	else
	{
		IScatterer* pScatterer = const_cast<IScatterer*>(pScene->GetScatterer(0));
		pScatterer->ResetAlbedoBuffer(pScatterer->m_texWidth, pScatterer->m_texHeight);
		MyFloat3* pAlbedoBuffer = pScatterer->GetAlbedoBuffer();

		for (int y = 0; y < pScatterer->m_texHeight; y++)
			for (int x = 0; x < pScatterer->m_texWidth; x++)
			{
				MyFloat3 AlbedoColor = { 0.8f, 0.8f, 0.8f };
				pAlbedoBuffer[y*pScatterer->m_texWidth + x] = AlbedoColor;
			}
	}

	return true;
}