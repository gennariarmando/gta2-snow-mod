#pragma once
#include "common.h"
#include "CVector.h"
#include "CSprite2d.h"

static float ScaleY(float y) {
    return static_cast<float>(y * SCREEN_HEIGHT / 480.0f);
}

static void RotateVertices(CVector2D* rect, float x, float y, float angle) {
    float xold, yold;
    //angle /= 57.2957795;
    float _cos = cosf(angle);
    float _sin = sinf(angle);
    for (unsigned int i = 0; i < 4; i++) {
        xold = rect[i].x;
        yold = rect[i].y;
        rect[i].x = x + (xold - x) * _cos + (yold - y) * _sin;
        rect[i].y = y - (xold - x) * _sin + (yold - y) * _cos;
    }
}

static void DrawSpriteWithRotation(CSprite2d* sprite, float x, float y, float w, float h, float angle, CRGBA const& col) {
    CVector2D posn[4];
    posn[1].x = x - (w * 0.5f); posn[1].y = y - (h * 0.5f); posn[0].x = x + (w * 0.5f); posn[0].y = y - (h * 0.5f);
    posn[3].x = x - (w * 0.5f); posn[3].y = y + (h * 0.5f);	posn[2].x = x + (w * 0.5f); posn[2].y = y + (h * 0.5f);

    RotateVertices(posn, x, y, angle);

    if (sprite)
        sprite->Draw(
            posn[3].x, posn[3].y, posn[2].x, posn[2].y,
            posn[1].x, posn[1].y, posn[0].x, posn[0].y, CRGBA(col));
    else
        CSprite2d::DrawRect(CRect(x - (w * 0.5f), y - (h * 0.5f), x + (w * 0.5f), y + (h * 0.5f)), col);
}
