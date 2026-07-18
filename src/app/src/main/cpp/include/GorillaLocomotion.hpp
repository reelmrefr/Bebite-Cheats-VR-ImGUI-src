#pragma once
#include "BNMResolve.hpp"

using namespace BNM;
using namespace BNM::IL2CPP;
using namespace BNM::Structures;
using namespace BNM::Structures::Unity;
using namespace BNM::UnityEngine;
#define O(str) BNM_OBFUSCATE(str)

namespace GorillaLocomotion{
    struct Player : Behaviour {
    public:
        static Class GetClass() {
            static BNM::Class clahh = Class(O("GorillaLocomotion"), O("Player"), Image(O("Assembly-CSharp.dll")));
            if (!clahh.IsValid()) {
                clahh = Class(O("GorillaLocomotion"), O("GTPlayer"), Image(O("Assembly-CSharp.dll")));
            }
            return clahh;
        }
        static MonoType* GetType() {
            return GetClass().GetMonoType();
        }

        static Player* get_Instance() {
            static Method<Player*> method = GetClass().GetMethod(O("get_Instance"));
            return method();
        }

        static Player* GetInstance() {
            static Field<Player*> field = GetClass().GetField(O("_instance"));
            return field();
        }

        SphereCollider* GetHeadCollider() {
            static Field<SphereCollider*> headCollider = GetClass().GetField(O("headCollider"));
            headCollider.SetInstance(this);
            return headCollider();
        }

        Collider* GetBodyCollider() {
            static Field<Collider*> bodyCollider = GetClass().GetField(O("bodyCollider"));
            bodyCollider.SetInstance(this);
            return bodyCollider();
        }

        Transform* GetLeftHandFollower() {
            static Field<Transform*> leftHandFollower = GetClass().GetField(O("leftHandFollower"));
            leftHandFollower.SetInstance(this);
            return leftHandFollower();
        }

        Transform* GetRightHandFollower() {
            static Field<Transform*> rightHandFollower = GetClass().GetField(O("rightHandFollower"));
            rightHandFollower.SetInstance(this);
            return rightHandFollower();
        }

        Transform* GetLeftHandTransform() {
            static Field<Transform*> leftHandTransform = GetClass().GetField(O("leftHandTransform"));
            if (!leftHandTransform.IsValid()) {
                leftHandTransform = GetClass().GetField(O("leftControllerTransform"));
            }

            leftHandTransform.SetInstance(this);
            return leftHandTransform();
        }

        Transform* GetRightHandTransform() {
            static Field<Transform*> rightHandTransform = GetClass().GetField(O("rightHandTransform"));
            if (!rightHandTransform.IsValid()) {
                rightHandTransform = GetClass().GetField(O("rightControllerTransform"));
            }
            rightHandTransform.SetInstance(this);
            return rightHandTransform();
        }

        Vector3 GetLastLeftHandPosition() {
            static Field<Vector3> lastLeftHandPosition = GetClass().GetField(O("lastLeftHandPosition"));
            lastLeftHandPosition.SetInstance(this);
            return lastLeftHandPosition();
        }

        Vector3 GetLastRightHandPosition() {
            static Field<Vector3> lastRightHandPosition = GetClass().GetField(O("lastRightHandPosition"));
            lastRightHandPosition.SetInstance(this);
            return lastRightHandPosition();
        }

        Vector3 GetLastHeadPosition() {
            static Field<Vector3> lastHeadPosition = GetClass().GetField(O("lastHeadPosition"));
            lastHeadPosition.SetInstance(this);
            return lastHeadPosition();
        }

        Rigidbody* GetPlayerRigidBody() {
            static Field<Rigidbody*> playerRigidBody = GetClass().GetField(O("playerRigidBody"));
            playerRigidBody.SetInstance(this);
            return playerRigidBody();
        }

        int GetVelocityHistorySize() {
            static Field<int> velocityHistorySize = GetClass().GetField(O("velocityHistorySize"));
            velocityHistorySize.SetInstance(this);
            return velocityHistorySize();
        }

