#include "rfpch.hpp"
#include "RfextendedMath.hpp"
#include <DirectXMath.h>

namespace Rimfrost
{
    namespace rfm
    {
        Vector3 planeIntersectLine(Vector4 plane, Vector3 linePoint1, Vector3 linePoint2)
        {
            DirectX::XMVECTOR XMplane{ plane.x, plane.y, plane.z, plane.w };
            DirectX::XMVECTOR XMlinePoint1{ linePoint1.x, linePoint1.y, linePoint1.z };
            DirectX::XMVECTOR XMlinePoint2{ linePoint2.x, linePoint2.y, linePoint2.z };
            DirectX::XMFLOAT3 XMresult;
            DirectX::XMStoreFloat3(&XMresult, DirectX::XMPlaneIntersectLine(XMplane, XMlinePoint1, XMlinePoint2));
            return Vector3(XMresult.x, XMresult.y, XMresult.z);
        }
        Vector4 planeFromPointNormal(Vector3 point, Vector3 normal)
        {
            DirectX::XMVECTOR XMpoint{ point.x, point.y, point.z };
            DirectX::XMVECTOR XMnormal{ normal.x, normal.y, normal.z };
            DirectX::XMFLOAT4 XMplane;
            DirectX::XMStoreFloat4(&XMplane, DirectX::XMPlaneFromPointNormal(XMpoint, XMnormal));
            return Vector4(XMplane.x, XMplane.y, XMplane.z, XMplane.w);
        }
    }
}