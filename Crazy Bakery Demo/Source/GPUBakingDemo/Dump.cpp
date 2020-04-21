#include "stdafx.h"

#include "Dump.h"
#include "GPUBake.h"
using namespace CrazyBakery;

//#include "Dump.h"
#include "Image.h"
//#include "Random.h"
#include "File.h"
//#include "Grid.h"
#include "String.h"
#include <algorithm>
#include <fstream>
#include <map>
//#include <optixu/optixu_math_namespace.h>
#include <cuda_runtime.h>
#include <set>
#include <string>

using namespace std;
//using namespace optix;

//namespace CrazyBakery
//{
/*void DumpTriangle(const vector<float3> &p, const wstring &path)
{
	fstream out;
	out.open(path, fstream::out);

	for (int i = 0; i < p.size(); i += 3)
	{
		const float3 p0 = p[i];
		const float3 p1 = p[i + 1];
		const float3 p2 = p[i + 2];

		out << "v " << p0.x << ' ' << p0.y << ' ' << p0.z << endl;
		out << "v " << p1.x << ' ' << p1.y << ' ' << p1.z << endl;
		out << "v " << p2.x << ' ' << p2.y << ' ' << p2.z << endl;

		out << "f " << i + 1 << ' ' << i + 2 << ' ' << i + 3 << endl;
	}

	out.close();
}*/

