/*
Pubsearch
Maxime Rousseau 2020

Plan for this little project
1. [x] build your query (some decent defaults for retmax and other categories)
2. [x] array of paper IDs
3. [x] get the summary of the papers -> format and place in bibtex struct
4. [ ] fmt output (info + abstract), export to bibtex, ...
5. [ ] implement the cli arguments

esearch -> make id list -> fetch -> get abstracts -> summary -> build bibtex
citatition -> format output (file and std output)

Create citation struct write all citations to stdout
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

package main

import (
	"encoding/json"
	"encoding/xml"
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
func getAbstract(raw_input string) []string {
	var abstract_arr []string = strings.Split(raw_input, "\n\n\n")

	return abstract_arr
}

type ESummaryResult struct {
	XMLName xml.Name `xml:"eSummaryResult"`
	Text    string   `xml:",chardata"`
	DocSum  []struct {
		Text string `xml:",chardata"`
		ID   string `xml:"Id"`
		Item []struct {
			Text string `xml:",chardata"`
			Name string `xml:"Name,attr"`
			Type string `xml:"Type,attr"`
			Item []struct {
				Text string `xml:",chardata"`
				Name string `xml:"Name,attr"`
				Type string `xml:"Type,attr"`
			} `xml:"Item"`
		} `xml:"Item"`
	} `xml:"DocSum"`
}

type Bibtex struct {
	title    string
	author   []string
	year     string
	journal  string
	volume   string
	number   string
	page     string
	abstract string
}

func parseXML(xml_stream string) ESummaryResult {

	var esr ESummaryResult
	xml.Unmarshal([]byte(xml_stream), &esr)
	// basically you begin by filtering throught the articles by ID. Then you
	// access the items of the articles, some of which are a lists (i.e. author
	// lists, which require you to access the nested struct)

	return esr
}

func xml_to_bib(xml_struct ESummaryResult, abstracts []string) []Bibtex {

	//parse entries to bibtex structs
	var retmax = 10 //TODO: make this a global var so that the loop doesnt break when I change the request retmax

	bib_arr := make([]Bibtex, retmax)

	for i := 0; i < retmax; i++ {

		num_author := len(xml_struct.DocSum[i].Item[3].Item)

		for a := 0; a < num_author; a++ {

			bib_arr[i].author = append(bib_arr[i].author, xml_struct.DocSum[i].Item[3].Item[a].Text)

		}

		bib_arr[i].title = xml_struct.DocSum[i].Item[5].Text
		bib_arr[i].journal = xml_struct.DocSum[i].Item[2].Text
		bib_arr[i].year = xml_struct.DocSum[i].Item[0].Text[:4]
		bib_arr[i].volume = xml_struct.DocSum[i].Item[6].Text
		bib_arr[i].number = xml_struct.DocSum[i].Item[7].Text
		bib_arr[i].page = xml_struct.DocSum[i].Item[8].Text

		bib_arr[i].abstract = abstracts[i]

	}

	return bib_arr
}

func ostream(lib []Bibtex) {
	//	var s_array []string
	retmax := 10

	for i := 0; i < retmax; i++ {
		s := fmt.Sprintf("@article{%s%s,\n", lib[i].author[len(lib[i].author)-1], lib[i].year)
		s += fmt.Sprintf("author = '%s',\n", lib[i].author)
		s += fmt.Sprintf("title = '%s',\n", lib[i].title)
		s += fmt.Sprintf("year = '%s',\n", lib[i].year)
		s += fmt.Sprintf("journal = '%s',\n", lib[i].journal)
		s += fmt.Sprintf("volume = '%s',\n", lib[i].volume)
		s += fmt.Sprintf("number = '%s',\n", lib[i].number)
		s += fmt.Sprintf("page = '%s',\n}", lib[i].page)
		fmt.Println(s) //TODO: OK this works, now concat strings
	}

	//ss string //final string stream to send to stdout
}

// getIdList from pubmed database and push to bibtex struct
// func getIdList() { }
func main() {
	var test string = buildQuery("search")

	// TODO: get the ID list string and instantiate as global variable
	var test_resp string = request(test)
	id = getIdlist(test_resp)

	var test_fetch string = buildQuery("fetch")
	var resp_fetch string = request(test_fetch)

	var abstracts []string = getAbstract(resp_fetch)
	//get abstract list split abstract by looking for \n\n (consecutive
	//newlines) OK done

	//get meta info about paper
	var test_summary string = buildQuery("summary")
	var resp_summary string = request(test_summary)

	//parse xml...
	var esr = parseXML(resp_summary)
	library := xml_to_bib(esr, abstracts) //array of library
	//	fmt.Println(library)

	//TODO: output bibtex stream
	ostream(library)

}
