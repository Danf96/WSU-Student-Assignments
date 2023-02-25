
from assignment3 import *
def main():
    judgement_file = "./relevance_judgments-v1.txt"
    input = open('./sample.txt', 'r')
    lines = input.readlines()
    input.close()
    rank = Ranking(judgement_file)
    for j in range(1, 5):
        for i in range(10, 20):
            testing(rank, i, lines[i], j)
            print()
def testing(ranking, line_num, query_line, thresh):
    print(f'Line: {line_num+1} Thresh: {thresh} Prec: {ranking.prec(query_line,thresh):.6f}')
    print(f'Line: {line_num+1} Thresh: {thresh} Recall:  {ranking.recall(query_line, thresh):.6f}')
    print(f'Line: {line_num+1} Thresh: {thresh} F1: {ranking.f1_score(query_line, thresh):.6f}')
    print(f'Line: {line_num+1} Thresh: {thresh} RR: {ranking.rr(query_line, thresh):.6f}')
    print(f'Line: {line_num+1}      NDCG: {ranking.ndcg(query_line):.6f}')
if __name__ == "__main__":
    main()
