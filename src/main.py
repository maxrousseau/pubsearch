#!/usr/bin/env python3
'''
Replace the stupid std stream stuff with fmt because the other one is insane
nnalso use conanfile.txt and integrate with cmake to make dependency management
neasier

Plan for this little project
1. [x] build your query (some decent defaults for retmax and other categories)
2. [x] vector of paper IDs
3. [ ] get the summary of the papers -> format and place in bibtex struct
4. [ ] get the references in bibtex fmt
5. [ ] fmt output (info + abstract), export to bibtex, ...
6. [ ] implement the cli arguments (just use click)

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
'''
import requests
import json


class Query:
    pass


q = Query()
q.esearch = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?"
q.efetch = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?"
q.esummary = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?"
q.db = "db=pubmed&"
q.term = "term=orthodontics&"
q.retmax = "retmax=10&"
q.json = "retmode=json"
q.text = "retmode=text&"
q.id = []

# def build_url(query_type):
#     # build url depending on the type of url sent
#     return url
# 
# def get_id(response):
#     # 
# 
# def get_abstract():
#     #
# 
#     return abstract
# 
# def gen_bibtex():
#     #
# 
def main():

if __name__== "__main__":
    main()
