#include <iostream>
#include <sstream>
#include <tinyxml2.h>
#include <tidy/tidy.h>
#include <tidy/buffio.h>
#include <curl/curl.h>

// g++ -g -ltinyxml2 -ltidy -o youtube_playlist_parser youtube_playlist_parser.cpp; read; ./youtube_playlist_parser

int writer(char *data, size_t size, size_t nmemb, std::string *writerData) {
	if (writerData == NULL)	return 0;
	writerData->append(data, size*nmemb);
	return size * nmemb;
}

std::string curl_getpage(CURL* curl, int index) {
	CURLcode res;
	std::string buffer;
	std::stringstream url;
	url << "http://www.youtube.com/my_favorites?sf=added&sa=0&pi=" << index;
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt" ); 
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt" ); 
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		//std::cout << "buffer.length():" << buffer.length() << std::endl;
		curl_easy_strerror(res);
	}
	return buffer;
}

void tinyxml2_parse(const TidyBuffer& output, int& counter) {
	tinyxml2::XMLDocument doc;

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
}

int main() {
	CURL *curl;
	int counter = 0;

	curl = curl_easy_init();
	TidyDoc tdoc = tidyCreate();
	TidyBuffer output = {0};
	tidyOptSetBool(tdoc, TidyXmlOut, yes);
	tidyOptSetBool(tdoc, TidyShowWarnings, no);
	tidyOptSetInt(tdoc, TidyWrapLen, 0);

	for(int i=0; i < 20; i++) {
		tidyBufClear(&output);
		tidyParseString(tdoc, curl_getpage(curl,i).c_str());
		tidySaveBuffer(tdoc, &output); 
		tinyxml2_parse(output, counter);
	}

	curl_easy_cleanup(curl);
	return 0;
}