        float GetMaxArmLength() {
            static Field<float> maxArmLength = GetClass().GetField(O("maxArmLength"));
            maxArmLength.SetInstance(this);
            return maxArmLength();
        }

        float GetUnStickDistance() {
            static Field<float> unStickDistance = GetClass().GetField(O("unStickDistance"));
            unStickDistance.SetInstance(this);
            return unStickDistance();
        }

        float GetVelocityLimit() {
            static Field<float> velocityLimit = GetClass().GetField(O("velocityLimit"));
            velocityLimit.SetInstance(this);
            return velocityLimit();
        }

        float GetMaxJumpSpeed() {
            static Field<float> maxJumpSpeed = GetClass().GetField(O("maxJumpSpeed"));
            maxJumpSpeed.SetInstance(this);
            return maxJumpSpeed();
        }

        float GetJumpMultiplier() {
            static Field<float> jumpMultiplier = GetClass().GetField(O("jumpMultiplier"));
            jumpMultiplier.SetInstance(this);
            return jumpMultiplier();
        }

        float GetMinimumRaycastDistance() {
            static Field<float> minimumRaycastDistance = GetClass().GetField(O("minimumRaycastDistance"));
            minimumRaycastDistance.SetInstance(this);
            return minimumRaycastDistance();
        }

        float GetefaultSlideFactor() {
            static Field<float> defaultSlideFactor = GetClass().GetField(O("defaultSlideFactor"));
            defaultSlideFactor.SetInstance(this);
            return defaultSlideFactor();
        }

        float GetDefaultPrecision() {
            static Field<float> defaultPrecision = GetClass().GetField(O("defaultPrecision"));
            defaultPrecision.SetInstance(this);
            return defaultPrecision();
        }

        Mono::Array<Vector3>* GetVelocityHistory() {
            static Field<Mono::Array<Vector3>*> velocityHistory = GetClass().GetField(O("velocityHistory"));
            velocityHistory.SetInstance(this);
            return velocityHistory();
        }

        int GetVelocityIndex() {
            static Field<int> velocityIndex = GetClass().GetField(O("velocityIndex"));
            velocityIndex.SetInstance(this);
            return velocityIndex();
        }

        Vector3 GetCurrentVelocity() {
            static Field<Vector3> currentVelocity = GetClass().GetField(O("currentVelocity"));
            currentVelocity.SetInstance(this);
            return currentVelocity();
        }

        Vector3 GetDenormalizedVelocityAverage() {
            static Field<Vector3> denormalizedVelocityAverage = GetClass().GetField(O("denormalizedVelocityAverage"));
            denormalizedVelocityAverage.SetInstance(this);
            return denormalizedVelocityAverage();
        }

        bool GetJumpHandIsLeft() {
            static Field<bool> jumpHandIsLeft = GetClass().GetField(O("jumpHandIsLeft"));
            jumpHandIsLeft.SetInstance(this);
            return jumpHandIsLeft();
        }

        Vector3 GetLeftHandOffset() {
            static Field<Vector3> leftHandOffset = GetClass().GetField(O("leftHandOffset"));
            leftHandOffset.SetInstance(this);
            return leftHandOffset();
        }

        Vector3 GetRightHandOffset() {
            static Field<Vector3> rightHandOffset = GetClass().GetField(O("rightHandOffset"));
            rightHandOffset.SetInstance(this);
            return rightHandOffset();
        }

        bool GetDisableMovement() {
            static Field<bool> disableMovement = GetClass().GetField(O("disableMovement"));
            disableMovement.SetInstance(this);
            return disableMovement();
        }

        float GetSlideControl() {
            static Field<float> slideControl = GetClass().GetField(O("slideControl"));
            slideControl.SetInstance(this);
            return slideControl();
        }

        void SetHeadCollider(SphereCollider* value) {
            static Field<SphereCollider*> headCollider = GetClass().GetField(O("headCollider"));
            headCollider.SetInstance(this);
            headCollider = value;
        }

        void SetBodyCollider(Component* value) {
            static Field<Component*> bodyCollider = GetClass().GetField(O("bodyCollider"));
            bodyCollider.SetInstance(this);
            bodyCollider = value;
        }

