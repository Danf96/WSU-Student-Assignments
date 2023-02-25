import math
class Ranking(object):
    """Takes in a file path, reads every line in the path, inserts relevant information into tuple and returns relevant information for particular query lines."""
    def __init__(self, judgement_file):
        self.judgement_dict = {}
        self.read_judgements(judgement_file)

    
    def prec(self, query_line: str, thresh: int):
        """Takes in query line and threshold and returns precision based on dictionary and query line"""
        query_line = self.parse_query_line(query_line)
        k: int = len(query_line[1]) 
        rel: int = 0
        for entry in self.judgement_dict[query_line[0]]:
            if entry[0] in query_line[1] and entry[1] >= thresh:
                rel+=1
        if k < 1:
            return 0
        return(rel / k)

   
    def recall(self, query_line, thresh):
        """Takes in query line and threshold and checks relevant query line urls against dictionary"""
        query_line = self.parse_query_line(query_line)
        k: int = 0 
        rel: int = 0            
        for entry in self.judgement_dict[query_line[0]]:
            if entry[0] in query_line[1] and entry[1] >= thresh:
                rel+=1
            if entry[1] >= thresh:
                k+=1
        if k < 1:
            return 0
        return(rel / k)
   
    def rr(self, query_line, thresh):
        """Gets position of first relevant URL in query line"""
        query_line = self.parse_query_line(query_line)
        r_pos: int = 0
        judgements = self.judgement_dict[query_line[0]]
        for url in query_line[1]:
            r_pos+=1
            try:
                j = [x[0] for x in judgements].index(url) #list comprehension to grab index of tuple
            except:
                continue
            if judgements[j][1] >= thresh:
                return (1/r_pos)
        return 0
    
    def f1_score(self, query_line, thresh):
        """Calculates F1 score by calling precision and recall methods, then uses beta score formula to calculate F1"""
        recall = self.recall(query_line, thresh)
        prec = self.prec(query_line, thresh)
        if recall == 0 or prec == 0:
            return 0
        return ((2*prec*recall)/(prec + recall))

    
    def ndcg(self, query_line):
        """Calls DCG and IDCG then divides them for normalization"""
        query_line = self.parse_query_line(query_line)
        dcg = self.dcg(query_line)
        idcg = self.idcg(query_line)
        return (dcg / idcg)

    
    def read_judgements(self, judgement_file: str):
        """Parses judgement file and inserts it into a dictionary with query as a key and value as a list of tuples containing url and judgment score"""
        input = open(judgement_file, 'r')
        lines = input.readlines()
        input.close()
        for line in lines:
            line = line.rstrip('\n')
            line = line.split() #line[0] = query, line[1] = url, line[2] = judgement
            if line[0] not in self.judgement_dict:
                self.judgement_dict[line[0]] = [(line[1], int(line[2]))]
            else:
                self.judgement_dict[line[0]].append((line[1], int(line[2])))
    def parse_query_line(self, query_line: str) -> tuple[str, list[str]]:
        """Takes original query line strings, converts to list of strings, and returns a tuple containing the query and a list of URLs"""
        query_line = query_line.split()
        return (query_line[0], query_line[3:13])
    def dcg(self, query_line):
        """Calculates DCG based on formula in assignment3.pdf, try and except will be equivalent to treating urls not present in judgement dictionary as 0 in summation"""
        sum = 0.0
        i = 0
        scores = self.judgement_dict[query_line[0]]
        for url in query_line[1]:
            i +=1
            try:
                j = [x[0] for x in scores].index(url) #list comprehension to get index of tuple
            except:
                continue
            relevance = scores[j][1]
            sum += (relevance/math.log2(i + 1))
            
        return sum
    def idcg(self, query_line):
        """Calculates IDCG grabbing 10 best relevance scores from judgement dict"""
        sum = 0.0
        i = 0
        scores = self.judgement_dict[query_line[0]]
        scores.sort(key= lambda x: x[1], reverse=True)
        if len(scores) > 10:
            scores = scores[0:10]
        for tuple in scores:
            i+=1
            sum += (tuple[1]/math.log2(i + 1))
        return sum

