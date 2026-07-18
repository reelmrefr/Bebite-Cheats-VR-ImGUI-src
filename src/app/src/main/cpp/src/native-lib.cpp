#include <jni.h>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include <cstring>
#include <cfloat>
#include <algorithm>
#include <map>

#include "BNM/Loading.hpp"
#include "BNMIncludes.hpp"

#include "imgui.h"
#include "czrender.hpp"
#include "XRInput.hpp"

#include "Mods.hpp"
#include "GorillaLocomotion.hpp"

// cz was here

static constexpr int kfbwidth  = 384;
static constexpr int kfbheight = 512;

static constexpr float kpanelmetersw = 0.18f;
static constexpr float kpanelmetersh = 0.24f;

static constexpr float khoverdist     = 0.40f;
static constexpr float ktriggerthresh = 0.55f;

struct czstate {
    bool initialized = false;

    ImGuiContext*    ctx        = nullptr;
    cz::framebuffer  framebuf {};
    cz::fontatlas    atlas {};

    GameObject* quad = nullptr;
    Texture2D*  tex  = nullptr;
    Material*   mat  = nullptr;

    Vector3    localoffset    {0.0f, 0.10f, 0.04f};
    Quaternion localrotoffset = Quaternion::FromEuler(65.0f, 0.0f, 0.0f);

    bool menuopen       = false;
    bool prevshowheld   = false;

    bool movemode       = false;
    bool dragging       = false;
    Vector3 draggrablocal {};
    Vector3 dragstartoffset {};

    bool  speed     = false;
    bool  longarms  = false;
    float speedmul  = 1.5f;
    float armreach  = 4.0f;

    bool prevclicked = false;
    float animprogress = 0.0f;

    BNM::Class playercls;

    BNM::FieldBase jumpmultiplierfield;
    BNM::FieldBase maxarmlengthfield;
    bool  origcached         = false;
    float origjumpmultiplier = 0.0f;
    float origmaxarmlength   = 1.5f;
} gcz;

namespace gtag {
    static BNM::Class playercls() {
        static BNM::Class c = BNM::Class("GorillaLocomotion", "Player");
        return c;
    }
    static void* player() {
        if (!playercls()) return nullptr;
        static Method<void*> m = playercls().GetMethod("get_Instance");
        return m.IsValid() ? m() : nullptr;
    }

    struct fcacheentry { const char* name; BNM::FieldBase fb; };
    static fcacheentry fcache[32];
    static int fcache_n = 0;

    static BNM::FieldBase cachedfield(const char* name) {
        for (int i = 0; i < fcache_n; ++i) {
            if (fcache[i].name == name) return fcache[i].fb;
        }
        for (int i = 0; i < fcache_n; ++i) {
            if (std::strcmp(fcache[i].name, name) == 0) return fcache[i].fb;
        }
        BNM::FieldBase fb = playercls().GetField(name);
        if (fcache_n < 32) fcache[fcache_n++] = { name, fb };
        return fb;
    }

    static void setpf(const char* name, float val) {
        void* p = player();
        if (!p) return;
        auto fb = cachedfield(name);
        if (!fb.IsValid()) return;
        BNM::Field<float> f = fb;
        f[p].Set(val);
    }
    static float getpf(const char* name) {
        void* p = player();
        if (!p) return 0;
        auto fb = cachedfield(name);
        if (!fb.IsValid()) return 0;
        BNM::Field<float> f = fb;
        return f[p].Get();
    }
    static void setpb(const char* name, bool val) {
        void* p = player();
        if (!p) return;
        auto fb = cachedfield(name);
        if (!fb.IsValid()) return;
        BNM::Field<bool> f = fb;
        f[p].Set(val);
    }
    static void setpv3(const char* name, Vector3 val) {
        void* p = player();
        if (!p) return;
        auto fb = cachedfield(name);
        if (!fb.IsValid()) return;
        BNM::Field<Vector3> f = fb;
        f[p].Set(val);
    }
    static Transform* head() {
        void* p = player();
        if (!p) return nullptr;
        static BNM::FieldBase fb = cachedfield("headCollider");
        if (!fb.IsValid()) return nullptr;
        BNM::Field<Component*> f = fb;
        Component* c = f[p].Get();
        return c ? c->GetTransform() : nullptr;
    }
    static Transform* body() {
        void* p = player();
        if (!p) return nullptr;
        static BNM::FieldBase fb = cachedfield("bodyCollider");
        if (!fb.IsValid()) return nullptr;
        BNM::Field<Component*> f = fb;
        Component* c = f[p].Get();
        return c ? c->GetTransform() : nullptr;
    }
    static Rigidbody* rb() {
        void* p = player();
        if (!p) return nullptr;
        static BNM::FieldBase fb = cachedfield("playerRigidBody");
        if (!fb.IsValid()) return nullptr;
        BNM::Field<Rigidbody*> f = fb;
        return f[p].Get();
    }
    static GameObject* rootobj_cached = nullptr;
    static int          rootobj_cd     = 0;
    static Transform* roottrans() {
        if (!rootobj_cached || rootobj_cd <= 0) {
            rootobj_cached = GameObject::Find("GorillaPlayer");
            rootobj_cd = 600;
        }
        rootobj_cd--;
        return rootobj_cached ? rootobj_cached->GetTransform() : nullptr;
    }
}