        void SetLeftHandFollower(Transform* value) {
            static Field<Transform*> leftHandFollower = GetClass().GetField(O("leftHandFollower"));
            leftHandFollower.SetInstance(this);
            leftHandFollower = value;
        }

        void SetRightHandFollower(Transform* value) {
            static Field<Transform*> rightHandFollower = GetClass().GetField(O("rightHandFollower"));
            rightHandFollower.SetInstance(this);
            rightHandFollower = value;
        }

        void SetLeftHandTransform(Transform* value) {
            static Field<Transform*> leftHandTransform = GetClass().GetField(O("leftHandTransform"));
            leftHandTransform.SetInstance(this);
            leftHandTransform = value;
        }

        void SetRightHandTransform(Transform* value) {
            static Field<Transform*> rightHandTransform = GetClass().GetField(O("rightHandTransform"));
            rightHandTransform.SetInstance(this);
            rightHandTransform = value;
        }

        void SetLastLeftHandPosition(Vector3 value) {
            static Field<Vector3> lastLeftHandPosition = GetClass().GetField(O("lastLeftHandPosition"));
            lastLeftHandPosition.SetInstance(this);
            lastLeftHandPosition = value;
        }

        void SetLastRightHandPosition(Vector3 value) {
            static Field<Vector3> lastRightHandPosition = GetClass().GetField(O("lastRightHandPosition"));
            lastRightHandPosition.SetInstance(this);
            lastRightHandPosition = value;
        }

        void SetLastHeadPosition(Vector3 value) {
            static Field<Vector3> lastHeadPosition = GetClass().GetField(O("lastHeadPosition"));
            lastHeadPosition.SetInstance(this);
            lastHeadPosition = value;
        }

        void SetPlayerRigidBody(Rigidbody* value) {
            static Field<Rigidbody*> playerRigidBody = GetClass().GetField(O("playerRigidBody"));
            playerRigidBody.SetInstance(this);
            playerRigidBody = value;
        }

        void SetVelocityHistorySize(int value) {
            static Field<int> velocityHistorySize = GetClass().GetField(O("velocityHistorySize"));
            velocityHistorySize.SetInstance(this);
            velocityHistorySize = value;
        }

        void SetMaxArmLength(float value) {
            static Field<float> maxArmLength = GetClass().GetField(O("maxArmLength"));
            maxArmLength.SetInstance(this);
            maxArmLength = value;
        }

        void SetUnStickDistance(float value) {
            static Field<float> unStickDistance = GetClass().GetField(O("unStickDistance"));
            unStickDistance.SetInstance(this);
            unStickDistance = value;
        }

        void SetVelocityLimit(float value) {
            static Field<float> velocityLimit = GetClass().GetField(O("velocityLimit"));
            velocityLimit.SetInstance(this);
            velocityLimit = value;
        }

        void SetMaxJumpSpeed(float value) {
            static Field<float> maxJumpSpeed = GetClass().GetField(O("maxJumpSpeed"));
            maxJumpSpeed.SetInstance(this);
            maxJumpSpeed = value;
        }

        void SetJumpMultiplier(float value) {
            static Field<float> jumpMultiplier = GetClass().GetField(O("jumpMultiplier"));
            jumpMultiplier.SetInstance(this);
            jumpMultiplier = value;
        }

        void SetMinimumRaycastDistance(float value) {
            static Field<float> minimumRaycastDistance = GetClass().GetField(O("minimumRaycastDistance"));
            minimumRaycastDistance.SetInstance(this);
            minimumRaycastDistance = value;
        }

        void SetDefaultSlideFactor(float value) {
            static Field<float> defaultSlideFactor = GetClass().GetField(O("defaultSlideFactor"));
            defaultSlideFactor.SetInstance(this);
            defaultSlideFactor = value;
        }

        void SetDefaultPrecision(float value) {
            static Field<float> defaultPrecision = GetClass().GetField(O("defaultPrecision"));
            defaultPrecision.SetInstance(this);
            defaultPrecision = value;
        }

