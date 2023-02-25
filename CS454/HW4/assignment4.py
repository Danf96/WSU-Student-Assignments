import os
from whoosh.fields import *
from whoosh import index
from whoosh.index import open_dir
from whoosh.writing import *
from whoosh.qparser import QueryParser
from whoosh.qparser import MultifieldParser
from whoosh import qparser

class WhooshSearcher(object):
    def __init__(self):
        self.tuples_returned: int = 10
        self.indexer = None

    def new_index(self, corpus_path):
        """Creates a new index based on the file path given containing documents"""
        schema = Schema(content=TEXT, url=ID(stored=True), title=TEXT(stored=True))
        if not os.path.exists("./indexdir"):
            os.mkdir("indexdir")
        self.indexer = index.create_in("./indexdir", schema)
        writer = self.indexer.writer()
        file_names = os.listdir(corpus_path)
        i = 0
        for name in file_names:
            full_path = os.path.join(corpus_path, name)
            if os.path.isfile(full_path): #make sure we aren't reading directories
                #open file and begin parsing
                i += 1
                file = open(full_path, 'r')
                link = file.readline()
                ftitle = file.readline()
                body:list[str] = file.readline() #should be the rest of the file on one line
                writer.add_document(content=body, url=link, title=ftitle)
                print(f'{i} {link} {ftitle}')
                file.close()
        writer.commit()

    def existing_index(self, index_path):
        """Loads existing index"""
        self.indexer = open_dir(index_path)

    def search(self, queryEntered, isDisjunctive):
        """Uses whoosh's query and search methods to return a tuple of 3 lists"""
        url = list()
        title = list()
        matched = list()
        with self.indexer.searcher() as search:
            if isDisjunctive:
                query = QueryParser('content', schema=self.indexer.schema, group=qparser.OrGroup)
            else:
                query = QueryParser('content', schema=self.indexer.schema, group=qparser.AndGroup)
            query = query.parse(queryEntered)
            results = search.search(query, limit=self.tuples_returned, terms=True)
            for x in results:
                url.append(x['url'])
                title.append(x['title'])
                matching = []
                for term in x.matched_terms():
                    matching.append(str(term[1]).lstrip('b'))
                matched.append(",".join(matching))
        return url, title, matched

def main():
    mySearcher = WhooshSearcher()
    while True:
        text_adventure(mySearcher)

def text_adventure(mySearcher: WhooshSearcher):
    """Runs on start of program to interact with WhooshSearcher class"""
    if(mySearcher.indexer is None):
        print("Index Currently Not Loaded!")
    print("(L)oad Index, (N)ew Index, (S)earch Index, (Q)uit")
    entry = input()
    match entry:
        case 's' | 'S': #search index
            search_text_handler(mySearcher) #search index method
        case 'l' | 'L': #load index
            load_index(mySearcher) 
        case 'n' | 'N': #new index from file
            new_index(mySearcher)
        case 'q' | 'Q':
            print("Closing Program.")
            os._exit(0)
        case _:
            print("Invalid Input")
            return
def search_text_handler(mySearcher: WhooshSearcher):
    """Text handling specifically for searches"""
    print(f'Tuples to be Returned: {mySearcher.tuples_returned}')
    print("Change (T)uples Returned, (C)onjunctive search, (D)isjunctive search, Go (B)ack")
    match input():
        case't' | 'T':
            modify_num_tuples(mySearcher)
        case'c' | 'C':
            search_handler(mySearcher, False)
        case 'd' | 'D':
            search_handler(mySearcher, True)
        case 'b' | 'B':
            return
        case _:
            print("Invalid Input")
            return
def search_handler(mySearcher: WhooshSearcher, isDisjunctive: bool):
    """Calls mySearcher's search methods and prints out the results"""
    query = input("Enter search terms here: ")
    urls, titles, matched = mySearcher.search(query, isDisjunctive)
    for a,b,c in zip(urls, titles, matched):
        a = a.rstrip('\n')
        b = b.split('-')[0].strip()
        c = c.rstrip('\n')
        print(f'< {a} {b} {c} >')

def load_index(mySearcher: WhooshSearcher):
    """Handles loading an existing index"""
    indexpath = input("Enter path of index: ")
    mySearcher.existing_index(indexpath)

def new_index(mySearcher: WhooshSearcher):
    """Handles creation of a new index"""
    dirpath = input("Enter directory of files to index: ")
    mySearcher.new_index(dirpath)

def modify_num_tuples(mySearcher: WhooshSearcher):
    """Handles changing number of tuples returned for a query"""
    num = input("Number of hits to return: ")
    mySearcher.tuples_returned = int(num)
if __name__ == '__main__':
    main()