struct cmod {
    const char* category;
    const char* name;
    bool        enabled    = false;
    bool        wasenabled = false;
    bool        oneshot    = false;
    void      (*tick)()    = nullptr;
};
static std::vector<cmod> g_mods;

static void addtick(const char* cat, const char* name, void(*fn)()) {
    g_mods.push_back({cat, name, false, false, false, fn});
}
static void addoneshot(const char* cat, const char* name, void(*fn)()) {
    g_mods.push_back({cat, name, false, false, true, fn});
}

struct pickedshader {
    Shader* shader = nullptr;
    bool    isurp  = false;
    const char* name = "";
    bool    intrinsictransparent = false;
};

static pickedshader findbestshader() {
    struct c { const char* name; bool urp; bool intrinsic; };
    c candidates[] = {
        {"Universal Render Pipeline/Unlit",      true,  false},
        {"Universal Render Pipeline/Simple Lit", true,  false},
        {"Universal Render Pipeline/Lit",        true,  false},
        {"Unlit/Transparent",                    false, true },
        {"Sprites/Default",                      false, true },
        {"UI/Default",                           false, true },
        {"Mobile/Particles/Alpha Blended",       false, true },
        {"Particles/Alpha Blended",              false, true },
        {"Hidden/Internal-Colored",              false, false},
    };
    for (auto& cand : candidates) {
        Shader* s = Shader::Find(cand.name);
        if (s) return {s, cand.urp, cand.name, cand.intrinsic};
    }
    return {nullptr, false, "", false};
}

static void matsetfloat(Material* mat, const char* name, float v) {
    static Method<void> m = Material::GetClass().GetMethod("SetFloat", {"name","value"});
    if (m.IsValid()) m[mat](BNM::CreateMonoString(name), v);
}

static void matenablekeyword(Material* mat, const char* name) {
    static Method<void> m = Material::GetClass().GetMethod("EnableKeyword", {"keyword"});
    if (m.IsValid()) m[mat](BNM::CreateMonoString(name));
}

static void matdisablekeyword(Material* mat, const char* name) {
    static Method<void> m = Material::GetClass().GetMethod("DisableKeyword", {"keyword"});
    if (m.IsValid()) m[mat](BNM::CreateMonoString(name));
}

static void matsetoverridetag(Material* mat, const char* tag, const char* val) {
    static Method<void> m = Material::GetClass().GetMethod("SetOverrideTag", {"tag","val"});
    if (m.IsValid()) m[mat](BNM::CreateMonoString(tag), BNM::CreateMonoString(val));
}

static void matsetrenderqueue(Material* mat, int q) {
    static Method<void> m = Material::GetClass().GetMethod("set_renderQueue", 1);
    if (m.IsValid()) m[mat](q);
}

static void matsettexturebyname(Material* mat, const char* name, void* tex) {
    static Method<void> m = Material::GetClass().GetMethod("SetTexture", {"name","value"});
    if (m.IsValid()) m[mat](BNM::CreateMonoString(name), tex);
}

static void matsettexturescalebyname(Material* mat, const char* name, Vector2 s) {
    static Method<void> m = Material::GetClass().GetMethod("SetTextureScale", {"name","value"});
    if (m.IsValid()) m[mat](BNM::CreateMonoString(name), s);
}

static void matsettextureoffsetbyname(Material* mat, const char* name, Vector2 o) {
    static Method<void> m = Material::GetClass().GetMethod("SetTextureOffset", {"name","value"});
    if (m.IsValid()) m[mat](BNM::CreateMonoString(name), o);
}

static void matsetmaintexture(Material* mat, Texture2D* tex) {
    static Method<void> m = Material::GetClass().GetMethod("set_mainTexture", 1);
    if (m.IsValid()) m[mat]((Object*)tex);
    matsettexturebyname(mat, "_MainTex", tex);
    matsettexturebyname(mat, "_BaseMap", tex);
}

static void matsetscaleandoffset(Material* mat, float sx, float sy, float ox, float oy) {
    Vector2 s(sx, sy), o(ox, oy);
    static Method<void> setscale  = Material::GetClass().GetMethod("set_mainTextureScale",  1);
    static Method<void> setoffset = Material::GetClass().GetMethod("set_mainTextureOffset", 1);
    if (setscale.IsValid())  setscale[mat](s);
    if (setoffset.IsValid()) setoffset[mat](o);
    matsettexturescalebyname (mat, "_MainTex", s);
    matsettextureoffsetbyname(mat, "_MainTex", o);
    matsettexturescalebyname (mat, "_BaseMap", s);
    matsettextureoffsetbyname(mat, "_BaseMap", o);
}