        void SetVelocityHistory(Mono::Array<Vector3>* value) {
            static Field<Mono::Array<Vector3>*> velocityHistory = GetClass().GetField(O("velocityHistory"));
            velocityHistory.SetInstance(this);
            velocityHistory = value;
        }

        void SetVelocityIndex(int value) {
            static Field<int> velocityIndex = GetClass().GetField(O("velocityIndex"));
            velocityIndex.SetInstance(this);
            velocityIndex = value;
        }

        void SetCurrentVelocity(Vector3 value) {
            static Field<Vector3> currentVelocity = GetClass().GetField(O("currentVelocity"));
            currentVelocity.SetInstance(this);
            currentVelocity = value;
        }

        void SetDenormalizedVelocityAverage(Vector3 value) {
            static Field<Vector3> denormalizedVelocityAverage = GetClass().GetField(O("denormalizedVelocityAverage"));
            denormalizedVelocityAverage.SetInstance(this);
            denormalizedVelocityAverage = value;
        }

        void SetJumpHandIsLeft(bool value) {
            static Field<bool> jumpHandIsLeft = GetClass().GetField(O("jumpHandIsLeft"));
            jumpHandIsLeft.SetInstance(this);
            jumpHandIsLeft = value;
        }

        void SetLeftHandOffset(Vector3 value) {
            static Field<Vector3> leftHandOffset = GetClass().GetField(O("leftHandOffset"));
            leftHandOffset.SetInstance(this);
            leftHandOffset = value;
        }

        void SetRightHandOffset(Vector3 value) {
            static Field<Vector3> rightHandOffset = GetClass().GetField(O("rightHandOffset"));
            rightHandOffset.SetInstance(this);
            rightHandOffset = value;
        }

        void SetDisableMovement(bool value) {
            static Field<bool> disableMovement = GetClass().GetField(O("disableMovement"));
            disableMovement.SetInstance(this);
            disableMovement = value;
        }

        bool IsHandTouching(bool leftHand) {
            static Method<bool> IsHandTouching = GetClass().GetMethod(O("IsHandTouching"));
            return IsHandTouching[this](leftHand);
        }
        void SetSlideControl(float value) {
            static Field<float> slideControl = GetClass().GetField(O("slideControl"));
            slideControl.SetInstance(this);
            slideControl = value;
        }
    };
}

struct VRMap : Il2CppObject {
    static Class GetClass() {
        static Class mclass = Class(O(""), O("VRMap"));
        return mclass;
    }

    Vector3 GetTrackingRotationOffset() {
        static Field<Vector3> trackingRotationOffset = GetClass().GetField(O("trackingRotationOffset"));
        trackingRotationOffset.SetInstance(this);
        return trackingRotationOffset();
    }
    void SetTrackingRotationOffset(Vector3 offset) {
        static Field<Vector3> trackingRotationOffset = GetClass().GetField(O("trackingRotationOffset"));
        trackingRotationOffset.SetInstance(this);
        trackingRotationOffset = offset;
    }
};

struct VRRig : Behaviour {
    static Class GetClass() {
        return Class(O(""), O("VRRig"));
    }

    VRMap* GetHead() {
        static Field<VRMap*> f = GetClass().GetField(O("head"));
        f.SetInstance(this);
        return f();
    }
    void SetHead(VRMap* value) {
        static Field<VRMap*> f = GetClass().GetField(O("head"));
        f.SetInstance(this);
        f = value;
    }

    VRMap* GetRightHand() {
        static Field<VRMap*> f = GetClass().GetField(O("rightHand"));
        f.SetInstance(this);
        return f();
    }
    void SetRightHand(VRMap* value) {
        static Field<VRMap*> f = GetClass().GetField(O("rightHand"));
        f.SetInstance(this);
        f = value;
    }

    VRMap* GetLeftHand() {
        static Field<VRMap*> f = GetClass().GetField(O("leftHand"));
        f.SetInstance(this);
        return f();
    }
    void SetLeftHand(VRMap* value) {
        static Field<VRMap*> f = GetClass().GetField(O("leftHand"));
        f.SetInstance(this);
        f = value;
    }

