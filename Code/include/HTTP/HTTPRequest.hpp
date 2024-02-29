#pragma once

#include "HTTPDocument.hpp"

namespace AxxonsoftInternProject
{
	namespace http
	{
		const string requestMethods[]
		{
			"GET",
			"POST",
			"DELETE"
		};

		struct HTTPRequest : public HTTPDocument
		{
			string method;
			string uri;
		};
	}
}