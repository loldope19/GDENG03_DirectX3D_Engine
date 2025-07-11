#include <DX3D/Math/Matrix4x4.h> // We will create/update this header next

using namespace dx3d;
using namespace DirectX;

Matrix4x4::Matrix4x4()
{
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), XMMatrixIdentity());
}

Matrix4x4 Matrix4x4::createTranslation(const Vec3& translation)
{
    Matrix4x4 result;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixTranslation(translation.x, translation.y, translation.z));
    return result;
}

Matrix4x4 Matrix4x4::createScale(const Vec3& scale)
{
    Matrix4x4 result;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixScaling(scale.x, scale.y, scale.z));
    return result;
}

Matrix4x4 Matrix4x4::createRotationX(float angle)
{
    Matrix4x4 result;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixRotationX(angle));
    return result;
}

Matrix4x4 Matrix4x4::createRotationY(float angle)
{
    Matrix4x4 result;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixRotationY(angle));
    return result;
}

Matrix4x4 Matrix4x4::createRotationZ(float angle)
{
    Matrix4x4 result;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixRotationZ(angle));
    return result;
}

Matrix4x4 Matrix4x4::createLookAtLH(const Vec3& eye, const Vec3& target, const Vec3& up)
{
    Matrix4x4 result;
    XMVECTOR eyeVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&eye));
    XMVECTOR targetVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&target));
    XMVECTOR upVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&up));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixLookAtLH(eyeVec, targetVec, upVec));
    return result;
}

Matrix4x4 Matrix4x4::createPerspectiveFovLH(float fovY, float aspectRatio, float nearPlane, float farPlane)
{
    Matrix4x4 result;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearPlane, farPlane));
    return result;
}

// Matrix multiplication
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
    Matrix4x4 result;
    XMMATRIX m1 = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(this));
    XMMATRIX m2 = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&other));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), XMMatrixMultiply(m1, m2));
    return result;
}

Matrix4x4 Matrix4x4::transpose(const Matrix4x4& matrix)
{
    Matrix4x4 result;
    DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&matrix));
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), DirectX::XMMatrixTranspose(m));
    return result;
}