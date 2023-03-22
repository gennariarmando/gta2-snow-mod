#include "plugin.h"
#include "CGame.h"
#include "CCamera.h"
#include "CSprite2d.h"
#include "CPlayerPed.h"
#include "CPed.h"
#include "sprites.h"
#include "Timer.h"
#include "Utility.h"
#include "CTextureManager.h"
#include "CStyle.h"

using namespace plugin;

CSprite2d snowSprite;
Timer timer;
int amountOfFlakes = 14;

class SnowFlake {
public:
    CVector pos;
    CVector2D pos2D;
    float size;
    float next;
    float time;
    float angle;
    float lifeTime;

public:
    SnowFlake(CVector start) {
        float x = plugin::Random(-16.0f, 16.0f);
        float y = plugin::Random(-16.0f, 16.0f);

        pos = start;
        pos.x += x;
        pos.y += y;
        pos.z = 8.0f;

        size = plugin::Random(2.0f, 5.0f);
        next = 0.0f;
        time = 0;
        angle = plugin::Random(0.0f, M_PI);
        lifeTime = timer.GetTimeInMilliseconds() + 10000;
    }

    ~SnowFlake() {

    }

    void Process() {
        if (time < timer.GetTimeInMilliseconds()) {
            next = plugin::Random(-1.0f, 1.0f);
            time = timer.GetTimeInMilliseconds() + 200;
        }

        pos.x += (next * 0.02f) * plugin::GetTimeStepFix();
        pos.y -= (next * 0.02f) * plugin::GetTimeStepFix();
        
        pos.z -= plugin::GetTimeStepFix() * 0.2f;
        angle -= (next * 0.02f) * plugin::GetTimeStepFix();

        if (size <= 0.0f)
            size = 0.0f;
    }

    void Draw() {
        CEncodedVector2D out;
        CCamera* cam = GetGame()->FindPlayerPed(0)->GetAuxCamera();
        cam->WorldToScreen2D(pos.ToInt16(), &out);

        CVector2D out2 = out.FromInt16();

        float scale = (1.0f / ((cam->m_vPosInterp.FromInt16().z) + (8.0f - pos.z))) * 10.0f;
        scale = ScaleY(size * scale);

        DrawSpriteWithRotation(&snowSprite, out2.x, out2.y, scale, scale, angle, CRGBA(255, 255, 255, 150));
    }
};

std::vector<SnowFlake*> objects;

class SnowII {
public:
    SnowII() {
        plugin::Events::initEngineEvent += []() {
            snowSprite.SetTexture("snow", 32, 32, 32, (unsigned char*)snowFlakeSprite);
        };

        plugin::Events::gameProcessEvent += []() {
            CVector pos = GetGame()->FindPlayerPed(0)->GetAuxCamera()->m_vPosInterp.FromInt16();
            for (int i = 0; i < amountOfFlakes; i++) {
                SnowFlake* obj = new SnowFlake(pos);
                objects.push_back(obj);
            }

            for (auto& it : objects) {
                it->Process();
            }

            for (auto& it : objects) {
                if (it->pos.z <= 0.0f || it->lifeTime < timer.GetTimeInMilliseconds()) {
                    SnowFlake* obj = it;
                    objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
                    delete obj;
                }
            }
        }; 

        plugin::Events::renderGameEvent += []() {
            RenderStateSet(D3DRENDERSTATE_FOGENABLE, (void*)FALSE);
            RenderStateSet(D3DRENDERSTATE_SRCBLEND, (void*)D3DBLEND_SRCALPHA);
            RenderStateSet(D3DRENDERSTATE_DESTBLEND, (void*)D3DBLEND_INVSRCALPHA);
            RenderStateSet(D3DRENDERSTATE_TEXTUREMAG, (void*)D3DFILTER_LINEAR);
            RenderStateSet(D3DRENDERSTATE_TEXTUREMIN, (void*)D3DFILTER_LINEAR);
            RenderStateSet(D3DRENDERSTATE_SHADEMODE, (void*)D3DSHADE_FLAT);
            RenderStateSet(D3DRENDERSTATE_ZENABLE, (void*)TRUE);
            RenderStateSet(D3DRENDERSTATE_ZWRITEENABLE, (void*)FALSE);
            RenderStateSet(D3DRENDERSTATE_VERTEXBLEND, (void*)TRUE);
            RenderStateSet(D3DRENDERSTATE_ALPHABLENDENABLE, (void*)TRUE);
            RenderStateSet(D3DRENDERSTATE_ALPHATESTENABLE, (void*)FALSE);
            RenderStateSet(D3DRENDERSTATE_TEXTUREADDRESS, (void*)D3DTADDRESS_CLAMP);
            RenderStateSet(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (void*)FALSE);

            for (auto& it : objects) {
                it->Draw();
            }
        };

        plugin::Events::d3dResetEvent += []() {
            snowSprite.Reset();
        };

        plugin::Events::shutdownEngineEvent += []() {
            snowSprite.Delete();

            for (auto& it : objects) {
                delete it;
            }

            objects.clear();
        };

        auto LoadTiles = [](CTextureManager* _this, int) {
            _this->m_bTexturesInitialised = 1;

            for (int i = 0; i < 1024; i++) {
                unsigned char* p = plugin::CallMethodAndReturn<unsigned char*, 0x4C2EB0>(GetCurrentStyle(), i);

                int pal = GetCurrentStyle()->m_pPaletteIndex->physPalette[i];

                switch (pal) {
                case 0:
                    pal = 13;
                    break;
                default:
                    pal = pal + 7;
                    break;
                }


                _this->m_pTextures[i] = gbh_RegisterTexture(64, 64, p, pal, 0);
            }
        };

        plugin::patch::RedirectJump(0x4C3040, LAMBDA(void, __fastcall, LoadTiles, CTextureManager*, int));
    };

    ~SnowII() {

    };
} snowII;

