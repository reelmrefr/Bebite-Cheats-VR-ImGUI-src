#include "czrender.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace cz {

static inline int clampi(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline void blendpixel(uint8_t* dst,
                              uint8_t sr, uint8_t sg, uint8_t sb, uint8_t sa) {
    if (sa == 0) return;
    if (sa == 255) {
        dst[0] = sr; dst[1] = sg; dst[2] = sb; dst[3] = 255;
        return;
    }
    uint32_t ia = 255u - sa;
    dst[0] = (uint8_t)((sr * sa + dst[0] * ia + 127) / 255);
    dst[1] = (uint8_t)((sg * sa + dst[1] * ia + 127) / 255);
    dst[2] = (uint8_t)((sb * sa + dst[2] * ia + 127) / 255);
    uint32_t na = sa + (dst[3] * ia + 127) / 255;
    dst[3] = (uint8_t)(na > 255 ? 255 : na);
}

static void rastertriangle(framebuffer& fb, const fontatlas& atlas,
                           const ImDrawVert& v0,
                           const ImDrawVert& v1,
                           const ImDrawVert& v2,
                           int clipx0, int clipy0, int clipx1, int clipy1,
                           float scalex, float scaley,
                           float originx, float originy) {
    float x0 = (v0.pos.x - originx) * scalex;
    float y0 = (v0.pos.y - originy) * scaley;
    float x1 = (v1.pos.x - originx) * scalex;
    float y1 = (v1.pos.y - originy) * scaley;
    float x2 = (v2.pos.x - originx) * scalex;
    float y2 = (v2.pos.y - originy) * scaley;

    float area = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
    if (std::fabs(area) < 1e-6f) return;
    float invarea = 1.0f / area;
    bool flip = area < 0.0f;

    int minx = (int)std::floor(std::fmin(x0, std::fmin(x1, x2)));
    int miny = (int)std::floor(std::fmin(y0, std::fmin(y1, y2)));
    int maxx = (int)std::ceil (std::fmax(x0, std::fmax(x1, x2)));
    int maxy = (int)std::ceil (std::fmax(y0, std::fmax(y1, y2)));

    if (minx < clipx0) minx = clipx0;
    if (miny < clipy0) miny = clipy0;
    if (maxx > clipx1 - 1) maxx = clipx1 - 1;
    if (maxy > clipy1 - 1) maxy = clipy1 - 1;
    if (maxx < minx || maxy < miny) return;

    float dw0dx = (y1 - y2) * invarea;
    float dw0dy = (x2 - x1) * invarea;
    float dw1dx = (y2 - y0) * invarea;
    float dw1dy = (x0 - x2) * invarea;
    float dw2dx = (y0 - y1) * invarea;
    float dw2dy = (x1 - x0) * invarea;

    float sx = (float)minx + 0.5f;
    float sy = (float)miny + 0.5f;
    float w0_row = ((x2 - x1) * (sy - y1) - (y2 - y1) * (sx - x1)) * invarea;
    float w1_row = ((x0 - x2) * (sy - y2) - (y0 - y2) * (sx - x2)) * invarea;
    float w2_row = ((x1 - x0) * (sy - y0) - (y1 - y0) * (sx - x0)) * invarea;

    float r0 = (float)((v0.col >>  0) & 0xFF);
    float g0 = (float)((v0.col >>  8) & 0xFF);
    float b0 = (float)((v0.col >> 16) & 0xFF);
    float a0 = (float)((v0.col >> 24) & 0xFF);
    float r1c= (float)((v1.col >>  0) & 0xFF);
    float g1c= (float)((v1.col >>  8) & 0xFF);
    float b1c= (float)((v1.col >> 16) & 0xFF);
    float a1c= (float)((v1.col >> 24) & 0xFF);
    float r2c= (float)((v2.col >>  0) & 0xFF);
    float g2c= (float)((v2.col >>  8) & 0xFF);
    float b2c= (float)((v2.col >> 16) & 0xFF);
    float a2c= (float)((v2.col >> 24) & 0xFF);

    float dudx = dw0dx*v0.uv.x + dw1dx*v1.uv.x + dw2dx*v2.uv.x;
    float dvdx = dw0dx*v0.uv.y + dw1dx*v1.uv.y + dw2dx*v2.uv.y;
    float dudy = dw0dy*v0.uv.x + dw1dy*v1.uv.x + dw2dy*v2.uv.x;
    float dvdy = dw0dy*v0.uv.y + dw1dy*v1.uv.y + dw2dy*v2.uv.y;

    float drdx = dw0dx*r0 + dw1dx*r1c + dw2dx*r2c;
    float dgdx = dw0dx*g0 + dw1dx*g1c + dw2dx*g2c;
    float dbdx = dw0dx*b0 + dw1dx*b1c + dw2dx*b2c;
    float dadx = dw0dx*a0 + dw1dx*a1c + dw2dx*a2c;
    float drdy = dw0dy*r0 + dw1dy*r1c + dw2dy*r2c;
    float dgdy = dw0dy*g0 + dw1dy*g1c + dw2dy*g2c;
    float dbdy = dw0dy*b0 + dw1dy*b1c + dw2dy*b2c;
    float dady = dw0dy*a0 + dw1dy*a1c + dw2dy*a2c;

    float u_row = w0_row*v0.uv.x + w1_row*v1.uv.x + w2_row*v2.uv.x;
    float v_row = w0_row*v0.uv.y + w1_row*v1.uv.y + w2_row*v2.uv.y;
    float r_row = w0_row*r0 + w1_row*r1c + w2_row*r2c;
    float g_row = w0_row*g0 + w1_row*g1c + w2_row*g2c;
    float b_row = w0_row*b0 + w1_row*b1c + w2_row*b2c;
    float a_row = w0_row*a0 + w1_row*a1c + w2_row*a2c;

    int aw = atlas.w, ah = atlas.h;
    const uint8_t* apx = atlas.px;
    if (aw <= 0 || ah <= 0 || !apx) return;
    int aw_m1 = aw - 1;
    int ah_m1 = ah - 1;

    const float inv255 = 1.0f / 255.0f;

    for (int py = miny; py <= maxy; ++py) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;
        float u  = u_row;
        float v  = v_row;
        float r  = r_row;
        float g  = g_row;
        float b  = b_row;
        float a  = a_row;

        uint8_t* dstrow = fb.px + (size_t)py * fb.w * 4;

        for (int px = minx; px <= maxx; ++px) {
            bool inside = flip
                ? (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f)
                : (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f);
            if (inside) {
                int sxi = (int)(u * (float)aw);
                int syi = (int)(v * (float)ah);
                if (sxi < 0) sxi = 0; else if (sxi > aw_m1) sxi = aw_m1;
                if (syi < 0) syi = 0; else if (syi > ah_m1) syi = ah_m1;
                const uint8_t* p = apx + ((size_t)syi * aw + sxi) * 4;
                uint32_t ar = p[0], ag = p[1], ab = p[2], aa = p[3];

                uint32_t fr = (uint32_t)((r * ar) * inv255 + 0.5f);
                uint32_t fg = (uint32_t)((g * ag) * inv255 + 0.5f);
                uint32_t fb2= (uint32_t)((b * ab) * inv255 + 0.5f);
                uint32_t fa = (uint32_t)((a * aa) * inv255 + 0.5f);
                if (fr > 255) fr = 255;
                if (fg > 255) fg = 255;
                if (fb2> 255) fb2= 255;
                if (fa > 255) fa = 255;
                if (fa != 0)
                    blendpixel(dstrow + px * 4, (uint8_t)fr, (uint8_t)fg, (uint8_t)fb2, (uint8_t)fa);
            }
            w0 += dw0dx; w1 += dw1dx; w2 += dw2dx;
            u  += dudx;  v  += dvdx;
            r  += drdx;  g  += dgdx;  b  += dbdx;  a  += dadx;
        }
        w0_row += dw0dy; w1_row += dw1dy; w2_row += dw2dy;
        u_row  += dudy;  v_row  += dvdy;
        r_row  += drdy;  g_row  += dgdy;  b_row  += dbdy;  a_row  += dady;
    }
}