    bool GetIsOfflineVRRig() {
        static Field<bool> f = GetClass().GetField(O("isOfflineVRRig"));
        f.SetInstance(this);
        return f();
    }
    void SetIsOfflineVRRig(bool value) {
        static Field<bool> f = GetClass().GetField(O("isOfflineVRRig"));
        f.SetInstance(this);
        f = value;
    }

    GameObject* GetMainCamera() {
        static Field<GameObject*> f = GetClass().GetField(O("mainCamera"));
        f.SetInstance(this);
        return f();
    }
    void SetMainCamera(GameObject* value) {
        static Field<GameObject*> f = GetClass().GetField(O("mainCamera"));
        f.SetInstance(this);
        f = value;
    }

    float GetRatio() {
        static Field<float> f = GetClass().GetField(O("ratio"));
        f.SetInstance(this);
        return f();
    }
    void SetRatio(float value) {
        static Field<float> f = GetClass().GetField(O("ratio"));
        f.SetInstance(this);
        f = value;
    }

    Vector3 GetHeadBodyOffset() {
        static Field<Vector3> f = GetClass().GetField(O("headBodyOffset"));
        f.SetInstance(this);
        return f();
    }
    void SetHeadBodyOffset(const Vector3& value) {
        static Field<Vector3> f = GetClass().GetField(O("headBodyOffset"));
        f.SetInstance(this);
        f = value;
    }

    Transform* GetPlayerOffsetTransform() {
        static Field<Transform*> f = GetClass().GetField(O("playerOffsetTransform"));
        f.SetInstance(this);
        return f();
    }
    void SetPlayerOffsetTransform(Transform* value) {
        static Field<Transform *> f = GetClass().GetField(O("playerOffsetTransform"));
        f.SetInstance(this);
        f = value;
    }

    Transform* GetLeftHandTransform() {
        static Field<Transform*> f = GetClass().GetField(O("leftHandTransform"));
        f.SetInstance(this);
        return f();
    }
    Transform* GetRightHandTransform() {
        static Field<Transform*> f = GetClass().GetField(O("rightHandTransform"));
        f.SetInstance(this);
        return f();
    }
    Text* GetPlayerText() {
        static Field<Text*> f = GetClass().GetField(O("playerText"));
        f.SetInstance(this);
        return f();
    }
    Component* GetPhotonView() {
        static Class PhotonView(O("Photon.Pun"), O("PhotonView"));
        static Method<Component*> Get = PhotonView.GetMethod(O("Get"), { O("component") });

        return Get(this);
    }

    Mono::Array<Material*>* GetMaterialsToChangeTo() {
        static Field<Array<Material*>*> materialsToChangeTo = GetClass().GetField(O("materialsToChangeTo"));
        materialsToChangeTo.SetInstance(this);
        return materialsToChangeTo();
    }

    void PlayHandTap(int soundIndex, bool isLeftHand, float tapVolume) {
        static Method<void> PlayHandTapLocal = GetClass().GetMethod(O("PlayHandTapLocal"));
        if (!PlayHandTapLocal.IsValid()) {
            PlayHandTapLocal = GetClass().GetMethod(O("PlayHandTap"));
        }
        PlayHandTapLocal[this](soundIndex, isLeftHand, tapVolume);
    }
};

struct GorillaTagger : Behaviour {
    static Class GetClass() {
        return Class(O(""), O("GorillaTagger"));
    }

    static GorillaTagger* get_Instance() {
        static Method<GorillaTagger*> get_Instance = GetClass().GetMethod(O("get_Instance"));
        return get_Instance();
    }

    float GetSphereCastRadius() {
        static Field<float> f = GetClass().GetField(O("sphereCastRadius"));
        f.SetInstance(this);
        return f();
    }
    void SetSphereCastRadius(float value) {
        static Field<float> f = GetClass().GetField(O("sphereCastRadius"));
        f.SetInstance(this);
        f = value;
    }