static void configurematerialtransparent(Material* mat, const pickedshader& sh) {
    matsetoverridetag(mat, "RenderType", "Transparent");
    matsetrenderqueue(mat, 3000);
    if (sh.isurp) {
        matsetfloat(mat, "_Surface",   1.0f);
        matsetfloat(mat, "_Blend",     0.0f);
        matsetfloat(mat, "_ZWrite",    0.0f);
        matsetfloat(mat, "_AlphaClip", 0.0f);
        matsetfloat(mat, "_SrcBlend",  5.0f);
        matsetfloat(mat, "_DstBlend", 10.0f);
        matsetfloat(mat, "_Cull",      0.0f);
        matdisablekeyword(mat, "_SURFACE_TYPE_OPAQUE");
        matenablekeyword (mat, "_SURFACE_TYPE_TRANSPARENT");
        matdisablekeyword(mat, "_ALPHATEST_ON");
        matdisablekeyword(mat, "_ALPHAPREMULTIPLY_ON");
        matenablekeyword (mat, "_ALPHABLEND_ON");
        return;
    }
    if (sh.intrinsictransparent) return;
    matsetfloat(mat, "_Mode",     3.0f);
    matsetfloat(mat, "_SrcBlend", 5.0f);
    matsetfloat(mat, "_DstBlend",10.0f);
    matsetfloat(mat, "_ZWrite",   0.0f);
    matsetfloat(mat, "_Cull",     0.0f);
    matdisablekeyword(mat, "_ALPHATEST_ON");
    matdisablekeyword(mat, "_ALPHAPREMULTIPLY_ON");
    matenablekeyword (mat, "_ALPHABLEND_ON");
}

static Material* creatematerial(const pickedshader& sh) {
    if (!sh.shader) return nullptr;
    Material* m = (Material*)Material::GetClass().CreateNewObjectParameters(sh.shader);
    if (!m) return nullptr;
    GameObject::DontDestroyOnLoad((Object*)m);
    return m;
}

static Texture2D* creatergba32texture(int w, int h) {
    Texture2D* tex = (Texture2D*)Texture2D::GetClass().CreateNewObjectParameters(w, h);
    if (!tex) { BNM_LOG_INFO("BebiteCheats: tex null"); return nullptr; }

    static Method<bool> reinit_named = Texture2D::GetClass().GetMethod("Reinitialize",
        {"width","height","textureFormat","hasMipMap"});
    static Method<bool> reinit_count = Texture2D::GetClass().GetMethod("Reinitialize", 4);
    static Method<bool> resize_named = Texture2D::GetClass().GetMethod("Resize",
        {"width","height","format","hasMipMap"});
    static Method<bool> resize_count = Texture2D::GetClass().GetMethod("Resize", 4);
    if (reinit_named.IsValid()) {
        reinit_named[tex](w, h, (int)TextureFormat::RGBA32, false);
        BNM_LOG_INFO("BebiteCheats: tex Reinitialize (named) ok");
    } else if (reinit_count.IsValid()) {
        reinit_count[tex](w, h, (int)TextureFormat::RGBA32, false);
        BNM_LOG_INFO("BebiteCheats: tex Reinitialize (count) ok");
    } else if (resize_named.IsValid()) {
        resize_named[tex](w, h, (int)TextureFormat::RGBA32, false);
        BNM_LOG_INFO("BebiteCheats: tex Resize (named) ok");
    } else if (resize_count.IsValid()) {
        resize_count[tex](w, h, (int)TextureFormat::RGBA32, false);
        BNM_LOG_INFO("BebiteCheats: tex Resize (count) ok");
    } else {
        BNM_LOG_INFO("BebiteCheats: NO reinit/resize available");
    }

    static Method<void> set_filtermode = BNM::Class("UnityEngine","Texture").GetMethod("set_filterMode", 1);
    if (set_filtermode.IsValid()) set_filtermode[tex](0);

    GameObject::DontDestroyOnLoad((Object*)tex);
    return tex;
}

static void textureloadrawandapply(Texture2D* tex, const uint8_t* src, int w, int h) {
    if (!tex || !src || w <= 0 || h <= 0) return;
    size_t rowbytes = (size_t)w * 4;
    size_t total    = rowbytes * (size_t)h;
    auto* arr = Array<uint8_t>::Create(total);
    if (!arr) return;
    uint8_t* dst = arr->GetData();
    if (!dst) return;
    for (int y = 0; y < h; ++y) {
        std::memcpy(dst + (size_t)y * rowbytes,
                    src + (size_t)(h - 1 - y) * rowbytes,
                    rowbytes);
    }

    static Method<void> loadraw = Texture2D::GetClass().GetMethod("LoadRawTextureData", {"data"});
    static Method<void> apply2  = Texture2D::GetClass().GetMethod("Apply", {"updateMipmaps","makeNoLongerReadable"});
    static Method<void> apply0  = Texture2D::GetClass().GetMethod("Apply", 0);
    try {
        if (loadraw.IsValid()) loadraw[tex](arr);
        if (apply2.IsValid())       apply2[tex](false, false);
        else if (apply0.IsValid())  apply0[tex]();
    } catch (...) {
        BNM_LOG_INFO("BebiteCheats: upload threw");
    }
}

namespace czstyle {
    static ImGuiStyle& s() { return ImGui::GetStyle(); }
    static ImVec4*    cols() { return s().Colors; }

