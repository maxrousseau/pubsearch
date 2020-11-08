/*
Pubsearch
Maxime Rousseau 2020

Plan for this little project
1. [x] build your query (some decent defaults for retmax and other categories)
2. [x] array of paper IDs
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

First hack TODO
1. I need to parse the abstracts and split them into strings according to their
id (PMID present at the very end of each abstract)
2. get the meta info for each article
3. call getAbstract and getInfo for the set query and create a vector of Bibtex
structs
*/

package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"strings"
)

var id string //id=1,2,3 <- close with

// buildQuery create a query url as a string
func buildQuery(query_type string) string {
	var esearch string = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?"
	var efetch string = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?"
	var esummary string = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?"
	var db string = "db=pubmed&"
	var term string = "term=orthodontics&"
	var retmax string = "retmax=10&"
	var json string = "retmode=json" //close with
	var text string = "retmode=text&"
	var abstract string = "rettype=abstract&"

	var url string

	switch query_type {
	case "search":
		s := []string{
			esearch,
			db,
			term,
			retmax,
			json}
		url = strings.Join(s, "")

	case "fetch":
		s := []string{
			efetch,
			db,
			abstract,
			text,
			id}
		url = strings.Join(s, "")

	case "summary":
		s := []string{
			esummary,
			db,
			id}
		url = strings.Join(s, "")
	default:
		url = "Error, type of query not specified"
	}

	return url
}

// make http request and return body as a string
func request(url string) string {
	resp, err := http.Get(url)
	body, err := ioutil.ReadAll(resp.Body)

	if err != nil {
		// do something with errors??
		panic(err)
	}

	defer resp.Body.Close()

	body_s := string(body)
	return body_s
}

// ok i understand how to make the json parser work but im gonna have to polish
// this a little more...
func getIdlist(json_stream string) string {
	type Head struct {
		Type    string `json:"type"`
		Version string `json:"version"`
	}
	type Esearch struct {
		Count            string   `json:"count"`
		Retmax           string   `json:"retmax"`
		Restart          string   `json:"restart"`
		Idlist           []string `json:"idlist"`
		Translationset   string   `json:"-"` //skip these fields - no use
		Translationstack string   `json:"-"`
		Querytranslation string   `json:"-"`
	}
	type Jsonstr struct {
		Header        Head    `json:"header"`
		Esearchresult Esearch `json:"esearchresult"`
	}

	var js Jsonstr
	json.Unmarshal([]byte(json_stream), &js)

	var id_list string = "id=" + strings.Join(js.Esearchresult.Idlist, ",")

	return id_list
}

// getAbstract ...
func getAbstract(raw_input string) {
	var abstract_arr []string = strings.Split(raw_input, "\n\n\n")
	// ok this works well
	fmt.Println(abstract_arr[1])

}

// getIdList from pubmed database and push to bibtex struct
//func getIdList() { }

func main() {
	var test string = buildQuery("search")
	// TODO: get the ID list string and instantiate as global variable
	fmt.Println(test)
	var test_resp string = request(test)

	fmt.Println(test_resp)
	id = getIdlist(test_resp)

	fmt.Println(id)
	var test_fetch string = buildQuery("fetch")
	var resp_fetch string = request(test_fetch)
	//	fmt.Println(resp_fetch)
	getAbstract(resp_fetch)
	//get abstract list split abstract by looking for \n\n (consecutive
	//newlines) OK done

	//get meta info about paper
	var test_summary string = buildQuery("summary")
	var resp_summary string = request(test_summary)
	fmt.Println(resp_summary)
	//TODO parse xml...

	//parse entries to bibtex structs
}
