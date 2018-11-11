#pragma once

#include "formatter.h"
#include "httpheaders.h"
#include "ihttpclient.h"

namespace loader
{
	class HttpRequest
	{
	public:
		HttpRequest(CURL* httpConnection, const std::string& url)
			: m_httpConnection(httpConnection)
			, m_url(url)
		{
		}

		HttpRequest(HttpRequest&) = delete;
		HttpRequest& operator=(HttpRequest&) = delete;

		virtual ~HttpRequest() = default;

		HttpResponse execute()
		{
			curl_easy_reset(m_httpConnection);
			setOption(CURLOPT_URL, m_url.c_str());

			HttpResponse response;
			setOption(CURLOPT_WRITEFUNCTION, &writeCallback);
			setOption(CURLOPT_WRITEDATA, &response.body);

			char errorMessage[CURL_ERROR_SIZE];
			setOption(CURLOPT_ERRORBUFFER, &errorMessage);
			setOption(CURLOPT_SSL_VERIFYPEER, 0);

			setCustomRequestOptions();
			setOption(CURLOPT_HTTPHEADER, m_headers.get());

			const CURLcode result = curl_easy_perform(m_httpConnection);

			if (result != CURLE_OK)
			{
				const std::string error = (Formatter() << "Cannot perform HTTP request ("
					<< "result: " << result
					<< ", message: " << errorMessage << ")").toString();

				throw std::runtime_error(error);
			}

			response.httpCode = getHttpCode();
			return response;
		}

	protected:
		virtual void setCustomRequestOptions()
		{
		}

		template <typename T>
		void setOption(CURLoption option, const T& value) const
		{
			const auto result = curl_easy_setopt(m_httpConnection, option, value);

			if (result != CURLE_OK)
			{
				const std::string error = (Formatter() << "Cannot set HTTP session option ("
					<< "option: " << option
					<< ", message: \"" << curl_easy_strerror(result) << "\"").toString();

				throw std::runtime_error(error);
			}
		}

		void addHeader(const std::string& name, const std::string& value)
		{
			m_headers.add(name, value);
		}

	private:
		long getHttpCode() const
		{
			long httpCode = 0;
			const auto result = curl_easy_getinfo(m_httpConnection, CURLINFO_RESPONSE_CODE, &httpCode);

			if (result != CURLE_OK)
			{
				throw std::runtime_error((Formatter() << "Cannot get HTTP code (" << curl_easy_strerror(result) << ")").toString());
			}

			return httpCode;
		}

		static size_t writeCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
		{
			std::string* dataReceived = reinterpret_cast<std::string*>(userdata);
			const auto bufferSize = size * nmemb;
			dataReceived->append(reinterpret_cast<const char*>(buffer), bufferSize);
			return bufferSize;
		}

	private:
		CURL* const m_httpConnection;
		const std::string m_url;
		HttpHeaders m_headers;
	};

	using HttpGetRequest = HttpRequest;

	class HttpDeleteRequest : public HttpRequest
	{
	public:
		HttpDeleteRequest(CURL* http_connection, const std::string& url)
			: HttpRequest(http_connection, url)
		{
		}

	private:
		void SetCustomRequestOptions()
		{
			setOption(CURLOPT_CUSTOMREQUEST, "DELETE");
		}
	};

	class HttpPostRequest : public HttpRequest
	{
	public:
		HttpPostRequest(CURL* httpConnection,
			const std::string& url,
			const std::string& uploadData)
			: HttpRequest(httpConnection, url)
			, m_uploadData(uploadData)
			, m_unsentPtr(uploadData.c_str())
			, m_unsentLength(uploadData.size())
		{
		}

	protected:
		void SetCustomRequestOptions()
		{
			setOption(CURLOPT_POST, 1L);
			setOption(CURLOPT_POSTFIELDSIZE, m_uploadData.length());
			setOption(CURLOPT_READFUNCTION, readCallback);
			setOption(CURLOPT_READDATA, this);
		}

	private:
		static size_t readCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
		{
			HttpPostRequest* that = reinterpret_cast<HttpPostRequest*>(userdata);
			auto bufferSize = size * nmemb;
			auto copySize = that->m_unsentLength < bufferSize ? that->m_unsentLength : bufferSize;
			std::copy(that->m_unsentPtr, that->m_unsentPtr + copySize,
				reinterpret_cast<char*>(buffer));
			that->m_unsentLength -= copySize;
			that->m_unsentPtr += copySize;
			return copySize;
		}

	private:
		const std::string& m_uploadData;
		const char* m_unsentPtr;
		size_t m_unsentLength;
	};
}