    static void background  (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_WindowBg]       = ImVec4(r,g,b,a); }
    static void titlecolor  (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_TitleBg]       = ImVec4(r,g,b,a*0.9f); cols()[ImGuiCol_TitleBgActive] = ImVec4(r,g,b,a); }
    static void accent      (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_ButtonHovered] = ImVec4(r,g,b,a); cols()[ImGuiCol_HeaderHovered] = ImVec4(r,g,b,a); cols()[ImGuiCol_FrameBgHovered] = ImVec4(r,g,b,a); cols()[ImGuiCol_SliderGrab] = ImVec4(r,g,b,a); }
    static void buttoncolor (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_Button]        = ImVec4(r,g,b,a); }
    static void framecolor  (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_FrameBg]       = ImVec4(r,g,b,a); }
    static void headercolor (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_Header]        = ImVec4(r,g,b,a); }
    static void textcolor   (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_Text]          = ImVec4(r,g,b,a); }
    static void bordercolor (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_Border]        = ImVec4(r,g,b,a); cols()[ImGuiCol_Separator] = ImVec4(r,g,b,a*0.8f); }
    static void checkmark   (float r, float g, float b, float a = 1.0f) { cols()[ImGuiCol_CheckMark]     = ImVec4(r,g,b,a); }

    static void rounding    (float r)        { s().WindowRounding = r; s().ChildRounding = r * 0.7f; s().FrameRounding = r * 0.55f; s().GrabRounding = r * 0.55f; s().PopupRounding = r * 0.7f; s().ScrollbarRounding = r * 0.55f; }
    static void bordersize  (float px)       { s().WindowBorderSize = px; }
    static void padding     (float x, float y) { s().WindowPadding = ImVec2(x,y); s().FramePadding = ImVec2(x * 0.85f, y * 0.6f); }
    static void spacing     (float x, float y) { s().ItemSpacing = ImVec2(x,y); }
    static void fontscale   (float scale)    { ImGui::GetIO().FontGlobalScale = scale; }
}