void rasterizedrawdata(ImDrawData* dd, framebuffer& fb, const fontatlas& atlas) {
    if (!fb.px || fb.w <= 0 || fb.h <= 0) return;
    std::memset(fb.px, 0, (size_t)fb.w * fb.h * 4);
    if (!dd || dd->CmdListsCount == 0) return;

    float dispw = dd->DisplaySize.x;
    float disph = dd->DisplaySize.y;
    if (dispw <= 0 || disph <= 0) return;

    float scalex = (float)fb.w / dispw;
    float scaley = (float)fb.h / disph;
    float originx = dd->DisplayPos.x;
    float originy = dd->DisplayPos.y;

    for (int n = 0; n < dd->CmdListsCount; n++) {
        const ImDrawList* cmdlist = dd->CmdLists[n];
        const ImDrawVert* vtx = cmdlist->VtxBuffer.Data;
        const ImDrawIdx* idx = cmdlist->IdxBuffer.Data;

        for (int ci = 0; ci < cmdlist->CmdBuffer.Size; ci++) {
            const ImDrawCmd* pcmd = &cmdlist->CmdBuffer[ci];
            if (pcmd->UserCallback) continue;

            ImVec4 clip = pcmd->ClipRect;
            int cx0 = (int)std::floor((clip.x - originx) * scalex);
            int cy0 = (int)std::floor((clip.y - originy) * scaley);
            int cx1 = (int)std::ceil ((clip.z - originx) * scalex);
            int cy1 = (int)std::ceil ((clip.w - originy) * scaley);
            cx0 = clampi(cx0, 0, fb.w);
            cy0 = clampi(cy0, 0, fb.h);
            cx1 = clampi(cx1, 0, fb.w);
            cy1 = clampi(cy1, 0, fb.h);
            if (cx1 <= cx0 || cy1 <= cy0) continue;

            for (unsigned int i = 0; i < pcmd->ElemCount; i += 3) {
                ImDrawIdx i0 = idx[pcmd->IdxOffset + i + 0];
                ImDrawIdx i1 = idx[pcmd->IdxOffset + i + 1];
                ImDrawIdx i2 = idx[pcmd->IdxOffset + i + 2];
                const ImDrawVert& a = vtx[pcmd->VtxOffset + i0];
                const ImDrawVert& b = vtx[pcmd->VtxOffset + i1];
                const ImDrawVert& c = vtx[pcmd->VtxOffset + i2];

                rastertriangle(fb, atlas, a, b, c,
                               cx0, cy0, cx1, cy1,
                               scalex, scaley, originx, originy);
            }
        }
    }
}

}
