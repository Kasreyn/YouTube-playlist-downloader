#include <iostream>
#include <sstream>
#include <tinyxml2.h>
#include <tidy/tidy.h>
#include <tidy/tidyenum.h>
#include <tidy/buffio.h>
#include <curl/curl.h>

// tidy -wrap 0 -b -asxml -i playlist.htm > playlist.xml
// g++ -g -ltinyxml2 -ltidy -o youtube_playlist_parser youtube_playlist_parser.cpp; read; ./youtube_playlist_parser playlist.xml

int writer(char *data, size_t size, size_t nmemb, std::string *writerData) {
	if (writerData == NULL)	return 0;
	writerData->append(data, size*nmemb);
	return size * nmemb;
}

std::string getpage(CURL* curl, int index) {
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
		//std::cout << "buffer size: " << buffer.length() << std::endl;
		//std::cout << "buffer:" << buffer <<  std::endl;
		curl_easy_strerror(res);
	}
	return buffer;
}

void parseTidyBuf(const TidyBuffer& output, int& counter) {
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

int main(int argc, char *argv[]) {
	CURL *curl;
	int counter = 0;
	if (argc < 2) return 1;

	curl = curl_easy_init();
	TidyDoc tdoc = tidyCreate();
	TidyBuffer output = {0};
	tidyOptSetBool(tdoc, TidyXmlOut, yes);
	tidyOptSetBool(tdoc, TidyShowWarnings, no);
	tidyOptSetInt(tdoc, TidyWrapLen, 0);

	for(int i=0; i < 20; i++) {
//		tidyBufFree(&output);
		tidyBufClear(&output);
	//	tidyParseFile(tdoc, argv[1]);
		tidyParseString(tdoc, getpage(curl,i).c_str());
		tidySaveBuffer(tdoc, &output); 
	//	tidySaveFile(tdoc, "tidy_test.xml");

	//	doc.LoadFile(argv[1]);
	//	doc.LoadFile("tidy_test.xml");
		parseTidyBuf(output, counter);
	}

	curl_easy_cleanup(curl);

	return 0;
}


//std::cout << counter << " " << n2->FirstChildElement("div")->FirstChildElement("div")->NextSibling()->FirstChildElement("div")->FirstChildElement("span")->FirstChildElement("a")->ToElement()->Attribute("class") << " : ";

