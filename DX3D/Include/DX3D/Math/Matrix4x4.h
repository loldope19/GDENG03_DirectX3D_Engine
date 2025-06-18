#pragma once
#include <DX3D/Math/Vec3.h>
#include <cmath>
#include <cstring> // for std::memset

namespace dx3d
{
	class Matrix4x4
	{
	public:
		Matrix4x4() { setIdentity(); }

		void setIdentity()
		{
			// Set all elements to 0
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					m_mat[i][j] = 0.0f;

			// Set diagonal to 1
			for (int i = 0; i < 4; ++i)
				m_mat[i][i] = 1.0f;
		}

		void setTranslation(const Vec3& translation)
		{
			setIdentity();
			m_mat[3][0] = translation.x;
			m_mat[3][1] = translation.y;
			m_mat[3][2] = translation.z;
		}

		void setScale(const Vec3& scale)
		{
			setIdentity();
			m_mat[0][0] = scale.x;
			m_mat[1][1] = scale.y;
			m_mat[2][2] = scale.z;
		}

		void setRotationX(float x)
		{
			setIdentity();
			m_mat[1][1] = cos(x); m_mat[1][2] = sin(x);
			m_mat[2][1] = -sin(x); m_mat[2][2] = cos(x);
		}

		void setRotationY(float y)
		{
			setIdentity();
			m_mat[0][0] = cos(y);  m_mat[0][2] = -sin(y);
			m_mat[2][0] = sin(y);  m_mat[2][2] = cos(y);
		}

		void setRotationZ(float z)
		{
			setIdentity();
			m_mat[0][0] = cos(z); m_mat[0][1] = sin(z);
			m_mat[1][0] = -sin(z); m_mat[1][1] = cos(z);
		}

		void setRotation(float pitch, float yaw, float roll)
		{
			Matrix4x4 x, y, z;
			x.setRotationX(pitch);
			y.setRotationY(yaw);
			z.setRotationZ(roll);
			*this = z * y * x; // Roll * Yaw * Pitch (Z * Y * X)
		}

		void transpose()
		{
			Matrix4x4 temp = *this;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					m_mat[i][j] = temp.m_mat[j][i];
		}

		Matrix4x4 transposed() const
		{
			Matrix4x4 temp = *this;
			temp.transpose();
			return temp;
		}

		void lookAt(const Vec3& eye, const Vec3& target, const Vec3& up)
		{
			Vec3 z_axis = (target - eye).normalized();
			Vec3 x_axis = Vec3::cross(up, z_axis).normalized();
			Vec3 y_axis = Vec3::cross(z_axis, x_axis);

			setIdentity();
			m_mat[0][0] = x_axis.x; m_mat[0][1] = y_axis.x; m_mat[0][2] = z_axis.x;
			m_mat[1][0] = x_axis.y; m_mat[1][1] = y_axis.y; m_mat[1][2] = z_axis.y;
			m_mat[2][0] = x_axis.z; m_mat[2][1] = y_axis.z; m_mat[2][2] = z_axis.z;

			m_mat[3][0] = -Vec3::dot(x_axis, eye);
			m_mat[3][1] = -Vec3::dot(y_axis, eye);
			m_mat[3][2] = -Vec3::dot(z_axis, eye);
		}

		void setPerspectiveFovLH(float fov, float aspect, float zNear, float zFar)
		{
			setIdentity();
			const float tanHalfFov = tan(fov / 2.0f);
			m_mat[0][0] = 1.0f / (aspect * tanHalfFov);
			m_mat[1][1] = 1.0f / tanHalfFov;
			m_mat[2][2] = zFar / (zFar - zNear);
			m_mat[2][3] = 1.0f;
			m_mat[3][2] = (-zNear * zFar) / (zFar - zNear);
			m_mat[3][3] = 0.0f;
		}

		Vec3 getZDirection() const { return { m_mat[2][0], m_mat[2][1], m_mat[2][2] }; }
		Vec3 getYDirection() const { return { m_mat[1][0], m_mat[1][1], m_mat[1][2] }; }

		Matrix4x4 operator*(const Matrix4x4& other) const
		{
			Matrix4x4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.m_mat[i][j] =
						m_mat[i][0] * other.m_mat[0][j] +
						m_mat[i][1] * other.m_mat[1][j] +
						m_mat[i][2] * other.m_mat[2][j] +
						m_mat[i][3] * other.m_mat[3][j];
				}
			}
			return result;
		}

		// Accessor if needed
		const float* data() const { return &m_mat[0][0]; }

	private:
		float m_mat[4][4];
	};
}