    bool GetInCosmeticsRoom() {
        static Field<bool> f = GetClass().GetField(O("inCosmeticsRoom"));
        f.SetInstance(this);
        return f();
    }
    void SetInCosmeticsRoom(bool value) {
        static Field<bool> f = GetClass().GetField(O("inCosmeticsRoom"));
        f.SetInstance(this);
        f = value;
    }

    Vector3 GetLastLeftHandPositionForTag() {
        static Field<Vector3> f = GetClass().GetField(O("lastLeftHandPositionForTag"));
        f.SetInstance(this);
        return f();
    }
    void SetLastLeftHandPositionForTag(Vector3 value) {
        static Field<Vector3> f = GetClass().GetField(O("lastLeftHandPositionForTag"));
        f.SetInstance(this);
        f = value;
    }

    Vector3 GetLastRightHandPositionForTag() {
        static Field<Vector3> f = GetClass().GetField(O("lastRightHandPositionForTag"));
        f.SetInstance(this);
        return f();
    }
    void SetLastRightHandPositionForTag(Vector3 value) {
        static Field<Vector3> f = GetClass().GetField(O("lastRightHandPositionForTag"));
        f.SetInstance(this);
        f = value;
    }

    Vector3 GetLastBodyPositionForTag() {
        static Field<Vector3> f = GetClass().GetField(O("lastBodyPositionForTag"));
        f.SetInstance(this);
        return f();
    }
    void SetLastBodyPositionForTag(Vector3 value) {
        static Field<Vector3> f = GetClass().GetField(O("lastBodyPositionForTag"));
        f.SetInstance(this);
        f = value;
    }

    Vector3 GetLastHeadPositionForTag() {
        static Field<Vector3> f = GetClass().GetField(O("lastHeadPositionForTag"));
        f.SetInstance(this);
        return f();
    }
    void SetLastHeadPositionForTag(Vector3 value) {
        static Field<Vector3> f = GetClass().GetField(O("lastHeadPositionForTag"));
        f.SetInstance(this);
        f = value;
    }

    Transform* GetRightHandTransform() {
        static Field<Transform*> f = GetClass().GetField(O("rightHandTransform"));
        f.SetInstance(this);
        return f();
    }
    void SetRightHandTransform(Transform* value) {
        static Field<Transform*> f = GetClass().GetField(O("rightHandTransform"));
        f.SetInstance(this);
        f = value;
    }

    Transform* GetLeftHandTransform() {
        static Field<Transform*> f = GetClass().GetField(O("leftHandTransform"));
        f.SetInstance(this);
        return f();
    }
    void SetLeftHandTransform(Transform* value) {
        static Field<Transform*> f = GetClass().GetField(O("leftHandTransform"));
        f.SetInstance(this);
        f = value;
    }

    float GetHapticWaitSeconds() {
        static Field<float> f = GetClass().GetField(O("hapticWaitSeconds"));
        f.SetInstance(this);
        return f();
    }
    void SetHapticWaitSeconds(float value) {
        static Field<float> f = GetClass().GetField(O("hapticWaitSeconds"));
        f.SetInstance(this);
        f = value;
    }

    float GetHandTapVolume() {
        static Field<float> f = GetClass().GetField(O("handTapVolume"));
        f.SetInstance(this);
        return f();
    }
    void SetHandTapVolume(float value) {
        static Field<float> f = GetClass().GetField(O("handTapVolume"));
        f.SetInstance(this);
        f = value;
    }

    float GetTapCooldown() {
        static Field<float> f = GetClass().GetField(O("tapCoolDown"));
        f.SetInstance(this);
        return f();
    }
    void SetTapCooldown(float value) {
        static Field<float> f = GetClass().GetField(O("tapCoolDown"));
        f.SetInstance(this);
        f = value;
    }

    float GetLastLeftTap() {
        static Field<float> f = GetClass().GetField(O("lastLeftTap"));
        f.SetInstance(this);
        return f();
    }
    void SetLastLeftTap(float value) {
        static Field<float> f = GetClass().GetField(O("lastLeftTap"));
        f.SetInstance(this);
        f = value;
    }

