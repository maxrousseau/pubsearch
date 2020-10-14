#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <cpr/cpr.h>
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

int main()
{

  // dummy example
  std::string test = buildQuery(search);
  cpr::Response r = cpr::Get(cpr::Url{test}); //for some reason this is
											  //segfaulting at runtime... maybe
											  //this is a cpr:: issue?

  auto json = nlohmann::json::parse(r.text);
  auto id_list = json["esearchresult"]["idlist"];
  std::cout << id_list << "\n"; //ok im getting ID list, I need to turn this
								//into a vector of strings...
  
  return 0;
}
