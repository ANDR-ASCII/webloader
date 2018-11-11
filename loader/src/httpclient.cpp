#include "httpclient.h"
#include "httprequest.h"

namespace loader
{

HttpResponse HttpClient::sendGetRequest(const std::string& url)
{
	return HttpGetRequest(m_connection, url).execute();
}

HttpResponse HttpClient::sendPostRequest(const std::string& url, const std::string& data)
{
	return HttpPostRequest(m_connection, url, data).execute();
}

HttpResponse HttpClient::sendDeleteRequest(const std::string& url)
{
	return HttpDeleteRequest(m_connection, url).execute();
}

}