    float GetLastRightTap() {
        static Field<float> f = GetClass().GetField(O("lastRightTap"));
        f.SetInstance(this);
        return f();
    }
    void SetLastRightTap(float value) {
        static Field<float> f = GetClass().GetField(O("lastRightTap"));
        f.SetInstance(this);
        f = value;
    }

    float GetTapHapticDuration() {
        static Field<float> f = GetClass().GetField(O("tapHapticDuration"));
        f.SetInstance(this);
        return f();
    }
    void SetTapHapticDuration(float value) {
        static Field<float> f = GetClass().GetField(O("tapHapticDuration"));
        f.SetInstance(this);
        f = value;
    }

    float GetTapHapticStrength() {
        static Field<float> f = GetClass().GetField(O("tapHapticStrength"));
        f.SetInstance(this);
        return f();
    }
    void SetTapHapticStrength(float value) {
        static Field<float> f = GetClass().GetField(O("tapHapticStrength"));
        f.SetInstance(this);
        f = value;
    }

    float GetTagHapticDuration() {
        static Field<float> f = GetClass().GetField(O("tagHapticDuration"));
        f.SetInstance(this);
        return f();
    }
    void SetTagHapticDuration(float value) {
        static Field<float> f = GetClass().GetField(O("tagHapticDuration"));
        f.SetInstance(this);
        f = value;
    }

    float GetTagHapticStrength() {
        static Field<float> f = GetClass().GetField(O("tagHapticStrength"));
        f.SetInstance(this);
        return f();
    }
    void SetTagHapticStrength(float value) {
        static Field<float> f = GetClass().GetField(O("tagHapticStrength"));
        f.SetInstance(this);
        f = value;
    }

    float GetTaggedHapticDuration() {
        static Field<float> f = GetClass().GetField(O("taggedHapticDuration"));
        f.SetInstance(this);
        return f();
    }
    void SetTaggedHapticDuration(float value) {
        static Field<float> f = GetClass().GetField(O("taggedHapticDuration"));
        f.SetInstance(this);
        f = value;
    }

    float GetTaggedHapticStrength() {
        static Field<float> f = GetClass().GetField(O("taggedHapticStrength"));
        f.SetInstance(this);
        return f();
    }
    void SetTaggedHapticStrength(float value) {
        static Field<float> f = GetClass().GetField(O("taggedHapticStrength"));
        f.SetInstance(this);
        f = value;
    }

    bool GetLeftHandTouching() {
        static Field<bool> f = GetClass().GetField(O("leftHandTouching"));
        f.SetInstance(this);
        return f();
    }
    void SetLeftHandTouching(bool value) {
        static Field<bool> f = GetClass().GetField(O("leftHandTouching"));
        f.SetInstance(this);
        f = value;
    }

    bool GetRightHandTouching() {
        static Field<bool> f = GetClass().GetField(O("rightHandTouching"));
        f.SetInstance(this);
        return f();
    }
    void SetRightHandTouching(bool value) {
        static Field<bool> f = GetClass().GetField(O("rightHandTouching"));
        f.SetInstance(this);
        f = value;
    }

    float GetTaggedTime() {
        static Field<float> f = GetClass().GetField(O("taggedTime"));
        f.SetInstance(this);
        return f();
    }
    void SetTaggedTime(float value) {
        static Field<float> f = GetClass().GetField(O("taggedTime"));
        f.SetInstance(this);
        f = value;
    }

    float GetTagCooldown() {
        static Field<float> f = GetClass().GetField(O("tagCooldown"));
        f.SetInstance(this);
        return f();
    }
    void SetTagCooldown(float value) {
        static Field<float> f = GetClass().GetField(O("tagCooldown"));
        f.SetInstance(this);
        f = value;
    }

    VRRig* GetMyVRRig() {
        static Field<VRRig*> f = GetClass().GetField(O("myVRRig"));
        if (!f.IsValid()) {
            f = GetClass().GetField(O("offlineVRRig"));
        }
        f.SetInstance(this);
        return f();
    }

    VRRig* GetOfflineVRRig() {
        static Field<VRRig*> f = GetClass().GetField(O("offlineVRRig"));
        f.SetInstance(this);
        return f();
    }
};