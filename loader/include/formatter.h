#pragma once

namespace loader
{
	class Formatter
	{
	public:
		template <typename T>
		Formatter& operator<<(const T& t)
		{
			m_sstream << t;
			return *this;
		}

		std::string toString() const
		{
			return m_sstream.str();
		}

	private:
		std::stringstream m_sstream;
	};
}