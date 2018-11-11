#include "httpclient.h"

using namespace loader;

std::atomic_int v(0);

void loadFunc()
{
	IHttpClient* connection = new HttpClient;

	while (true)
	{
		const HttpResponse response = connection->sendGetRequest("http://www.cyberforum.ru");
		++v;
		std::cout << v.load() << std::endl;
	}
}

int main()
{
	curl_global_init(CURL_GLOBAL_ALL);

	std::vector<std::thread> threads;

	for (size_t i = 0; i < std::thread::hardware_concurrency() * 2; ++i)
	{
		threads.push_back(std::thread(loadFunc));
	}

	for (size_t i = 0; i < threads.size(); ++i)
	{
		threads[i].join();
	}

	std::cin.get();

	curl_global_cleanup();
}