static void registermods() {
    if (!g_mods.empty()) return;

    addoneshot("Movement", "Speed Boost", []() {
        gtag::setpf("jumpMultiplier", 2.25f);
        gtag::setpf("maxJumpSpeed", 999.0f);
    });
    addoneshot("Movement", "BIG Speed Boost", []() {
        gtag::setpf("jumpMultiplier", 7.5f);
        gtag::setpf("maxJumpSpeed", 999.0f);
    });
    addoneshot("Movement", "Fix Speed", []() {
        gtag::setpf("jumpMultiplier", 1.1f);
        gtag::setpf("maxJumpSpeed", 6.5f);
    });
    addoneshot("Movement", "Long Arms", []() {
        gtag::setpf("maxArmLength", 999.9f);
    });
    addoneshot("Movement", "Fix Arms", []() {
        gtag::setpf("maxArmLength", 1.5f);
    });

    addtick("Movement", "Fly (R-Trigger)", []() {
        float t = XRInput::GetFloatFeature(Trigger, Right);
        if (t < 0.5f) return;
        Transform* h = gtag::head();
        Transform* r = gtag::roottrans();
        Rigidbody* body = gtag::rb();
        if (!h || !r) return;
        Vector3 step = h->GetForward() * Time::GetDeltaTime() * 12.0f;
        r->SetPosition(r->GetPosition() + step);
        if (body) body->SetVelocity(Vector3(0,0,0));
    });
    addtick("Movement", "Bark Fly (L-Trigger)", []() {
        if (!XRInput::GetBoolFeature(TriggerButton, Left)) return;
        Rigidbody* body = gtag::rb();
        Transform* h = gtag::head();
        if (!body || !h) return;
        Vector3 fwd = h->GetForward();
        fwd.y = 0; fwd = Vector3::Normalize(fwd);
        body->AddForce(fwd * 7.0f * Time::GetDeltaTime() * 35.0f, ForceMode::VelocityChange);
    });
    addtick("Movement", "Bounce (L-Trigger)", []() {
        if (!XRInput::GetBoolFeature(TriggerButton, Left)) return;
        Rigidbody* body = gtag::rb();
        if (body) body->AddForce(Vector3(0, 3.5f, 0), ForceMode::VelocityChange);
    });
    addtick("Movement", "Up + Down (Triggers)", []() {
        Rigidbody* body = gtag::rb();
        if (!body) return;
        float dt = Time::GetDeltaTime();
        if (XRInput::GetFloatFeature(Trigger, Right) > 0.5f)
            body->SetVelocity(body->GetVelocity() + Vector3(0, 1, 0) * dt * 30.0f);
        if (XRInput::GetFloatFeature(Trigger, Left) > 0.5f)
            body->SetVelocity(body->GetVelocity() + Vector3(0,-1, 0) * dt * 30.0f);
    });
    addtick("Movement", "Gorilla Car (Triggers)", []() {
        Rigidbody* body = gtag::rb();
        Transform* b = gtag::body();
        if (!body || !b) return;
        float dt = Time::GetDeltaTime();
        if (XRInput::GetFloatFeature(Trigger, Right) > 0.5f)
            body->SetVelocity(body->GetVelocity() + b->GetForward() * dt * 30.0f);
        if (XRInput::GetFloatFeature(Trigger, Left) > 0.5f)
            body->SetVelocity(body->GetVelocity() - b->GetForward() * dt * 30.0f);
    });
    addtick("Movement", "PSA (R-Primary)", []() {
        if (!XRInput::GetBoolFeature(PrimaryButton, Right)) return;
        Transform* b = gtag::body();
        Transform* r = gtag::roottrans();
        if (!b || !r) return;
        r->SetPosition(r->GetPosition() + b->GetForward() * Time::GetDeltaTime() * 5.5f);
    });
    addtick("Movement", "Slow PSA (R-Primary)", []() {
        if (!XRInput::GetBoolFeature(PrimaryButton, Right)) return;
        Transform* b = gtag::body();
        Transform* r = gtag::roottrans();
        if (!b || !r) return;
        r->SetPosition(r->GetPosition() + b->GetForward() * Time::GetDeltaTime() * 2.5f);
    });
    addtick("Movement", "Joystick Fly", []() {
        Rigidbody* body = gtag::rb();
        Transform* b = gtag::body();
        if (!body || !b) return;
        body->AddForce(Physics::GetGravity() * -1.0f, ForceMode::Acceleration);
        Vector2 lj = XRInput::GetVector2Feature(Primary2DAxis, Left);
        Vector2 rj = XRInput::GetVector2Feature(Primary2DAxis, Right);
        Vector3 fwd = b->GetForward(); fwd.y = 0;
        Vector3 rgh = b->GetRight();   rgh.y = 0;
        Vector3 vel = rgh * lj.x + Vector3(0,1,0) * rj.y + fwd * lj.y;
        vel *= 10.0f;
        Vector3 cur = body->GetVelocity();
        body->SetVelocity(Vector3(cur.x + (vel.x - cur.x) * 0.12f,
                                  cur.y + (vel.y - cur.y) * 0.12f,
                                  cur.z + (vel.z - cur.z) * 0.12f));
    });

    addoneshot("Gravity", "Zero Gravity",   []() { Physics::SetGravity(Vector3(0,  0,    0)); });
    addoneshot("Gravity", "Low Gravity",    []() { Physics::SetGravity(Vector3(0, -5,    0)); });
    addoneshot("Gravity", "High Gravity",   []() { Physics::SetGravity(Vector3(0,-50,    0)); });
    addoneshot("Gravity", "Normal Gravity", []() { Physics::SetGravity(Vector3(0, -9.81f,0)); });

    addtick("Body", "Force Tag Freeze", []() { gtag::setpb("disableMovement", true); });
    addtick("Body", "No Tag Freeze",    []() { gtag::setpb("disableMovement", false); });
    addtick("Body", "Slide Control",    []() { gtag::setpf("defaultSlideFactor", 1.0f); });
    addtick("Body", "Headless", []() {
        Transform* h = gtag::head();
        if (h) h->SetLocalScale(Vector3(0,0,0));
    });
    addoneshot("Body", "Fix Head", []() {
        Transform* h = gtag::head();
        if (h) h->SetLocalScale(Vector3(1,1,1));
    });
    addtick("Body", "Backwards Head", []() {
        Transform* h = gtag::head();
        if (!h) return;
        h->SetLocalRotation(Quaternion::FromEuler(180.0f, 0.0f, 0.0f));
    });

    addtick("Hands", "Stick Long Arms", []() {
        gtag::setpv3("rightHandOffset", Vector3(0, 0, 0.35f));
        gtag::setpv3("leftHandOffset",  Vector3(0, 0, 0.35f));
    });
    addoneshot("Hands", "Fix Stick Arms", []() {
        gtag::setpv3("rightHandOffset", Vector3(0, 0, 0));
        gtag::setpv3("leftHandOffset",  Vector3(0, 0, 0));
    });

    addoneshot("Photon", "Set Master", []() {
       PhotonMods::SetMas();
    });
    addtick("Photon", "Crash All(RT)", []() {
        PhotonMods::CrashA();
    });
    addoneshot("Photon", "Spoof Id", []() {
        PhotonMods::Soofid();
    });
    addtick("Photon", "Vibrate All", []() {
        PhotonMods::VibA();
    });
    addtick("Photon", "Slow All", []() {
        PhotonMods::SlowA();
    });

    addtick("Visual", "Tracers", []() {
       visual::tracers();
    });

    addoneshot("Credits", "Mr.E", []() {});
}

