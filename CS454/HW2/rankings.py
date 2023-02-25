import csv
import math

class TF_IDF(object):
    """docstring for TF_IDF"""
    def __init__(self, dataFile):
        self.inv_index = {}         #key: term, value: key: id, value: number of occurances
        self.doc_term_count = {}    #key: id, value: number of terms in doc
        self.tf_dict = {}           #key: term, value: key: id, value: TF score
        self.idf_dict = {}          #key: word, value: 1/num docs containing word
        self.read_csv(dataFile)
        self.build_tf_and_idf_list()
    def tf_idf(self, Q:str, k:int): 
        tf_idf_help = {}
        tf_idf_result = []
        Q = Q.split()
        for word in Q: #gather the correct relevance scores for each term per document to then sum and sort
            if word not in self.inv_index:
                continue
            ids = self.inv_index[word]
            for id in ids:
                if id not in tf_idf_help:
                    tf_idf_help[id] = [self.tf_dict[word][id] * self.idf_dict[word]]
                else:
                    tf_idf_help[id].append(self.tf_dict[word][id] * self.idf_dict[word])
        for id in tf_idf_help:
            tf_idf_result.append((id, sum(tf_idf_help[id])))
        tf_idf_result.sort(key=lambda x: x[1], reverse=True)
        if len(tf_idf_result) < k: #returns list of tuples up to the number specified or the amount in the list if less than specified
            return(tf_idf_result[:len(tf_idf_result)]) 
        else:
            return(tf_idf_result[:k])

    def relevance(self, d: str, Q: str):  #returns relevance score as int
        rel_list = []
        Q = Q.split()
        for word in Q:
            if word not in self.tf_dict:
                continue
            if d not in self.tf_dict[word]:
                continue
            rel_list.append(self.tf_dict[word][d] * self.idf_dict[word])
        return(sum(rel_list))
    def tf(self, d:str, t:str):         #returns tf score as int
        if t not in self.tf_dict[t]:
            return(0)
        if d not in self.tf_dict[t]:
            return(0)
        else:
            return(self.tf_dict[t][d])
    def read_csv(self, path: str):  #reads every row, tracks number of terms in a document, and passes description to be parsed
        with open(path, newline='') as input_file:
            csv_reader = csv.reader(input_file, delimiter= ',')
            next(csv_reader) #skip header
            for row in csv_reader:
                description = row[1].split()
                self.doc_term_count[row[0]] = len(description)
                self.read_description(row[0], description)
    def read_description(self, id: str, description: list[str]):    #parses description for inverted index
        for word in description:
            if word not in self.inv_index:
                self.inv_index[word] = {id: 1} 
            else:
                if id in self.inv_index[word]:
                    num_occurances = self.inv_index[word][id]
                    self.inv_index[word][id] = num_occurances + 1
                else:
                    self.inv_index[word][id] = 1
    def build_tf_and_idf_list(self):        #builds dictionaries for quick lookups when running tf-idf
        for word in self.inv_index:
            if word not in self.idf_dict:
                num_docs_containing = len(self.inv_index[word]) 
                self.idf_dict[word] = 1.0 / num_docs_containing
            for id in self.inv_index[word]:
                if word not in self.tf_dict: #TF(d,t) = log(1 + (num times term occurs in doc/ num terms in a document))
                    self.tf_dict[word] = {id: math.log(1.0 + (self.inv_index[word][id]/self.doc_term_count[id]))}
                else:
                    self.tf_dict[word][id] = math.log(1.0 + (self.inv_index[word][id]/self.doc_term_count[id]))
            
class BM_25(object):
    """docstring for TF_IDF""" #left comment from the assignment page as-is
    def __init__(self, dataFile):
        self.inv_index = {}         #key: term, value: key: id, value: number of occurances
        self.doc_term_count = {}    #key: id, value: number of terms in doc
        self.tf_dict = {}         #key: term, value: key: id, value: term frequency of BM25 score
        self.idf_dict = {}          #key: word, value: 1/num docs containing word
        self.read_csv(dataFile)
        self.corpus_size = len(self.doc_term_count)
        self.avg_term_count = sum(self.doc_term_count.values())/self.corpus_size
        self.build_bm25()
    def read_csv(self, path: str):
        with open(path, newline='') as input_file:
            csv_reader = csv.reader(input_file, delimiter= ',')
            next(csv_reader) #skip header
            for row in csv_reader:
                description = row[1].split()
                self.doc_term_count[row[0]] = len(description)
                self.read_description(row[0], description)
    def read_description(self, id: str, description: list[str]): #takes word from description and inserts it into a dictionary along with the document id
        for word in description:
            if word not in self.inv_index:
                self.inv_index[word] = {id: 1} 
            else:
                if id in self.inv_index[word]:
                    num_occurances = self.inv_index[word][id]
                    self.inv_index[word][id] = num_occurances + 1
                else:
                    self.inv_index[word][id] = 1
    def build_bm25(self): # k1 = 1.2, k2 = 500, b = 0.75 Note: all of these were lumped together with constants in the actual calculations
       for word in self.inv_index:
            if word not in self.idf_dict:
                df_t = len(self.inv_index[word])
                self.idf_dict[word] = math.log((self.corpus_size - df_t + 0.5)/(df_t + 0.5))
            for id in self.inv_index[word]:
                if word not in self.tf_dict:
                    self.tf_dict[word] = {id: (2.2 * self.inv_index[word][id])/ \
                    ((1.2 * (0.25 + 0.75 *(self.doc_term_count[id]/self.avg_term_count))) + self.inv_index[word][id])}
                else:
                    self.tf_dict[word][id] = (2.2 * self.inv_index[word][id])/ \
                    ((1.2 * (0.25 + 0.75 *(self.doc_term_count[id]/self.avg_term_count))) + self.inv_index[word][id])  
    def bm25(self, query:str, k:int): #same as tf_idf, just with query frequency portion being calculated, rest is dictionary lookups
        query = query.split()
        query_dict = {}
        bm_help = {}
        bm_result = []
        for word in query:
            if word not in self.inv_index:
                continue
            if word not in query_dict:
                query_dict[word] = (501 * query.count(word)) / (500 + query.count(word))
            ids = self.inv_index[word]
            for id in ids:
                if id not in bm_help:
                    bm_help[id] = [self.idf_dict[word] * self.tf_dict[word][id] * query_dict[word]]
                else:
                    bm_help[id].append(self.idf_dict[word] * self.tf_dict[word][id] * query_dict[word])
        for id in bm_help:
            bm_result.append((id, sum(bm_help[id])))
        bm_result.sort(key=lambda x: x[1], reverse=True)
        if len(bm_result) < k:
            return bm_result[:len(bm_result)]
        else:
            return bm_result[:k]
 
