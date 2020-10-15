#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <curl/curl.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

/*
Replace the stupid std stream stuff with fmt because the other one is insane
nnalso use conanfile.txt and integrate with cmake to make dependency management
easier

Create citation struct write all citations to file
 @article{AuthorYear,
  nn     author = "",
       title = "",
       year = "",
       journal = "",
       volume = "",
       number = "",
       page = "",
 }

Plan for this little project
1. [x] build your query (some decent defaults for retmax and other categories)
2. [ ] vector of paper IDs
3. [ ] get the summary of the papers
4. [ ] get the references in bibtex fmt
5. [ ] fmt output (info + abstract), export to bibtex, ...
6. [ ] implement the cli arguments

*/

struct Query
{
  std::string esearch = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?";
  std::string efetch = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?";
  std::string db = "db=pubmed&";
  std::string term = "term=orthodontics&";
  std::string retmax = "retmax=10&";
  std::string json = "retmode=json";
  std::string text = "retmode=text&";
  //  std::vector id_list;
  std::string abstract = "rettype=abstract";
};

enum queryType { search, fetch, info };


// pass type argument and the query struct
// > check if type is search or summary or author info
//
std::string buildQuery(queryType qt)
{
  
  Query q;
  std::stringstream ss;

  switch(qt)
  {
  case search : ss << q.esearch << q.db << q.term << q.retmax << q.json; break;
  case fetch : ss << q.efetch << q.db << "q.id_list" << q.text << q.abstract; break;
	// TODO -> figure out how to make the id_list vector
  case info : ss << "nothing yet ... \n"; break;
  };


  std::string query = ss.str();
  return query;
}

/*
void getSummary(id_list)
{
}

void exportBibtex()
{
}p
*/

// Adapted curl request from this: https://gist.github.com/whoshuu/2dc858b8730079602044
// not sure I understand everything that's goin
size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

std::string curlGet(std::string query_url)
{

  auto curl = curl_easy_init();
  std::string response_string;
  const char *c_query_url = query_url.c_str(); //need to pass url as char*
											   //(string not supported by libcurl)
  
    if (curl) {
	    
        curl_easy_setopt(curl, CURLOPT_URL, c_query_url);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        

        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        
        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
       
		curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        curl = NULL;
	
	}
	
	return response_string;
}

int main()
{

  // dummy example
  std::string test_query = buildQuery(search);
  std::string test_response = curlGet(test_query);

  auto json = nlohmann::json::parse(test_response);
  auto id_list = json["esearchresult"]["idlist"];
  
  std::cout << id_list << "\n"; //ok im getting ID list, I need to turn this
								//into a vector of strings...
  
  return 0;
}