/*void DumpScatterer(const Scatterer *scatterer, const wstring &path, const wstring texture)
{
	int poff = 1;
	int noff = 1;
	int uoff = 1;

	wstring parent = GetParent(path);

	wofstream obj, mtl;
	obj.open(path, wofstream::out);
	mtl.open(parent + scatterer->m_name + L".mtl", wofstream::out);

	obj << L"mtllib " << scatterer->m_name.c_str() << L".mtl" << endl;

	const unsigned int nPos = scatterer->m_triangle.m_vertexPosition.size();
	const unsigned int nNorm = scatterer->m_triangle.m_vertexNormal.size();
	const unsigned int nUV = scatterer->m_triangle.m_vertexUV.size();

	for (int i = 0; i < int(nPos); i++)
	{
		float3 pos = scatterer->m_triangle.m_vertexPosition[i];
		obj << L"v " << pos.x << L' ' << pos.y << L' ' << pos.z << endl;
	}
	for (int i = 0; i < int(nUV); i++)
	{
		float2 uv = scatterer->m_triangle.m_vertexUV[i];
		obj << L"vt " << uv.x << L' ' << uv.y << endl;
	}
	for (int i = 0; i < int(nNorm); i++)
	{
		float3 norm = scatterer->m_triangle.m_vertexNormal[i];
		obj << L"vn " << norm.x << L' ' << norm.y << L' ' << norm.z << endl;
	}

	for (int g = 0; g < scatterer->m_materialGroup.size(); g++)
	{
		const Scatterer::MaterialGroup &mg = scatterer->m_materialGroup[g];

		mtl << L"newmtl " << mg.m_name.c_str() << endl;
		mtl << L"Kd "
			<< mg.m_material->m_albedo.x << L" "
			<< mg.m_material->m_albedo.y << L" "
			<< mg.m_material->m_albedo.z << endl;

		if (texture.size())
			mtl << L"map_Kd " << texture.c_str() << endl;

		obj << L"usemtl " << mg.m_name.c_str() << endl;

		const int nFace = mg.m_positionIndexSize;
		for (int i = 0; i < nFace; i++)
		{
			const int3 pid = scatterer->m_triangle.m_vertexPositionIndex[mg.m_positionIndexOff + i];
			const int3 nid = scatterer->m_triangle.m_vertexNormalIndex[mg.m_normalIndexOff + i];

			obj << L"f ";
			if (nUV)
			{
				const int3 uid = scatterer->m_triangle.m_vertexUVIndex[mg.m_uvIndexOff + i];
				obj << poff + pid.x << L"/" << uoff + uid.x << L"/" << noff + nid.x << L' ';
				obj << poff + pid.y << L"/" << uoff + uid.y << L"/" << noff + nid.y << L' ';
				obj << poff + pid.z << L"/" << uoff + uid.z << L"/" << noff + nid.z << endl;
			}
			else
			{
				obj << poff + pid.x << L"//" << noff + nid.x << L' ';
				obj << poff + pid.y << L"//" << noff + nid.y << L' ';
				obj << poff + pid.z << L"//" << noff + nid.z << endl;
			}
		}
	}

	obj.close();
	mtl.close();
}

void DumpScatterer(const vector<const Scatterer*> &scatterer, const wstring &path)
{
	for (int i = 0; i < scatterer.size(); i++)
	{
		const wstring s = path + L"geom_" + to_wstring(i) + L".obj";
		DumpScatterer(scatterer[i], s);
	}
}*/
extern wchar_t m_strOutputPath[256];
void DumpMergedScatterer(const IScene* pScene, const wchar_t* strTextureType)
{
	if (pScene->GetScattererNumber() == 0)
		return;

	wstring strPath = m_strOutputPath;
	wstring strObjName(L"Scene.obj");
	wstring strMaterialName(L"Scene.mtl");
	wstring textureType = strTextureType;

	wfstream obj, mtl;
	obj.open(strPath + strObjName, fstream::out);
	mtl.open(strPath + strMaterialName, fstream::out);

	obj << L"mtllib " << strMaterialName << endl;//在obj里指定使用的material文件名
		
	//下面三个变量用于累加偏移量，因为obj里所有的顶点坐标都在一起索引，即使有多个g也不行
	//而模型里的索引都是以模型为单位的
	int poff = 1;
	int noff = 1;
	int uoff = 1;

	for (size_t iScatterer = 0; iScatterer < pScene->GetScattererNumber(); ++iScatterer)
	{
		for (int k = 0; k < pScene->GetEntityNumber(iScatterer); k++)
		{
			const IStaticEntity* pEntity = pScene->GetEntity(iScatterer,k);
			const IMesh* pMesh = pEntity->GetMesh();

			//向obj里写入VB
			obj << endl << L"g " << pEntity->GetEntityName() << L"_" << pEntity->GetMeshName() << endl;

			for (size_t i = 0; i < pMesh->m_numVertex; i++)
			{
				const MyMatrix4x4& matWorld = pEntity->m_matWorld;
				MyFloat3 Pos = matWorld * pMesh->m_pVertexPosition[i];
				obj << L"v " << Pos.x << L' ' << Pos.y << L' ' << Pos.z << endl;
			}
#ifdef VertexAttributeUseDifferentIndex
			for (size_t i = 0; i < pMesh->m_numUV2; i++)
#else
			for (size_t i = 0; i < pMesh->m_numVertex; i++)
#endif
				
			{
				MyFloat2 UV = pMesh->m_pVertexUV2[i];
				UV *= pEntity->m_vMapScale;
				UV += pEntity->m_vMapOffset;
				obj << L"vt " << UV.x << L' ' << UV.y << endl;
			}
#ifdef VertexAttributeUseDifferentIndex
			for (size_t i = 0; i < pMesh->m_numNormal; i++)
#else
			for (size_t i = 0; i < pMesh->m_numVertex; i++)
#endif
			{
				const MyMatrix3x3& matWorld3x3 = pEntity->m_matWorld3x3;
				//TODO 验证是否正确，应该是逆矩阵的转置矩阵
				MyFloat3 Normal = matWorld3x3 * pMesh->m_pVertexNormal[i];
				obj << L"vn " << Normal.x << L' ' << Normal.y << L' ' << Normal.z << endl;
			}

			for (size_t iSubMesh = 0; iSubMesh < pMesh->m_numSubMesh; ++iSubMesh)
			{
				const IMesh::ISubMesh* pSubMesh = &pMesh->m_pSubMeshes[iSubMesh];
				const IMaterial* pMaterial = pEntity->GetMaterial(iSubMesh);
				const wstring strMaterialName = (wstring(pScene->GetScatterer(iScatterer)->GetName()) + L"_" + pEntity->GetMaterialName(iSubMesh)).c_str();

				//向材质文件里写入材质
				mtl << L"newmtl " << strMaterialName.c_str() << endl;
				mtl << L"Ka "
					<< 0.0 << L" "
					<< 0.0 << L" "
					<< 0.0 << endl;
				mtl << L"Ks "
					<< 0.0 << L" "
					<< 0.0 << L" "
					<< 0.0 << endl;
				if (bool boJustShowLightmap=true)//输出的obj在工具中只显示Lightmap的颜色，不包含材质的
				{
					mtl << L"Kd "
						<< 1.0 << L" "
						<< 1.0 << L" "
						<< 1.0 << endl;
				} 
				else
				{
					mtl << L"Kd "
						<< pMaterial->m_vAlbedoTint.x << L" "
						<< pMaterial->m_vAlbedoTint.y << L" "
						<< pMaterial->m_vAlbedoTint.z << endl;
				}

				if (textureType.size())
					mtl << L"map_Kd " << pScene->GetScatterer(iScatterer)->GetName() << textureType.c_str() << endl;

				//向obj文件里写入使用的材质
				obj << L"usemtl " << strMaterialName.c_str() << endl;

				//向obj文件里写入顶点索引
				for (size_t i = 0; i < pSubMesh->m_numFaces; ++i)
				{
					const MyInt3& pid = pMesh->m_pVertexIndices[pSubMesh->m_numOffset + i];
					const MyInt3& nid = pMesh->m_pVertexIndices[pSubMesh->m_numOffset + i];

					obj << L"f ";
#ifdef VertexAttributeUseDifferentIndex
					if (pMesh->m_numUV2)
#else
					if (pMesh->m_numVertex)
#endif
					{
#ifdef VertexAttributeUseDifferentIndex
						MyInt3* VertexUVIndex = pMesh->m_pVertexUV2Index;
#else
						MyInt3* VertexUVIndex = pMesh->m_pVertexIndices;
#endif
						const MyInt3 uid = VertexUVIndex[pSubMesh->m_numOffset + i];
						obj << poff + pid.x << L"/" << uoff + uid.x << L"/" << noff + nid.x << L' ';
						obj << poff + pid.y << L"/" << uoff + uid.y << L"/" << noff + nid.y << L' ';
						obj << poff + pid.z << L"/" << uoff + uid.z << L"/" << noff + nid.z << endl;
					}
					else
					{
						obj << poff + pid.x << L"//" << noff + nid.x << L' ';
						obj << poff + pid.y << L"//" << noff + nid.y << L' ';
						obj << poff + pid.z << L"//" << noff + nid.z << endl;
					}
				}
			}

#ifdef VertexAttributeUseDifferentIndex
			poff += pMesh->m_numVertex;
			noff += pMesh->m_numNormal;
			uoff += pMesh->m_numUV2;
#else
			poff += pMesh->m_numVertex;
			noff += pMesh->m_numVertex;
			uoff += pMesh->m_numVertex;
#endif
				
		}//end of entity
	}//end of scatterer

	obj.close();
	mtl.close();
}