static void initimgui() {
    if (gcz.ctx) return;
    IMGUI_CHECKVERSION();
    gcz.ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(gcz.ctx);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)kfbwidth, (float)kfbheight);
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.DeltaTime = 1.0f / 72.0f;
    io.MouseDrawCursor = true;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.ConfigFlags  |= ImGuiConfigFlags_NoMouseCursorChange;
    io.FontGlobalScale = 1.4f;

    io.Fonts->AddFontDefault();

    unsigned char* pixels = nullptr;
    int aw = 0, ah = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &aw, &ah);
    gcz.atlas.w = aw;
    gcz.atlas.h = ah;
    size_t bytes = (size_t)aw * ah * 4;
    uint8_t* copy = (uint8_t*)std::malloc(bytes);
    std::memcpy(copy, pixels, bytes);
    gcz.atlas.px = copy;
    io.Fonts->SetTexID((ImTextureID)(intptr_t)1);

    gcz.framebuf.w = kfbwidth;
    gcz.framebuf.h = kfbheight;
    gcz.framebuf.px = (uint8_t*)std::calloc((size_t)kfbwidth * kfbheight * 4, 1);

    czstyle::rounding    (14.0f);
    czstyle::bordersize  (1.0f);
    czstyle::padding     (12.0f, 12.0f);
    czstyle::spacing     (10.0f, 10.0f);
    czstyle::fontscale   (1.4f);

    czstyle::background  (0.06f, 0.07f, 0.10f, 0.78f);
    czstyle::bordercolor (0.35f, 0.55f, 0.95f, 0.55f);
    czstyle::titlecolor  (0.15f, 0.40f, 0.85f, 0.95f);
    czstyle::headercolor (0.18f, 0.30f, 0.55f, 0.55f);
    czstyle::buttoncolor (0.20f, 0.24f, 0.32f, 0.85f);
    czstyle::framecolor  (0.12f, 0.14f, 0.20f, 0.85f);
    czstyle::accent      (0.30f, 0.55f, 0.95f, 0.95f);
    czstyle::checkmark   (0.30f, 0.95f, 0.50f, 1.00f);
    czstyle::textcolor   (0.95f, 0.96f, 1.00f, 1.00f);
}

static void renderersetmaterial(Renderer* r, Material* m) {
    static Method<void> set_sharedmat = BNM::Class("UnityEngine","Renderer").GetMethod("set_sharedMaterial", 1);
    static Method<void> set_mat       = BNM::Class("UnityEngine","Renderer").GetMethod("set_material", 1);
    if (set_sharedmat.IsValid()) { set_sharedmat[r]((Object*)m); BNM_LOG_INFO("BebiteCheats: set_sharedMaterial ok"); return; }
    if (set_mat.IsValid())       { set_mat[r]((Object*)m);       BNM_LOG_INFO("BebiteCheats: set_material ok");       return; }
    BNM_LOG_INFO("BebiteCheats: NO material setter found");
}

static int texwidth (Texture2D* t) {
    static Method<int> m = BNM::Class("UnityEngine","Texture").GetMethod("get_width", 0);
    return m.IsValid() ? m[t]() : -1;
}
static int texheight(Texture2D* t) {
    static Method<int> m = BNM::Class("UnityEngine","Texture").GetMethod("get_height", 0);
    return m.IsValid() ? m[t]() : -1;
}

static void buildpanel(Transform* parent) {
    if (gcz.quad) return;

    pickedshader sh = findbestshader();
    if (!sh.shader) { BNM_LOG_INFO("BebiteCheats: no shader found"); return; }
    BNM_LOG_INFO("BebiteCheats: shader '%s' (%s)", sh.name, sh.isurp ? "URP" : "BIRP");

    gcz.mat = creatematerial(sh);
    if (!gcz.mat) { BNM_LOG_INFO("BebiteCheats: mat null"); return; }

    gcz.tex = creatergba32texture(kfbwidth, kfbheight);
    if (!gcz.tex) { BNM_LOG_INFO("BebiteCheats: tex null"); return; }

    int tw = texwidth (gcz.tex);
    int th = texheight(gcz.tex);
    BNM_LOG_INFO("BebiteCheats: tex %dx%d", tw, th);
    if (tw <= 0 || th <= 0) {
        BNM_LOG_INFO("BebiteCheats: tex has no native backing, aborting panel build");
        gcz.tex = nullptr;
        return;
    }

    matsetmaintexture(gcz.mat, gcz.tex);
    matsetscaleandoffset(gcz.mat, 1.0f, 1.0f, 0.0f, 0.0f);
    configurematerialtransparent(gcz.mat, sh);

    gcz.quad = GameObject::CreatePrimitive(PrimitiveType::Quad);
    if (!gcz.quad) { BNM_LOG_INFO("BebiteCheats: quad null"); return; }
    gcz.quad->SetName("BebiteCheats panel");
    gcz.quad->SetActive(false);

    Transform* qt = gcz.quad->GetTransform();
    qt->SetParent(parent, false);
    qt->SetLocalPosition(gcz.localoffset);
    qt->SetLocalRotation(gcz.localrotoffset);
    qt->SetLocalScale(Vector3(kpanelmetersw, kpanelmetersh, 1.0f));

    auto* col = (Collider*)gcz.quad->GetComponent(Collider::GetType());
    if (col) col->SetEnabled(false);

    auto* mr = (MeshRenderer*)gcz.quad->GetComponent(MeshRenderer::GetType());
    if (mr) renderersetmaterial((Renderer*)mr, gcz.mat);
    BNM_LOG_INFO("BebiteCheats: panel built mr=%p mat=%p", (void*)mr, (void*)gcz.mat);

    size_t basesize = (size_t)kfbwidth * kfbheight * 4;
    if (gcz.framebuf.px) {
        for (size_t i = 0; i < basesize; i += 4) {
            gcz.framebuf.px[i+0] = 255;
            gcz.framebuf.px[i+1] = 40;
            gcz.framebuf.px[i+2] = 40;
            gcz.framebuf.px[i+3] = 255;
        }
        textureloadrawandapply(gcz.tex, gcz.framebuf.px, kfbwidth, kfbheight);
        BNM_LOG_INFO("BebiteCheats: red flash uploaded");
    }
}

