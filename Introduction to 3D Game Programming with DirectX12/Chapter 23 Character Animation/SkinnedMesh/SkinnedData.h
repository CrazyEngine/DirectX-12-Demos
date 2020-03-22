#ifndef SKINNEDDATA_H
#define SKINNEDDATA_H

#include "../../Common/d3dUtil.h"
#include "../../Common/MathHelper.h"

///<summary>
/// A Keyframe defines the bone transformation at an instant in time.
///</summary>
//David：某个个骨骼节点在某一个时刻的变换信息
struct Keyframe
{
	Keyframe();
	~Keyframe();

    float TimePos;
	//David：之所以存储TSR，是因为需要做插值
	DirectX::XMFLOAT3 Translation;
    DirectX::XMFLOAT3 Scale;
    DirectX::XMFLOAT4 RotationQuat;
};

///<summary>
/// A BoneAnimation is defined by a list of keyframes.  For time
/// values inbetween two keyframes, we interpolate between the
/// two nearest keyframes that bound the time.  
///
/// We assume an animation always has two keyframes.
///</summary>
//David：某个骨骼在某一个动作里的所有关键帧信息
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	//David：根据时间点t，找出最近的两个关键帧，插值出变换矩阵，存储进M
    void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

	std::vector<Keyframe> Keyframes; 	
};

///<summary>
/// Examples of AnimationClips are "Walk", "Run", "Attack", "Defend".
/// An AnimationClip requires a BoneAnimation for every bone to form
/// the animation clip.    
///</summary>
//David：某个动作所涉及的所有信息
struct AnimationClip
{
	float GetClipStartTime()const;
	float GetClipEndTime()const;

	//David：重点讲：在两帧之间插值，计算出新的坐标变换矩阵
    void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

    std::vector<BoneAnimation> BoneAnimations; 	
};

//David：某个3D蒙皮模型对应的蒙皮部分的所有数据：骨架层级、骨骼绑定位置逆变换矩阵、所有的动作
class SkinnedData
{
public:

	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;

	void Set(
		std::vector<int>& boneHierarchy, 
		std::vector<DirectX::XMFLOAT4X4>& boneOffsets,
		std::unordered_map<std::string, AnimationClip>& animations);

	 // In a real project, you'd want to cache the result if there was a chance
	 // that you were calling this several times with the same clipName at 
	 // the same timePos.
    void GetFinalTransforms(const std::string& clipName, float timePos, 
		 std::vector<DirectX::XMFLOAT4X4>& finalTransforms)const;

private:
    // Gives parentIndex of ith bone.
	std::vector<int> mBoneHierarchy;

	std::vector<DirectX::XMFLOAT4X4> mBoneOffsets;
   
	std::unordered_map<std::string, AnimationClip> mAnimations;
};
 
#endif // SKINNEDDATA_H