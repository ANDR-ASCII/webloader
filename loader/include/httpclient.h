#pragma once

#include "ihttpclient.h"

namespace loader
{

class HttpClient : public IHttpClient
{
public:
	HttpClient()
		: m_connection(initCurl())
	{
	}

	virtual HttpResponse sendGetRequest(const std::string& url) override;
	virtual HttpResponse sendPostRequest(const std::string& url, const std::string& data) override;
	virtual HttpResponse sendDeleteRequest(const std::string& url) override;

private:
	static CURL* initCurl()
	{
		CURL* const result = curl_easy_init();

		if (!result)
		{
			throw std::runtime_error("Cannot initialize CURL");
		}

		return result;
	}

private:
	CURL* m_connection;
};

}
