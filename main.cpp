#include <algorithm>
#include <iostream>
#include <map>
#include <math.h>
#include <set>
#include <string>
#include <vector>
#include <utility>
using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;
string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

struct Document {
    int id;
    double relevance;
};

struct Query {
    set<string> plus;
    set<string> minus;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
	    
       if(document.size()>0)
       {
            vector<string> words =SplitIntoWordsNoStop(document);
            double N=words.size();
            for (string word:words)
                word_to_document_freqs_[word][document_id]+=1/N;
            document_count_++;
       }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    int document_count_=0;
    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
		Query query_words;
		for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0]!='-')
			{query_words.plus.insert(word);}
            else
            {query_words.minus.insert(word.substr(1));}
        }
        return query_words;
    }
    vector<Document> FindAllDocuments(const Query& query_words) const {
		map<int, double> document_to_relevance;
        for(const string& searcWord :query_words.plus)
        {
            if (word_to_document_freqs_.count(searcWord)!=0 )
            {
                const map<int, double>& lineWr =  word_to_document_freqs_.at(searcWord);
                int coll=lineWr.size();
                if(coll>0)
                {
                    double idf=log(static_cast<double>(document_count_)/coll);
                    for (const auto& [id, tf]:lineWr)
                    {
                        document_to_relevance[id]+=idf*tf;
                    }
                }
            }
        }
		for (string ms:query_words.minus)
		{
			if (word_to_document_freqs_.count(ms)>0) 
            {
                for (const auto& [id, v]:word_to_document_freqs_.at(ms))
                {
                    document_to_relevance.erase(id);
                }
            }
		}
		vector<Document> matched_documents;
		for (const auto& [key, value] : document_to_relevance) {
			matched_documents.push_back({key, value});
        }
		return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}
