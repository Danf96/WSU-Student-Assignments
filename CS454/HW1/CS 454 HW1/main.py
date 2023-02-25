import requests
from bs4 import BeautifulSoup
import queue
import json
from urllib import parse
from urllib import robotparser #json library can dump dictionary into xml, pickle can save as object
import time
import socket
import os
import signal


def main(): 
    print('Enter number of pages to be crawled. (Ex. \'50\')')      #initial input to be passed to our crawler
    num_pages = int(input())
    if num_pages <=0:
        print(f'{num_pages} is not a valid number!')
        os._exit(1)
    print('Enter links to be used as a starting seed separated by spaces. (Ex. \'https://wsu.edu https://www.wikipedia.org https://www.imdb.com\')')
    seed = input()
    seed = seed.split(' ')

    socket.setdefaulttimeout(2)                                     #necessary to avoid hanging indefinitely when parsing poorly formatted robots.txt files
    path = os.path.dirname(__file__)
    os.makedirs(path + '/files', exist_ok=True)

    q = queue.Queue()
    for link in seed:
        q.put(link)
    breadth_first_crawl(q, path, num_pages)

def breadth_first_crawl(q: queue.Queue, path: str, num_pages: int):     #this method follows the basic algorithm outlined in the slides with the exception of not building an index
    url: str
    count: int
    file_path = path + '/files'
    adj_sites = {}
    robot_dict: dict[str, (list[str], robotparser.RobotFileParser)] = {}    #this dictionary will store the hostname as a key and the value will be a tuple containing a list of urls belonging to a hostname as well as a robot object
    count = 0
    signal.signal(signal.SIGINT, ExitHandler((path, adj_sites)))            #allows user to press CTRL+C in order to exit program while running while still saving the adjacency matrix
    
    while (not q.empty() and count < num_pages):
        url = q.get()
        print(f'Popping {url} from Queue')
        split_url = parse.urlsplit(url)

        if split_url is not None:
            if split_url.hostname in robot_dict:
                robot = robot_dict[split_url.hostname][1] #assign pre-created robot object to current url
            else:
                robot = get_robot(split_url.scheme, split_url.hostname)
                robot_dict[split_url.hostname] = ([], robot)

            if robot is not None and url not in robot_dict[split_url.hostname][0]: #ensures we have a proper robots.txt and url has not been visited yet
                if robot.can_fetch('*', url):
                    print(f'Currently in: {url}')
                    delay_time = robot.crawl_delay('*')
                    if delay_time is not None and delay_time <= 15:             #limiting to 15 because certain websites have 30 seconds of delay yet don't ban for breaking the rule
                        time.sleep(delay_time)
                    raw_page = get_page(url)
                    if raw_page is not None:
                        content_type = raw_page.headers.get('content-type')
                        if content_type is not None:
                            print(f'URL is of type: {content_type}')
                            if 'text/html' in content_type:                     #continue loop if page grabbed is not of correct type
                                print(f'Adding {url} to visited list.')
                                robot_dict[split_url.hostname][0].append(url)
                                count += 1
                                print(f'Number of pages crawled: {count}')
                                url_filename = url.replace("/", "").replace(":", "")
                                if len(url_filename) >= 255:                        #hash filename to fit linux max filename size
                                    url_filename = str(hash(url_filename))
                                bs = BeautifulSoup(raw_page.text, 'lxml')
                                content = bs.get_text(" ", strip=True)              #grab all text from a file
                                with open(f'{file_path}/{url_filename}.txt', 'w') as f: 
                                    f.write(url + '\n')
                                    f.write(content) 
                                for link in bs.find_all('a'): #gets all anchor text
                                    cleaned_link = link.get('href')
                                    if cleaned_link is not None and cleaned_link != "":
                                        if cleaned_link[0] == '/':
                                            cleaned_link = parse.urljoin(url, cleaned_link)
                                        if 'http' in cleaned_link:
                                            if cleaned_link not in adj_sites:       #create new linked list to store adjacencies
                                                adj_sites[cleaned_link] = [url]
                                            else:                                   #append to existing list and replace
                                                if url not in adj_sites[cleaned_link]:
                                                    adj_sites[cleaned_link].append(url) 
                                            q.put(cleaned_link)
    print(f'Dumping adjacency matrix to \'adjacent_links\' in current directory.')
    with open(f'{path}/adjacent_links', 'w') as f:
        json.dump(adj_sites, f)
            
def get_robot(scheme: str, hostname: str): #tries to create a robot object, if an exception occurs returns none
        robot = robotparser.RobotFileParser()
        try:
            robot.set_url(parse.urljoin(scheme + '://' + hostname, 'robots.txt'))
            robot.read()
            return robot
        except:
            print(f'Failed to grab robots.txt for {hostname}.')
            return None
def get_page(url: str):
        try:
            return requests.get(url,timeout=3)
        except:
            print(f'Failed to get page for {url}.')
            return None

class ExitHandler:                          #saves current state of adjacency matrix to json then exits program
    def __init__(self, tuple):
        self.my_tuple = tuple
    def __call__(self, signo, frame):
        print(f'Dumping adjacency matrix to \'adjacent_links\' in current directory.')
        with open(f'{self.my_tuple[0]}/adjacent_links', 'w') as f:
            json.dump(self.my_tuple[1], f)
        os._exit(0)


if __name__ == '__main__':
    main()