//void DumpRay(const Ray *ray, const Hit *hit, const int n, const std::wstring &path)
//{
//	std::fstream out;
//	out.open(path.c_str(), std::fstream::out);

//	int voff = 1;
//	for (int i = 0; i < n; i++)
//	{
//		if (hit[i].t < 0.f)
//			continue;

//		const float3 p0 = ray[i].origin;
//		const float3 p1 = ray[i].origin + ray[i].direction/* * hit[i].t*/;
//		const float3 p2 = p1;

//		out << "v " << p0.x << ' ' << p0.y << ' ' << p0.z << std::endl;
//		out << "v " << p1.x << ' ' << p1.y << ' ' << p1.z << std::endl;
//		out << "v " << p2.x << ' ' << p2.y << ' ' << p2.z << std::endl;
//		out << "f " << voff << ' ' << voff + 1 << ' ' << voff + 2 << std::endl;

//		voff += 3;
//	}

//	out.close();
//}
//void DumpOptixRay(const Ray *devRay, const Hit *devHit, const int n, const std::wstring & path)
//{
//	vector<Ray> hostRay(n);
//	vector<Hit> hostHit(n);
//	cudaMemcpy(&hostRay[0], devRay, n * sizeof(Ray), cudaMemcpyDeviceToHost);
//	cudaMemcpy(&hostHit[0], devHit, n * sizeof(Hit), cudaMemcpyDeviceToHost);
//	DumpRay(&hostRay[0], &hostHit[0], n, path);
//}

