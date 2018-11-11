#pragma once

namespace loader
{
	class HttpHeaders final
	{
	public:
		HttpHeaders()
			: m_head(nullptr)
		{
		}

		~HttpHeaders()
		{
			curl_slist_free_all(m_head);
		}

		void add(const std::string& name, const std::string& value)
		{
			m_head = curl_slist_append(m_head, (name + ": " + value).c_str());

			if (!m_head)
			{
				throw std::runtime_error("Cannot add header");
			}
		}

		curl_slist* get() const
		{
			return m_head;
		}

	private:
		curl_slist* m_head;
	};
}