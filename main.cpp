#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <cpr/cpr.h>
#include <fmt/core.h>

/*
Replace the stupid std stream stuff with fmt because the other one is insane
also use conanfile.txt and integrate with cmake to make dependency management
easier

Create citation struct write all citations to file
 @article{AuthorYear,
       author = "",
       title = "",
       year = "",
       journal = "",
       volume = "",
       number = "",
       page = "",
 }

plan for this little project
1. [x] build your query (some decent defaults for retmax and other categories)
2. [x] get some paper IDs
3. [x] get the summary of the papers
4. [ ] get the references in bibtex fmt
5. [ ] return to std_out, export (), ...

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

// pass type argument and the query struct
// > check if type is search or summary or author info
//
std::string buildQuery(std::string type)
{
  Query q;
  std::stringstream ss;
  //TODO convert this to a switch statement
  if (type == "search") {
	ss << q.esearch << q.db << q.term << q.retmax << q.json;
  }
  //TODO make the IDlisit!!!
  if (type == "summary") {
	//	ss << q.efetch << q.db << q.id_list << q.text << q.abstract;
  }
  //TODO add the info statement 
  else {
	std::cout << "should not run\n";
  }

  std::string query = ss.str();


  return query;
}

/*
void getSummary(id_list)
{
}

void exportBibtex()
{
}
*/

int main()
{
  // little test
  std::string test = buildQuery("search");
  cpr::Response r = cpr::Get(cpr::Url{test});
  std::cout << r.text;

  return 0;
}
