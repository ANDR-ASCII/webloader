#pragma once

namespace loader
{
	struct HttpResponse
	{
		long httpCode;
		std::string body;

		HttpResponse()
			: httpCode(0)
		{
		}
	};

	class IHttpClient
	{
	public:
		virtual HttpResponse sendGetRequest(const std::string& url) = 0;
		virtual HttpResponse sendPostRequest(const std::string& url, const std::string& data) = 0;
		virtual HttpResponse sendDeleteRequest(const std::string& url) = 0;
		virtual ~IHttpClient() = default;
	};
}