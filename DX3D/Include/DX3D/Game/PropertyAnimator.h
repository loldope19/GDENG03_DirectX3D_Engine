#pragma once
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Mathf.h>

namespace dx3d
{
	enum class LoopType
	{
		None,     // plays once and stops
		Repeat,   // restarts from the beginning (by which i mean snapping back)
		PingPong  // reverses direction at the end
	};
	class PropertyAnimator
	{
	public:
		PropertyAnimator() = default;

		void start(const Vec3& startValue, const Vec3& endValue, float duration, LoopType loopType = LoopType::None)
		{
			m_startValue = startValue;
			m_endValue = endValue;
			m_duration = duration > 0 ? duration : 0.001f;
			m_loopType = loopType;
			m_elapsedTime = 0.0f;
			m_isReversing = false;
			m_isRunning = true;
		}

		void stop()
		{
			m_isRunning = false;
		}

		void update(float dt)
		{
			if (!m_isRunning) return;

			m_elapsedTime += dt;

			if (m_elapsedTime >= m_duration)
			{
				switch (m_loopType)
				{
				case LoopType::None:
					m_elapsedTime = m_duration;
					m_isRunning = false;
					break;
				case LoopType::Repeat:
					m_elapsedTime -= m_duration;
					break;
				case LoopType::PingPong:
					m_elapsedTime = 0.0f;
					m_isReversing = !m_isReversing;
					break;
				}
			}
		}

		Vec3 getCurrentValue() const
		{
			if (!m_isRunning && m_loopType == LoopType::None) return m_endValue;

			float progress = m_elapsedTime / m_duration;

			if (m_loopType == LoopType::PingPong && m_isReversing)
			{
				return Mathf::lerp(m_endValue, m_startValue, progress);
			}

			return Mathf::lerp(m_startValue, m_endValue, progress);
		}

		bool isRunning() const { return m_isRunning; }

	private:
		Vec3 m_startValue;
		Vec3 m_endValue;
		float m_duration = 1.0f;
		float m_elapsedTime = 0.0f;

		LoopType m_loopType = LoopType::None;
		bool m_isReversing = false;
		bool m_isRunning = false;
	};
}