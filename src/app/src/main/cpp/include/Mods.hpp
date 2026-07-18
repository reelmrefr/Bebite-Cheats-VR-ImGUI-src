#include "BNMResolve.hpp"
#include "XRInput.hpp"
#include "PhotonResolve.hpp"
//#include "GorillaLocomotion.hpp"
#include "SDK/Includes/VRRigData.h"
#include "SDK/Includes/VRMap.h"
#include "SDK/Includes/VRRig.h"
#include "SDK/Includes/VRRigAnchorOverrides.h"
#include "SDK/Includes/VRRigReliableState.h"
#include "SDK/Includes/GorillaParent.h"
#include "SDK/Includes/GorillaTagger.h"
#include "SDK/Includes/GorillaGameManager.h"

namespace PhotonMods{
    void SetMas(){
        PhotonNetwork::SetMasterClient(PhotonNetwork::GetLocalPlayer());
    }
    void CrashA(){
        if (XRInput::GetBoolFeature(BoolFeature::TriggerButton, Controller::Right)){
            PhotonNetwork::SetMasterClient(PhotonNetwork::GetLocalPlayer());
            for (int i = 0; i < 100; ++i){
                PhotonNetwork::DestroyAll();
                PhotonNetwork::DestroyAll();
                PhotonNetwork::DestroyAll();
            }
        }
    }
    void Soofid(){
        PhotonNetwork::GetLocalPlayer()->SetUserId("BebiteCheats");
    }
    void VibA(){
        for (Player* pler : PhotonNetwork::GetPlayerListOthers()){
            GlobalNamespace::GorillaGameManager* gmaes = GlobalNamespace::GorillaGameManager::instance();
            PhotonView* photonview = (PhotonView*)gmaes->FindVRRigForPlayer(pler);
            photonview->RPC("SetJoinTaggedTime", pler, nullptr);
        }
    }
    void SlowA(){
        for (Player* pler : PhotonNetwork::GetPlayerListOthers()){
            GlobalNamespace::GorillaGameManager* gmaes = GlobalNamespace::GorillaGameManager::instance();
            PhotonView* photonview = (PhotonView*)gmaes->FindVRRigForPlayer(pler);
            photonview->RPC("SetTaggedTime", pler, nullptr);
        }
    }
}

namespace visual{

    void tracers(){
        Transform* someotho = (Transform*)GlobalNamespace::GorillaTagger::get_Instance()->rightHandTransform();
        GlobalNamespace::GorillaParent* parent = GlobalNamespace::GorillaParent::instance();
        auto rigs = parent->vrrigs<BNM::Structures::Mono::List<GlobalNamespace::VRRig*>*>();
        if (PhotonNetwork::GetInRoom()){
            for (auto rig : rigs->ToVector()){
                if (!rig->isMyPlayer()){
                    continue;
                    Transform* jeij = (Transform*)rig->rightHandTransform();
                    Color tracerC = Color::blue;
                    GameObject* holder = (GameObject*)GameObject::GetClass().CreateNewObjectParameters("handthing");
                    LineRenderer* line = (LineRenderer*)holder->AddComponent(LineRenderer::GetType());
                    line->SetUseWorldScape(true);
                    line->GetMaterial()->SetShader(Shader::Find("GUI/Text Shader"));
                    line->SetPositionCount(2);
                    line->SetStartWidth(0.02f);
                    line->SetEndWidth(0.02f);
                    line->SetStartColor(tracerC);
                    line->SetEndColor(tracerC);
                    line->SetPosition(0, someotho->GetPosition());
                    line->SetPosition(1, jeij->GetPosition());
                    GameObject::Destroy(holder, Time::GetDeltaTime());
                }
            }
        }
    }
    void place(){
        //
    }
}