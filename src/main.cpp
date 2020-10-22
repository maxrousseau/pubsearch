#include <iostream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>

#include <typeinfo> // keep only for dev/debugging

#include <curl/curl.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

/*
Replace the stupid std stream stuff with fmt because the other one is insane
nnalso use conanfile.txt and integrate with cmake to make dependency management
neasier

Plan for this little project
1. [x] build your query (some decent defaults for retmax and other categories)
2. [x] vector of paper IDs
3. [ ] get the summary of the papers -> format and place in bibtex struct
4. [ ] get the references in bibtex fmt
5. [ ] fmt output (info + abstract), export to bibtex, ...
6. [ ] implement the cli arguments

esearch -> make id list -> fetch -> get abstracts -> summary -> build bibtex
citatition -> format output (file and std output)

Create citation struct write all citations to file
 @article{AuthorYear,
       author = "",
       title = "",
       year = "",
       journal = "",
       volume = "",
       number = "",
       page = "",
	   abstract = "",
 }
*/

struct Query
{
  std::vector<std::string> id_vector;
  int retmax_int = 10; // TODO -> this needs to be parsed into the retmax string

  std::string esearch = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?";
  std::string efetch = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?";
  std::string esummary = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?";

  std::string db = "db=pubmed&";
  std::string term = "term=orthodontics&";
  std::string retmax = "retmax=10&";
  std::string json = "retmode=json"; //close with
  std::string text = "retmode=text&";
  std::string id; //id=1,2,3 <- close with

  std::string abstract = "rettype=abstract&";
};

enum queryType { search, fetch, info };
Query q; //initialize a single query struct visible for all functions

struct Bibtex
{
  std::string author;
  std::string title;
  std::string year;
  std::string journal;
  std::string volume;
  std::string number;
  std::string page;
  std::string abstract;
};

std::string buildQuery(queryType qt)
{
  
  std::stringstream ss;

  switch(qt)
  {
  case search : ss << q.esearch << q.db << q.term << q.retmax << q.json; break;
  case fetch : ss << q.efetch << q.db << q.abstract << q.text << q.id; break;
  case info : ss << q.esummary << q.db << q.id; break;
  };


  std::string query = ss.str();
  return query;
}



void getId(std::string response)
{
  // build the id string for the summary and fetch requests
  auto json = nlohmann::json::parse(response);
  auto id_list = json["esearchresult"]["idlist"];

  // TODO -> replace with fmt::join
  for (int i = 0; i < q.retmax_int; i++)
	{
	  auto id = id_list[i].get<std::string>();
	  q.id_vector.push_back(id);
	}  
  
  std::ostringstream oss;

  oss << "id=";
  
  if (!q.id_vector.empty()){
	std::copy(q.id_vector.begin(), q.id_vector.end()-1,
			  std::ostream_iterator<std::string>(oss, ","));

	oss << q.id_vector.back();
  }

  q.id = oss.str();

}

/*

void exportBibtex()
{
}
*/


// Adapted curl request from this: https://gist.github.com/whoshuu/2dc858b8730079602044
// not sure I understand everything that's goin
size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
  data->append((char*) ptr, size * nmemb); //FIX: warning: use of old-style cast [-Wold-style-cast] 
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


std::vector<std::string> split_lines(const std::string& text)
{
  std::string tmp_line;
  std::vector<std::string> tmp_vec;
  std::vector<std::string> abstract_vec;
  std::stringstream ss(text);
  
  while(getline(ss,tmp_line,'\n')) {
	tmp_vec.push_back(tmp_line);
	std::cout << tmp << "[LINE] \n"; //testing
	// 1. check if tmp_line contains PMID
	// 2. if it does, merge tmp_vec and push back to abstract_vec
  }
  return abstract_vec;
}


void getAbstract()
{

  std::string abstract_query = buildQuery(fetch);
  std::cout << abstract_query << "\n";

  std::string abstract_response = curlGet(abstract_query);
  std::cout << abstract_response << "\n";

  split(abstract_response);
  
}

/*
First hack TODO
1. I need to parse the abstracts and split them into strings according to their
id (PMID present at the very end of each abstract)
2. get the meta info for each article
3. call getAbstract and getInfo for the set query and create a vector of Bibtex
structs


void getInfo()
{
// 
}

void generateBibtex()
{
  //use getAbstract and getInfo to build a vector of Bibtex structs
}
*/


int main()
{

  
  // dummy example
  std::string test_query = buildQuery(search);
  std::string test_response = curlGet(test_query);
  getId(test_response); //ok this works
  getAbstract();

  return 0;
}
