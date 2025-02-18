﻿#include "ChackCollision.h"
#include "GameData.h"

using namespace std;

namespace
{
    constexpr float kPushPowLight = 0.3f;
    constexpr float kPushPowNormal = 0.5f;
    constexpr float kPushPowHeavy = 0.7f;
}

namespace
{
    //最大当たり判定ポリゴン数
    constexpr int kMaxColHitPolyNum = 2000;
    //壁押し出し処理の最大試行回数
    constexpr int kMaxColHitTryNum = 16;
    //壁押し出し時にスライドさせる距離
    constexpr float kColHitSlideLength = 1.0f;
    //移動したかを判断するための変数
    constexpr float kMove = 0.01f;
    //壁ポリゴンか床ポリゴンかを判断するための変数
    constexpr float kWallPolyBorder = 0.4f;
    //壁ポリゴンと判断するための高さ変数
    constexpr float kWallPolyHeight = 5.0f;

    //頭の高さを設定
    constexpr float kHeadHeight = 40.0f;
}

bool ChackCollision::GetIsCollision(Collider& colA, Collider& colB)
{
    VECTOR vec = VZero();
    // 当たり判定をとらないColliderがあれば何もせず通過
    if (colA.isChackOther && colB.isChackOther)
    {
        if (colA.GetCollisionForm() == ColliderForm::SPHERE)
        {
            if (colB.GetCollisionForm() == ColliderForm::SPHERE)
            {
                vec = GetIsColSphereAndSphere(colA, colB);
            }
            else if (colB.GetCollisionForm() == ColliderForm::CAPSULE)
            {
                vec = GetIsColSphereAndCapsule(colA, colB);
            }
        }
        else if (colA.GetCollisionForm() == ColliderForm::CAPSULE)
        {
            if (colB.GetCollisionForm() == ColliderForm::SPHERE)
            {
                vec = VScale(GetIsColSphereAndCapsule(colB, colA), -1);
            }
            else if (colB.GetCollisionForm() == ColliderForm::CAPSULE)
            {
                vec = GetIsColCapsuleAndCapsule(colA, colB);
            }
        }
    }

    if (VSize(vec) == 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool ChackCollision::GetIsToGround(Collider& colA, Collider& colB, VECTOR& colAPreMoveVec)
{
    if (colA.prefer != 0 && colB.prefer == 0)
    {
        auto vec = MoveCollFieldUpdate(colA, colB, colAPreMoveVec, true);
        if (VSize(vec) == 0)    // 重力落下後も当たらない
        {
            return false;
        }
        else                    // 重力落下により当たる位置にいる
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

VECTOR ChackCollision::GetPushVec(Collider& mover, Collider& pusher, VECTOR& colAPreMoveVec)
{
    if (mover.prefer == -1 || pusher.prefer == -1)
    {
        return VZero();
    }
    else
    {
        // preferの大小が逆の時の処理を省くことで重複してはじくのを防ぐ
        if (!mover.isChackOther || !pusher.isChackOther)
        {
            return VZero();
        }
        VECTOR vec = VZero();
        if (mover.GetCollisionForm() == ColliderForm::SPHERE)
        {
            if (pusher.GetCollisionForm() == ColliderForm::SPHERE)
            {
                vec = GetIsColSphereAndSphere(mover, pusher);
            }
            else if (pusher.GetCollisionForm() == ColliderForm::CAPSULE)
            {
                vec = GetIsColSphereAndCapsule(mover, pusher);
            }
            else if (pusher.GetCollisionForm() == ColliderForm::MAP)
            {
                vec = MoveCollFieldUpdate(mover, pusher, colAPreMoveVec,false);
            }
        }
        else if (mover.GetCollisionForm() == ColliderForm::CAPSULE)
        {
            if (pusher.GetCollisionForm() == ColliderForm::SPHERE)
            {
                vec = VScale(GetIsColSphereAndCapsule(pusher, mover), -1);
            }
            else if (pusher.GetCollisionForm() == ColliderForm::CAPSULE)
            {
                vec = GetIsColCapsuleAndCapsule(mover, pusher);
            }
            else if (pusher.GetCollisionForm() == ColliderForm::MAP)
            {
                vec = MoveCollFieldUpdate(mover, pusher, colAPreMoveVec, false);
            }
        }


        if (mover.prefer == 1 && pusher.prefer != 0)      // 重たいものは押させない
        {
            {
                vec = VScale(vec, 0);
            }
        }

        return vec;
    }
}

bool ChackCollision::GetIsGround(Collider& colA, Collider& colB, VECTOR& colAPreMoveVec)
{
    if (!(colA.prefer > 0 && colB.prefer == 0))
    {
        return false;
    }
    else
    {
        auto vec = VNorm(MoveCollFieldUpdate(colA, colB, colAPreMoveVec, false));
        // （0,1,0）上ベクトルを内積に使うため、実質ｙ成分のみ
        float dot = vec.y;
        if (dot >= 1) // 45度以上の傾き
        {
            colA.isGround = true;
            return true;
        }
        else
        {
            colA.isGround = false;
            return false;
        }
    }
}


VECTOR ChackCollision::GetIsColSphereAndSphere(Collider& colA, Collider& colB)
{
    // 距離 / 半径の和
    float scale = VSize(VSub(colB.pos[0], colA.pos[0])) / (colB.radius + colA.radius);
    if (scale < 1.0f)
    {
        // 方向ベクトル
        VECTOR dirVec = VNorm(VSub(colB.pos[0], colA.pos[0]));
        // 接近した分のベクトルを返す
        return VScale(dirVec, scale);
    }
    else
    {
        return VZero();
    }
}

VECTOR ChackCollision::GetIsColSphereAndCapsule(Collider& colA, Collider& colB)
{
    // カプセルの方向ベクトル
    VECTOR B0ToB1 = VSub(colB.pos[1],colB.pos[0]);
    // 球からカプセルの球の方向ベクトル
    VECTOR B0ToA = VSub(colA.pos[0],colB.pos[0]);
    VECTOR B1ToA = VSub(colA.pos[0],colB.pos[1]);
    // 内積
    float dotB0 = B0ToB1.x * B0ToA.x + B0ToB1.y * B0ToA.y + B0ToB1.z * B0ToA.z;
    float dotB1 = (- B0ToB1.x) * B1ToA.x + (- B0ToB1.y) * B1ToA.y + (- B0ToB1.z) * B1ToA.z;

    // カプセル２球間に球があるとき
    if (dotB0 >= 0 && dotB1 >= 0)
    {
        float cos = dotB0 / (VSize(B0ToA) * VSize(B0ToB1));
        float temp = cos * VSize(B0ToB1);
        VECTOR BInToB0 = VScale(B0ToB1, -(temp / VSize(B0ToB1)));
        VECTOR BInToA = VAdd(BInToB0, B0ToA);
        float scale = VSize(BInToA) / (colA.radius + colB.radius);

        if (scale < 1.0f)
        {
            return VScale(BInToA, scale);
        }
        else
        {
            return VZero();
        }

    }

    // カプセル２球間以外に球があるとき
    else
    {
        if (dotB0 < 0)
        {
            // 距離 / 半径の和
            float scale = VSize(VSub(colB.pos[0], colA.pos[0])) / (colB.radius + colA.radius);
            if (scale < 1.0f)
            {
                // 方向ベクトル
                VECTOR dirVec = VNorm(VSub(colB.pos[0], colA.pos[0]));
                // 接近した分のベクトルを返す
                return VScale(dirVec, scale);
            }
            else
            {
                return VZero();
            }
        }
        else if (dotB1 < 0)
        {
            // 距離 / 半径の和
            float scale = VSize(VSub(colB.pos[1], colA.pos[0])) / (colB.radius + colA.radius);
            if (scale < 1.0f)
            {
                // 方向ベクトル
                VECTOR dirVec = VNorm(VSub(colB.pos[1], colA.pos[0]));
                // 接近した分のベクトルを返す
                return VScale(dirVec, scale);
            }
            else
            {
                return VZero();
            }
        }
    }
}

//VECTOR ChackCollision::GetIsColSphereAndWALL(Collider& colA, Collider& colB)
//{
//    return false;
//}
// 線分間の最近接点を計算するヘルパー関数
//VECTOR closestPointsOnSegments(Collider& colA, Collider& colB)
//{
//    auto d1 = VSub(colB.pos[0], colA.pos[0]); // 線分1の方向ベクトル
//	auto d2 = VSub(colB.pos[1], colA.pos[1]); // 線分2の方向ベクトル
//    auto r = VSub(colA.pos[1], colA.pos[0]);
//
//    double d1d1 = VSize(d1) * VSize(d1); // |d1|^2
//    double d2d2 = VSize(d2) * VSize(d2); // |d2|^2
//    double f = d2.x * r.x + d2.y * r.y + d2.z * r.z;
//
//    double s = 0.0, t = 0.0;
//
//	// 両方の線分が点の場合
//    if (d1d1 <= 1e-6 && d2d2 <= 1e-6) 
//    {
//        return { , p2 };
//    }
//
//    if (a <= 1e-6) {
//        // 線分1が点の場合
//        s = 0.0;
//        t = std::clamp(f / e, 0.0, 1.0);
//    }
//    else {
//        double c = d1.x * r.x + d1.y * r.y + d1.z * r.z;
//        if (e <= 1e-6) {
//            // 線分2が点の場合
//            t = 0.0;
//            s = std::clamp(-c / a, 0.0, 1.0);
//        }
//        else {
//            // 線分1と線分2が一般的な場合
//            double b = d1.x * d2.x + d1.y * d2.y + d1.z * d2.z;
//            double denom = a * e - b * b;
//
//            if (denom != 0.0) {
//                s = std::clamp((b * f - c * e) / denom, 0.0, 1.0);
//            }
//            else {
//                s = 0.0;
//            }
//
//            t = std::clamp((b * s + f) / e, 0.0, 1.0);
//        }
//    }
//
//    Vector3 closestPoint1 = p1 + d1 * s;
//    Vector3 closestPoint2 = p2 + d2 * t;
//
//    return { closestPoint1, closestPoint2 };
//}
// 

// カプセルとカプセルの当たり判定
VECTOR ChackCollision::GetIsColCapsuleAndCapsule(Collider& colA, Collider& colB)
{
    // カプセルの位置ベクトル
    VECTOR d1 = VSub(colA.pos[1], colA.pos[0]);
	VECTOR d2 = VSub(colB.pos[1], colB.pos[0]);
	VECTOR r = VSub(colA.pos[0], colB.pos[0]);

	// 線分間の最近接点を計算
	float a = VSize(d1) * VSize(d1);
	float b = VDot(d1, d2);
	float e = VSize(d2) * VSize(d2);
	float c = VDot(d1, r);
	float f = VDot(d2, r);

	// 分母
    float denominator = a * e - b * b;
    // 分母が0の場合、線分が平行
    float s, t;
    if (denominator <= 0.00001f) 
    {
        // 平行な場合の特別な処理
        s = 0.0f;
        t = (f / e);
        if (t < 0.0f) t = 0.0f;
        else if (t > 1.0f) t = 1.0f;
    }
    else 
    {
        s = (b * f - c * e) / denominator;
        t = (a * f - b * c) / denominator;

        // s,tを0~1の範囲に制限
        if (s < 0.0f) s = 0.0f;
        else if (s > 1.0f) s = 1.0f;

        if (t < 0.0f) t = 0.0f;
        else if (t > 1.0f) t = 1.0f;
    }

	// 最短距離ベクトルを計算
    VECTOR closestPointA = VAdd(colA.pos[0], VScale(d1, s));
	VECTOR closestPointB = VAdd(colB.pos[0], VScale(d2, t));

	// 最短距離ベクトル
    VECTOR diff = VSub(closestPointA, closestPointB);
    float distance = VSize(diff);

    // カプセルが重なっている場合、最短距離ベクトルを計算
    if (distance < colA.radius + colB.radius) {
        float penetrationDepth = colA.radius + colB.radius - distance;
        VECTOR penetrationVector = VScale(VNorm(diff), penetrationDepth);
        return penetrationVector;
    }



    // カプセルが衝突していない場合、ゼロベクトルを返す
    return { 0.0f, 0.0f, 0.0f };
}


// VECTOR ChackCollision::GetIsColCapsuleAndCapsule(Collider& colA, Collider& colB)
//{
//    VECTOR A0 = colA.pos[0];
//    VECTOR A1 = colA.pos[1];
//    VECTOR B0 = colB.pos[0];
//    VECTOR B1 = colB.pos[1];
//
//    VECTOR A0ToA1 = VSub(A1, A0);
//    VECTOR B0ToB1 = VSub(B1, B0);
//    VECTOR A0ToB0 = VSub(B0, A0);
//
//    float pOut_dist = 0;
//
//    VECTOR ANorm = VNorm(A0ToA1);
//    VECTOR BNorm = VNorm(B0ToB1);
//
//    float dot1 = A0ToB0.x * ANorm.x + A0ToB0.y * ANorm.y + A0ToB0.z * ANorm.z;
//    float dot2 = (-A0ToB0.x) * BNorm.x + (-A0ToB0.y) * BNorm.y + (-A0ToB0.z) * BNorm.z;
//
//    VECTOR clossAToB = VGet(A0ToA1.y * B0ToB1.z - A0ToA1.z * B0ToB1.y,
//                            A0ToA1.z * B0ToB1.x - A0ToA1.x * B0ToB1.z,
//                            A0ToA1.x * B0ToB1.y - A0ToA1.y * B0ToB1.x);
//
//    float Dv = VSize(clossAToB);
//
//    // カプセル同士が平行な場合
//    if (Dv < 0.000001f) {
//        if (pOut_dist) {
//            VECTOR ClossAB0ToA01 = VGet(A0ToB0.y * A0ToA1.z - A0ToB0.z * A0ToA1.y,
//                                        A0ToB0.z * A0ToA1.x - A0ToB0.x * A0ToA1.z,
//                                        A0ToB0.x * A0ToA1.y - A0ToB0.y * A0ToA1.x);
//            pOut_dist = VSize(ClossAB0ToA01);
//        }
//
//        VECTOR dirVec = VSub(VZero(), A0ToB0);
//        dirVec.y = 0;
//
//        float scale = 0;
//        if (VSize(dirVec) <= (colB.radius + colA.radius))
//        {
//            scale = (colB.radius + colA.radius) - VSize(dirVec);
//        }
//        return VScale(VNorm(dirVec), scale);
//    }
//
//    float dotACloss = ANorm.x * clossAToB.x + ANorm.y * clossAToB.y + ANorm.z * clossAToB.z;
//    if (dotACloss < 0.000001f) {
//        // 方向ベクトル
//        VECTOR dirVec = VSub(VZero(), A0ToB0);
//        dirVec.y = 0;
//
//        // 移動距離
//        float scale = 0;
//        // 範囲内であれば距離設定
//        if (VSize(dirVec) <= (colB.radius + colA.radius))
//        {
//            scale = (colB.radius + colA.radius) - VSize(dirVec);
//        }
//        // 接近した分のベクトルを返す
//        return VScale(VNorm(dirVec), scale);
//    }
//
//    float dotV = ANorm.x * BNorm.x + ANorm.y * BNorm.y + ANorm.z * BNorm.z;
//    float t1 = (dot1 - dot2 * Dv) / (1.0f - Dv * Dv);
//    float t2 = (dot2 - dot1 * Dv) / (Dv * Dv - 1.0f);
//
//    VECTOR Q1 = VAdd(A0, VScale(ANorm, t1));
//    VECTOR Q2 = VAdd(B0, VScale(BNorm, t2));
//
//    float distA = VSize(VSub(Q1, A0)) / (VSize(A0ToA1) + colA.radius);
//    float distB = VSize(VSub(Q2, B0)) / (VSize(B0ToB1) + colB.radius);
//
//    if (distA > 1.0f || distB > 1.0f) {
//        return VZero();
//    }
//    else {
//        float scale = VSize(VSub(Q2, Q1)) / (colA.radius + colB.radius);
//        if (scale < 1.0f) {
//            return VScale(VNorm(VSub(Q2, Q1)), scale);
//        }
//        else {
//            return VZero();
//        }
//    }
//}

VECTOR ChackCollision::GetIsColOtherAndMap(Collider& colOther, Collider& colMap, VECTOR& colAPreMoveVec, bool isAddGravity)
{
    // 重力作用前の座標
    auto otherPos = colOther.pos[0];
    // 重力の作用後の座標
    if (isAddGravity)
    {
        otherPos.y -= GameData::kGravity;
    }
    // オブジェクトとマップ自体の当たり判定チェックと
    // その結果の構造体化
    auto dim = MV1CollCheck_Sphere(colMap.mapHandle, -1, otherPos, colOther.radius);

    // 構造体データをもとに計算
    if (dim.HitNum > 0 && !colOther.isGround)
    {
        float highestVecY = 0;
        VECTOR nearestSideVec = VZero();   // 最接近点を入れるため、てきとうな数値で初期化
        for (int i = 0; i < dim.HitNum; i++)
        {
            auto hitNormal = dim.Dim[i].Normal;
            // 上ベクトル(0,1,0)との内積値
            // （上ベクトル成分がyのみだから比較するベクトルのyの値そのまま）
            auto absVecY = abs(hitNormal.y);
            // 上ベクトル
            if (absVecY >= 0.5f) // 上下方向から45度範囲内のベクトル
            {
                // 接しているポリゴンの中で一番高い位置にあるものを探す
                auto vecY = dim.Dim[i].HitPosition.y - otherPos.y;
                if (highestVecY < vecY)
                {
                    highestVecY = vecY;
                }
            }
            // 横ベクトル
            else if(absVecY < 0.1f)    // 真横方向から約15度範囲内のベクトル
            {
                auto vec = VSub( dim.Dim[i].HitPosition, otherPos);
                // 横方向に接しているポリゴンの中で,
                // radiusの範囲内かつ一番遠い位置にあるものを探す
                if ((VSize(nearestSideVec) < VSize(vec)) && (VSize(nearestSideVec) <= colOther.radius))
                {
                    nearestSideVec = vec;
                }
            }
        }

        float upScale = colOther.radius + highestVecY;

        // ｘｚ成分のみに調整
        nearestSideVec.y = 0;
        float sideScale = - (colOther.radius - VSize(nearestSideVec));
        // if (VSize(nearestSideVec) != 0)
        // {
        nearestSideVec = VScale(VNorm(nearestSideVec), sideScale);
        // }

        //// 当たり判定構造体の削除
        MV1CollResultPolyDimTerminate(dim);
        // colOther.isGround = true;
        return VGet(nearestSideVec.x, upScale, nearestSideVec.z);
    }
    else
    {
        // 当たり判定構造体の削除
        MV1CollResultPolyDimTerminate(dim);
        return VZero();
    }
}

//VECTOR ChackCollision::GetIsColCapsuleAndWALL(Collider& colA, Collider& colB)
//{
//    return false;
//}
//
//VECTOR ChackCollision::GetIsColWALLAndWALL(Collider& colA, Collider& colB)
//{
//    return false;
//}


VECTOR ChackCollision::MoveCollFieldUpdate(Collider& colOther, Collider& colMap, VECTOR& preMoveVec, bool isAddGravity)
{
    //// 移動前の座標を保存
    //m_oldPos = m_info.pos;

    //// 移動後の座標を算出
    //m_nextPos = VAdd(m_oldPos, GetInfo().vec);

    //// プレイヤーの周囲にあるステージポリゴンを取得する
    //// ( 検出する範囲は移動距離も考慮する )
    //m_hitDim = MV1CollCheck_Sphere(pField->GetModel()->GetModelHandle(), -1, GetInfo().pos, dynamic_cast<CharacterBase*>(this)->GetCircle()->GetRadius() + VSize(GetInfo().vec));
    
    VECTOR resVec = VZero();
    
    // 重力作用前の座標
    auto otherPos = colOther.pos[0];
    // 重力の作用後の座標
    if (isAddGravity)
    {
        otherPos.y -= GameData::kGravity;
    }
    auto nextPos = VAdd(otherPos, preMoveVec);

    // オブジェクトとマップ自体の当たり判定チェックと
    // その結果の構造体化
    m_hitDim = MV1CollCheck_Sphere(colMap.mapHandle, -1, otherPos, colOther.radius);

    // x軸かy軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
    if (fabs(preMoveVec.x) > kMove || fabs(preMoveVec.z) > kMove)
    {
        m_isMoveFlag = true;
    }
    else
    {
        m_isMoveFlag = false;
    }

    //壁と床の当たり判定を調べる
    CheckWallAndFloor(colOther);
    //壁との当たり判定処理
    FixPositionWithWall(colOther, nextPos);
    nextPos = FixPositionWithWall(colOther, nextPos);
    //床との当たり判定処理
    FixNowPositionWithFloor(colOther, nextPos);
    nextPos = FixNowPositionWithFloor(colOther, nextPos);

    // 新しい移動量を保存する
    // もともとの移動量は別で使うんで返してもらいます。
    resVec = VSub(VSub(nextPos, otherPos), preMoveVec);

    // 検出したプレイヤーの周囲のポリゴン情報を開放する
    MV1CollResultPolyDimTerminate(m_hitDim);

    return resVec;
}

void ChackCollision::CheckWallAndFloor(Collider& colOther)
{
    // 壁ポリゴンと床ポリゴンの数を初期化する
    m_wallNum = 0;
    m_floorNum = 0;

    // 検出されたポリゴンの数だけ繰り返し
    for (int i = 0; i < m_hitDim.HitNum; i++)
    {
        // ポリゴンの法線のＹ成分が壁ポリゴンボーダーに達っしているかどうかで壁ポリゴンか床ポリゴンかを判断する
        if (m_hitDim.Dim[i].Normal.y < kWallPolyBorder && m_hitDim.Dim[i].Normal.y > -kWallPolyBorder)
        {
            // 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標より高いポリゴンのみ当たり判定を行う
            if (m_hitDim.Dim[i].Position[0].y > colOther.pos[0].y + kWallPolyHeight ||
                m_hitDim.Dim[i].Position[1].y > colOther.pos[0].y + kWallPolyHeight ||
                m_hitDim.Dim[i].Position[2].y > colOther.pos[0].y + kWallPolyHeight)
            {
                // ポリゴンの数が限界数に達していなかったらポリゴンを配列に追加
                if (m_wallNum < ColInfo::kMaxColHitPolyNum)
                {
                    // ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
                    m_pWallPoly[m_wallNum] = &m_hitDim.Dim[i];

                    // 壁ポリゴンの数を加算する
                    m_wallNum++;
                }
            }
        }
        else
        {
            // ポリゴンの数が限界数に達していなかったらポリゴンを配列に追加
            if (m_floorNum < ColInfo::kMaxColHitPolyNum)
            {
                // ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
                m_pFloorPoly[m_floorNum] = &m_hitDim.Dim[i];

                // 床ポリゴンの数を加算する
                m_floorNum++;
            }
        }
    }
}

VECTOR ChackCollision::FixPositionWithWall(Collider& colOther, VECTOR& nextPos)
{
    auto vec = VSub(nextPos, colOther.pos[0]);
    auto resNextPos = nextPos;
    // 壁ポリゴンがない場合は何もしない
    if (m_wallNum == 0) return resNextPos;

    // 壁ポリゴンとの当たり判定処理
    // 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
    m_isHitFlag = false;

    // 移動したかどうかで処理を分岐
    if (m_isMoveFlag)
    {
        // 壁ポリゴンの数だけ繰り返し
        for (int i = 0; i < m_wallNum; i++)
        {
            // i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
            m_pPoly = m_pWallPoly[i];

            // ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
            if (!HitCheck_Capsule_Triangle(nextPos, VAdd(nextPos,
                VGet(0.0f, colOther.radius, 0.0f)),
                colOther.radius,
                m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

            // ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
            m_isHitFlag = true;

            //壁を考慮した移動を外積を使って算出
            VECTOR SlideVec;

            // 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
            SlideVec = VCross(vec, m_pPoly->Normal);

            // 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
            // 元の移動成分から壁方向の移動成分を抜いたベクトル
            SlideVec = VCross(m_pPoly->Normal, SlideVec);

            // それを移動前の座標に足したものを新たな座標とする
            resNextPos = VAdd(colOther.pos[0], SlideVec);


            // 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
            bool isHitWallPolygon = false;
            for (int j = 0; j < m_wallNum; j++)
            {
                // j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
                m_pPoly = m_pWallPoly[j];

                // 当たっていたらループから抜ける
                if (HitCheck_Capsule_Triangle(nextPos, VAdd(nextPos,
                    VGet(0.0f, colOther.radius, 0.0f)),
                    colOther.radius,
                    m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
                {
                    //trueにする
                    isHitWallPolygon = true;
                    break;
                }
            }

            // 全てのポリゴンと当たっていなかったらここでループ終了
            if (!isHitWallPolygon)
            {
                //ヒットフラグを倒す
                m_isHitFlag = false;
                break;
            }
        }
    }
    else
    {
        // 移動していない場合の処理

        // 壁ポリゴンの数だけ繰り返し
        for (int i = 0; i < m_wallNum; i++)
        {
            // i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
            m_pPoly = m_pWallPoly[i];

            // ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
            if (HitCheck_Capsule_Triangle(nextPos, VAdd(nextPos,
                VGet(0.0f, colOther.radius, 0.0f)),
                colOther.radius,
                m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
            {
                m_isHitFlag = true;
                break;
            }
        }
    }

    // 壁に当たっていたら壁から押し出す処理を行う
    if (m_isHitFlag)
    {
        resNextPos = FixPositionWithWallInternal(colOther, resNextPos);
    }

    return resNextPos;
}

VECTOR ChackCollision::FixPositionWithWallInternal(Collider& colOther, VECTOR& nextPos)
{
    auto resNextPos = nextPos;
    // 壁からの押し出し処理を試みる最大数だけ繰り返し
    for (int i = 0; i < ColInfo::kMaxColHitTryNum; i++)
    {
        // 当たる可能性のある壁ポリゴンを全て見る
        bool isHitWall = false;
        // 壁ポリゴンの数だけ繰り返し
        for (int j = 0; j < m_wallNum; j++)
        {
            // i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
            m_pPoly = m_pWallPoly[j];

            // プレイヤーと当たっているかを判定
            if (!HitCheck_Capsule_Triangle(nextPos, VAdd(nextPos,
                VGet(0.0f, colOther.radius, 0.0f)),
                colOther.radius, m_pPoly->Position[0],
                m_pPoly->Position[1], m_pPoly->Position[2])) continue;

            // 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
            // auto scale = colOther.radius - VSize(VSub(nextPos, m_pPoly->HitPosition));
            resNextPos = VAdd(nextPos, VScale(m_pPoly->Normal, kColHitSlideLength));

            // 移動した上で壁ポリゴンと接触しているかどうかを判定
            for (int k = 0; k < m_wallNum; k++)
            {
                // 当たっていたらループを抜ける
                m_pPoly = m_pWallPoly[k];
                if (HitCheck_Capsule_Triangle(nextPos, VAdd(nextPos,
                    VGet(0.0f, colOther.radius, 0.0f)),
                    colOther.radius,
                    m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
                {
                    isHitWall = true;
                    break;
                }
            }

            // 全てのポリゴンと当たっていなかったらここでループ終了
            if (!isHitWall) break;
        }

        //ループ終了
        if (!isHitWall) break;
    }
    return resNextPos;
}

VECTOR ChackCollision::FixNowPositionWithFloor(Collider& colOther, VECTOR& nextPos)
{
    auto resNextPos = nextPos;
    //床ポリゴンがない場合は何もしない
    if (m_floorNum == 0) return resNextPos;
    // 床ポリゴンとの当たり判定処理
    //あたったかどうかのフラグ初期化
    bool IsHitFlag = false;

    //ジャンプ中かつ上昇中の場合
    // if (dynamic_cast<CharacterBase*>(this)->GetJumpState() && dynamic_cast<CharacterBase*>(this)->GetJumpPower() >= 0.0f)
    // {
        // 天井に頭をぶつける処理を行う
        // 一番低い天井にぶつける為の判定用変数を初期化
        //float PolyMinPosY = 0.0f;

        //// 床ポリゴンの数だけ繰り返し
        //for (int i = 0; i < m_floorNum; i++)
        //{
        //    // i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
        //    m_pPoly = m_pFloorPoly[i];

        //    // 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
        //    m_lineRes = HitCheck_Line_Triangle(nextPos, VAdd(nextPos, VGet(0.0f, kHeadHeight, 0.0f)),
        //        m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

        //    // 接触していなかったら何もしない
        //    if (!m_lineRes.HitFlag) continue;

        //    // 天井ポリゴンが今まで検出されたポリゴンより低い場合処理を通す
        //    if (PolyMinPosY < m_lineRes.Position.y)
        //    {
        //        // ポリゴンに当たったフラグを立てる
        //        IsHitFlag = true;

        //        // 接触したＹ座標を保存する
        //        PolyMinPosY = m_lineRes.Position.y;
        //    }
        //}

        //// 接触したポリゴンがあれば
        //if (IsHitFlag)
        //{
        //    // 接触した場合はプレイヤーのＹ座標を接触座標を元に更新
        //    resNextPos.y = PolyMinPosY - kHeadHeight;

        //    //地面に当たった時の処理を行う
        //    dynamic_cast<CharacterBase*>(this)->HitGroundUpdate();
        //}
    // }

    // 床ポリゴンとの当たり判定
    // else
    // {
        //一番高い床ポリゴンにぶつける為の判定用変数を初期化
        float PolyMaxPosY = 0.0f;

        // 床ポリゴンに当たったかどうかのフラグを倒しておく
        m_isHitFlag = false;

        // 床ポリゴンの数だけ繰り返し
        for (int i = 0; i < m_floorNum; i++)
        {
            // i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
            m_pPoly = m_pFloorPoly[i];

            // 頭から足元まででポリゴンと当たっているかを判定
            m_lineRes = HitCheck_Line_Triangle(VAdd(nextPos, VGet(0.0f, kHeadHeight, 0.0f)),
                nextPos, m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

            // 当たっていなかったら何もしない
            if (!m_lineRes.HitFlag) continue;

            // 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
            if (m_isHitFlag && PolyMaxPosY> m_lineRes.Position.y) continue;

            // ポリゴンに当たったフラグを立てる
            m_isHitFlag = true;

            // 接触したＹ座標を保存する
            PolyMaxPosY = m_lineRes.Position.y;
        }

        // 床ポリゴンの当たり判定かつ、ジャンプ力が0よりも小さい(下降中の場合)どうかで処理を分岐
        if (m_isHitFlag)
        {
            // 接触したポリゴンで一番高いＹ座標からradius分上げた座標をプレイヤーのＹ座標にする
            resNextPos.y = PolyMaxPosY;
            //dynamic_cast<CharacterBase*>(this)->HitGroundUpdate();

            ////ジャンプ力が0よりも小さい(下降中)かつ、ジャンプ中であった場合
            ////ジャンプ処理を終了する
            //if (dynamic_cast<CharacterBase*>(this)->GetJumpPower() <= 0.0f &&
            //    dynamic_cast<CharacterBase*>(this)->GetJumpState())
            //{
            //    dynamic_cast<CharacterBase*>(this)->EndJumpState();
            //}
        }
        return resNextPos;
    // }
}