void DumpBuffer(size_t numWidth, size_t numHeight, MyFloat3* pData, const wchar_t* StageName, int iLightmap, const wchar_t* strName, bool boFloat)
{
	wstring path = m_strOutputPath;
	path = path + L"_" + StageName + L"Lightmap" + to_wstring((long long)iLightmap) + L"." + strName;// strPath;

	ImageRGBf image;
//		vector<float3> hostBuffer;
	const int w = numWidth;
	const int h = numHeight;

//		hostBuffer.resize(w * h);
//		cudaMemcpy(&hostBuffer[0], pData, w * h * sizeof(float3), cudaMemcpyDeviceToHost);

	image.Resize(w, h);
	for (int y = 0, k = 0; y < h; y++)
		for (int x = 0; x < w; x++, k++)
			image.SetPixel(x, y, float3{ pData[k].x, pData[k].y, pData[k].z });

	File output;
	output.m_format = boFloat ? File::f32 : File::u8;
	output.m_channel = File::RGB;
	output.m_path = path;
	output.m_path += boFloat ? L".hdr" : L".png";
	output.Save(image);
}

void ProcessMessage(EMessageType eType, const wchar_t* strMessage)
{
	MessageBox(NULL, strMessage, L"", MB_OK);
}

void RegisterDumpFuncs()
{
	g_pDumpMergedScatterer = DumpMergedScatterer;
	g_pDumpBuffer = DumpBuffer;

	g_pProcessMessage = ProcessMessage;
}
/*void DumpBuffer(const vector<Batch::Output> &devBuffer, const wchar_t* strPath, const wchar_t* StageName, bool boFloat)
{
	const wstring path = strPath;
	const std::wstring StageName = StageName;
	for (int i = 0; i < devBuffer.size(); i++)
		DumpBuffer(devBuffer[i], path + L"_" + StageName + L"Lightmap" + to_wstring((long long)i) + L"." + devBuffer[i].name, boFloat);
}*/

/*void DumpOptixImage(const Buffer<float4> &devImage, const int w, const int h, const wstring &path)
{
	vector<float4> buffer(w * h);
	cudaMemcpy(&buffer[0], devImage.ptr(), w * h * sizeof(float4), cudaMemcpyDeviceToHost);

	ImageRGBf hostImage(w, h);
	for (int j = 0, k = 0; j < h; j++)
		for (int i = 0; i < w; i++, k++)
			hostImage.SetPixel(i, j, float3{ buffer[k].x, buffer[k].y, buffer[k].z });

	File output;
	output.m_format = File::f32;
	output.m_channel = File::RGB;
	output.m_path = path + L".RGB.hdr";
	output.Save(hostImage);

	for (int j = 0, k = 0; j < h; j++)
		for (int i = 0; i < w; i++, k++)
			hostImage.SetPixel(i, j, float3{ buffer[k].w, buffer[k].w, buffer[k].w });
	output.m_path = path + L".W.hdr";
	output.Save(hostImage);
}

void DumpOptixImage(const Buffer<float3> &devImage, const int w, const int h, const wstring &path)
{
	vector<float3> buffer(w * h);
	cudaMemcpy(&buffer[0], devImage.ptr(), w * h * sizeof(float3), cudaMemcpyDeviceToHost);

	ImageRGBf hostImage(w, h);
	for (int j = 0, k = 0; j < h; j++)
		for (int i = 0; i < w; i++, k++)
			hostImage.SetPixel(i, j, float3{ buffer[k].x, buffer[k].y, buffer[k].z });

	File output;
	output.m_format = File::f32;
	output.m_channel = File::RGB;
	output.m_path = path + L".RGB.hdr";
	output.Save(hostImage);
}*/
//}
