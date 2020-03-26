#ifndef SKINNEDDATA_H
#define SKINNEDDATA_H

#include "../../Common/d3dUtil.h"
#include "../../Common/MathHelper.h"

///<summary>
/// A Keyframe defines the bone transformation at an instant in time.
///</summary>
//David��ĳ���������ڵ���ĳһ��ʱ�̵ı任��Ϣ
struct Keyframe
{
	Keyframe();
	~Keyframe();

    float TimePos;
	//David��֮���Դ洢TSR������Ϊ��Ҫ����ֵ
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
//David��ĳ��������ĳһ������������йؼ�֡��Ϣ
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	//David������ʱ���t���ҳ�����������ؼ�֡����ֵ���任���󣬴洢��M
    void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

	std::vector<Keyframe> Keyframes; 	
};

///<summary>
/// Examples of AnimationClips are "Walk", "Run", "Attack", "Defend".
/// An AnimationClip requires a BoneAnimation for every bone to form
/// the animation clip.    
///</summary>
//David��ĳ���������漰��������Ϣ
struct AnimationClip
{
	float GetClipStartTime()const;
	float GetClipEndTime()const;

	//David���ص㽲������֮֡���ֵ��������µ�����任����
    void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

    std::vector<BoneAnimation> BoneAnimations; 	
};

//David��ĳ��3D��Ƥģ�Ͷ�Ӧ����Ƥ���ֵ��������ݣ��Ǽܲ㼶��������λ����任�������еĶ���
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