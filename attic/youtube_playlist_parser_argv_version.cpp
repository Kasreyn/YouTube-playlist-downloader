#include <iostream>
#include <tinyxml2.h>
#include <tidy/tidy.h>
#include <tidy/tidyenum.h>
#include <tidy/buffio.h>
#include <curl/curl.h>

// tidy -wrap 0 -b -asxml -i playlist.htm > playlist.xml
// g++ -ltinyxml2 -ltidy -o youtube_playlist_parser youtube_playlist_parser.cpp; read; ./youtube_playlist_parser playlist.xml

int writer(char *data, size_t size, size_t nmemb, std::string *writerData) {
	if (writerData == NULL)	return 0;
	writerData->append(data, size*nmemb);
	return size * nmemb;
}

int main(int argc, char *argv[]) {
	int counter = 0;
	std::string buffer;
	tinyxml2::XMLDocument doc;
	if (argc < 2) return 1;

	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://www.youtube.com/my_favorites?sf=added&sa=0&pi=5");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt" ); 
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt" ); 
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)  fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		std::cout << "buffer size: " << buffer.length() << std::endl;
		//std::cout << "buffer:" << buffer <<  std::endl;
		curl_easy_strerror(res);
		curl_easy_cleanup(curl);
	}

	TidyDoc tdoc = tidyCreate();
	TidyBuffer output = {0};
	tidyOptSetBool(tdoc, TidyXmlOut, yes);
	tidyOptSetBool(tdoc, TidyShowWarnings, no);
	tidyOptSetInt(tdoc, TidyWrapLen, 0);
//	tidyParseFile(tdoc, argv[1]);
	tidyParseString(tdoc, buffer.c_str());
	tidySaveBuffer(tdoc, &output); 
//	tidySaveFile(tdoc, "tidy_test.xml");

//	doc.LoadFile(argv[1]);
//	doc.LoadFile("tidy_test.xml");
	doc.Parse((const char*)output.bp);
	
	tinyxml2::XMLNode* n2 = doc.FirstChildElement("html")->FirstChildElement("body")->FirstChildElement("div")->FirstChildElement("div")->NextSibling()->NextSibling()->FirstChildElement("div")->FirstChildElement("div")->NextSibling()->FirstChildElement("div")->NextSibling()->FirstChildElement("div")->NextSibling()->NextSibling()->FirstChildElement("ol")->FirstChildElement("li");

	while (n2 != NULL) {
		std::cout << counter << " "; 
		std::string id(n2->ToElement()->Attribute("id"));
		std::cout << id.erase(0,9) ;
		
		const char* t = n2->FirstChildElement("div")->FirstChildElement("div")->NextSibling()->FirstChildElement("div")->FirstChildElement("span")->FirstChildElement("a")->ToElement()->GetText();
		(t == NULL) ? std::cout << " : [DELETED]" : std::cout << " : " << t ;

		std::cout << std::endl;
		counter++;
		n2 = n2->NextSibling();
	}

	return 0;
}


		//std::cout << counter << " " << n2->FirstChildElement("div")->FirstChildElement("div")->NextSibling()->FirstChildElement("div")->FirstChildElement("span")->FirstChildElement("a")->ToElement()->Attribute("class") << " : ";