static bool projectrighthandtoimgui(Transform* panelt, Vector3 rightpos,
                                    ImVec2& outmouse, float& outdepth) {
    Vector3 panelpos = panelt->GetPosition();
    Quaternion panelrot = panelt->GetRotation();
    Quaternion invrot = Quaternion::Inverse(panelrot);
    Vector3 local = invrot * (rightpos - panelpos);
    float halfw = kpanelmetersw * 0.5f;
    float halfh = kpanelmetersh * 0.5f;

    float u = (local.x + halfw) / kpanelmetersw;
    float v = 1.0f - (local.y + halfh) / kpanelmetersh;

    outmouse = ImVec2(u * (float)kfbwidth, v * (float)kfbheight);
    outdepth = local.z;
    bool over   = u >= -0.05f && u <= 1.05f && v >= -0.05f && v <= 1.05f;
    bool nearby = std::fabs(local.z) < khoverdist;
    return over && nearby;
}

static void buildui() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize
                           | ImGuiWindowFlags_NoMove
                           | ImGuiWindowFlags_NoCollapse
                           | ImGuiWindowFlags_NoSavedSettings
                           | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("BebiteCheats", nullptr, flags);

    if (ImGui::Button(gcz.movemode ? "drop here" : "move menu", ImVec2(-1, 28))) {
        gcz.movemode = !gcz.movemode;
        if (!gcz.movemode) gcz.dragging = false;
    }

    std::map<std::string, std::vector<int>> bycat;
    for (int i = 0; i < (int)g_mods.size(); ++i)
        bycat[g_mods[i].category].push_back(i);

    for (auto& kv : bycat) {
        if (kv.first == "Credits") continue;
        if (!ImGui::CollapsingHeader(kv.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            continue;
        for (int idx : kv.second) {
            cmod& m = g_mods[idx];
            ImGui::Checkbox(m.name, &m.enabled);
        }
    }

    if (bycat.count("Credits")) {
        if (ImGui::CollapsingHeader("Credits", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (int idx : bycat["Credits"]) {
                cmod& m = g_mods[idx];
                ImGui::Checkbox(m.name, &m.enabled);
            }
        }
    }

    ImGuiStyle& s = ImGui::GetStyle();
    if (ImGui::CollapsingHeader("settings")) {
        ImGui::SliderFloat("window rounding",  &s.WindowRounding, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat("frame rounding",   &s.FrameRounding,  0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("border thickness", &s.WindowBorderSize, 0.0f, 4.0f, "%.1f");
        ImGui::SliderFloat("font scale",       &io.FontGlobalScale, 0.5f, 2.5f, "%.2f");
        ImGui::ColorEdit4("background", (float*)&s.Colors[ImGuiCol_WindowBg]);
        ImGui::ColorEdit4("border",     (float*)&s.Colors[ImGuiCol_Border]);
        ImGui::ColorEdit4("title",      (float*)&s.Colors[ImGuiCol_TitleBgActive]);
        ImGui::ColorEdit4("accent",     (float*)&s.Colors[ImGuiCol_ButtonHovered]);
        ImGui::ColorEdit4("text",       (float*)&s.Colors[ImGuiCol_Text]);
    }

    ImGui::Spacing();
    ImGui::TextDisabled("%.0f fps", io.Framerate);
    ImGui::End();
}

static void* getplayerinstance() {
    if (!gcz.playercls) gcz.playercls = BNM::Class("GorillaLocomotion", "Player");
    if (!gcz.playercls) return nullptr;
    static Method<void*> get_instance = gcz.playercls.GetMethod("get_Instance");
    if (!get_instance.IsValid()) return nullptr;
    return get_instance();
}

static GameObject* findhand(const char* a, const char* b, const char* c) {
    if (auto* g = GameObject::Find(a)) return g;
    if (auto* g = GameObject::Find(b)) return g;
    if (auto* g = GameObject::Find(c)) return g;
    return nullptr;
}

static void tick(void*) {
    GameObject* leftctrl  = findhand("LeftHand Controller",  "LeftHandAnchor",  "Left Controller");
    GameObject* rightctrl = findhand("RightHand Controller", "RightHandAnchor", "Right Controller");
    if (!leftctrl || !rightctrl) {
        static int warncount = 0;
        if (warncount++ < 3) BNM_LOG_INFO("BebiteCheats: hands not found yet");
        return;
    }
    static bool announcedhands = false;
    if (!announcedhands) {
        BNM_LOG_INFO("BebiteCheats: hands found");
        announcedhands = true;
    }

    Transform* lefthand  = leftctrl->GetTransform();
    Transform* righthand = rightctrl->GetTransform();
    if (!lefthand || !righthand) return;

    for (auto& m : g_mods) {
        bool rising = m.enabled && !m.wasenabled;
        if (m.tick) {
            try {
                if (m.oneshot) { if (rising) m.tick(); }
                else           { if (m.enabled) m.tick(); }
            } catch (...) {}
        }
        m.wasenabled = m.enabled;
    }

    if (!gcz.initialized) {
        initimgui();
        registermods();
        buildpanel(lefthand);
        gcz.initialized = true;
    }
    if (!gcz.quad || !gcz.tex || !gcz.mat) return;

    Transform* panelt = gcz.quad->GetTransform();

    Vector3 leftpos = lefthand->GetPosition();
    Quaternion leftrot = lefthand->GetRotation();
    Vector3 rightpos = righthand->GetPosition();

    float righttrigger = XRInput::GetFloatFeature(Trigger, Right);
    bool  triggerheld  = righttrigger > ktriggerthresh;

    bool overpanel = false;
    {
        ImVec2 mousetmp; float depthtmp;
        overpanel = projectrighthandtoimgui(panelt, rightpos, mousetmp, depthtmp);
    }

    bool clickwhileover = overpanel && triggerheld;

    if (gcz.movemode) {
        if (gcz.dragging) {
            if (triggerheld) {
                Quaternion invleftrot = Quaternion::Inverse(leftrot);
                Vector3 rightinleft = invleftrot * (rightpos - leftpos);
                Vector3 delta = rightinleft - gcz.draggrablocal;
                gcz.localoffset = gcz.dragstartoffset + delta;
            } else {
                gcz.dragging = false;
                gcz.movemode = false;
            }
        } else if (clickwhileover && !gcz.prevclicked) {
            gcz.dragging = true;
            Quaternion invleftrot = Quaternion::Inverse(leftrot);
            gcz.draggrablocal = invleftrot * (rightpos - leftpos);
            gcz.dragstartoffset = gcz.localoffset;
        }
    }

    bool showheld = XRInput::GetBoolFeature(SecondaryButton, Left)
                 || XRInput::GetBoolFeature(GripButton,      Left);
    gcz.menuopen = showheld;
    if (!gcz.menuopen) { gcz.movemode = false; gcz.dragging = false; }
    gcz.prevshowheld = showheld;

    const float animspeed = 1.0f / 0.15f;
    float dt = std::fmax(1.0f / 240.0f, Time::GetUnscaledDeltaTime());
    if (gcz.menuopen) {
        gcz.animprogress = std::fmin(1.0f, gcz.animprogress + dt * animspeed);
    } else {
        gcz.animprogress = std::fmax(0.0f, gcz.animprogress - dt * animspeed);
    }

    bool visible = gcz.animprogress > 0.001f;
    if (gcz.quad->GetActiveSelf() != visible) {
        gcz.quad->SetActive(visible);
    }

    if (!visible) {
        gcz.prevclicked = false;
        return;
    }

    float p = gcz.animprogress;
    float eased = 1.0f - (1.0f - p) * (1.0f - p) * (1.0f - p);

    panelt->SetLocalPosition(gcz.localoffset);
    panelt->SetLocalRotation(gcz.localrotoffset);
    panelt->SetLocalScale(Vector3(kpanelmetersw * eased, kpanelmetersh * eased, 1.0f));

    if (!gcz.menuopen) {
        gcz.prevclicked = false;
        return;
    }

    ImGui::SetCurrentContext(gcz.ctx);
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = std::fmax(1.0f / 240.0f, Time::GetUnscaledDeltaTime());
    io.DisplaySize = ImVec2((float)kfbwidth, (float)kfbheight);

    ImVec2 mouse; float depth;
    bool over = projectrighthandtoimgui(panelt, rightpos, mouse, depth);
    if (over) io.AddMousePosEvent(mouse.x, mouse.y);
    else      io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);

    bool clicknow = over && triggerheld;
    if (clicknow != gcz.prevclicked) {
        if (!gcz.movemode) io.AddMouseButtonEvent(0, clicknow);
        else if (!clicknow) io.AddMouseButtonEvent(0, false);
    }
    gcz.prevclicked = clicknow;

    ImGui::NewFrame();
    buildui();
    ImGui::Render();
    ImDrawData* dd = ImGui::GetDrawData();
    cz::rasterizedrawdata(dd, gcz.framebuf, gcz.atlas);

    if (gcz.framebuf.px) {
        textureloadrawandapply(gcz.tex, gcz.framebuf.px, kfbwidth, kfbheight);
    }
}

static void (*orig_ccupdate)(void*) = nullptr;
static void hook_ccupdate(void* self) {
    if (orig_ccupdate) orig_ccupdate(self);
    try { tick(self); } catch (...) {}
}

static void onloaded() {
    gcz.playercls = BNM::Class("GorillaLocomotion", "Player");

    auto cc = BNM::Class("GorillaNetworking", "CosmeticsController");
    if (!cc) { BNM_LOG_INFO("BebiteCheats: cc class missing"); return; }
    auto update = cc.GetMethod("Update", 0);
    if (!update.IsValid()) { BNM_LOG_INFO("BebiteCheats: cc has no Update"); return; }
    BNM::InvokeHook(update, hook_ccupdate, orig_ccupdate);
    BNM_LOG_INFO("BebiteCheats: ready");
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env = nullptr;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    BNM::Loading::AddOnLoadedEvent(onloaded);
    BNM::Loading::TryLoadByJNI(env);
    return JNI_VERSION_1_6;
}
